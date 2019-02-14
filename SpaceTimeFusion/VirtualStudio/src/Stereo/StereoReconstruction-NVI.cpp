#include "StereoReconstruction.h"
#include "Disk.h"
#include "Transform-2x2.h"
#include <algorithm>
#include "Timer.h"
#include "ImageProcessing.h"
#include "RegistrationDefs.h"
#include "CCLabeler.h"
#include "PiecesBlender.h"

void StereoReconstruction::ReconstructNV(SR_Params reconstructionParams, bool overwriteResults)
{
	ENSURE(this->rig.cameras.size() > 0);
	this->srParams = reconstructionParams;

	if(overwriteResults == false)
	{		
		Camera *targetCam = this->rig.GetCamera(this->srParams.targetCamID);
		if(Disk::DoesFileExist(GetTargetContextNovelViewFN(targetCam, this->srParams.iFrame, this->rig.params.outDir)) &&
		   Disk::DoesFileExist(GetTargetContextRegFN(targetCam, this->srParams.iFrame, this->rig.params.outDir)))
		{
			return;
		}
	}

	loadContext(false);

	reconstructNV();	

	unloadContext();
}

void StereoReconstruction::reconstructNV()
{
	CByteImage  medianReconImg;
	CByteImage  medianReconImgMask;
	CByteImage  reconstructionMask;
	CShortImage medianPlaneMap;

	reconstructNV_MedianUsingGC(reconstructionMask,
						 medianReconImg, medianReconImgMask, medianPlaneMap);
	//reconstructNV_Median(reconstructionMask,
	//					 medianReconImg, medianReconImgMask, medianPlaneMap);


	//bug
	//reconstructionMask = ImageProcessing::ErodeImage(reconstructionMask, 5, MASK_INVALID);
	reconstructionMask = ImageProcessing::ErodeImage(medianReconImgMask, 5, MASK_INVALID);

	CByteImage noReconstructionMask = ImageProcessing::GetInvertedMask(reconstructionMask);

	RegisteredPiece regPiece;
	regPiece.id = 0;
	regPiece.mask = medianReconImgMask.Clone();
	regPiece.image = ImageProcessing::ByteToFloatImg(medianReconImg);
	regPiece.filteredImage = regPiece.image;
	regPiece.ComputeEdgeImages();
	regPiece.ComputeColDistSqrImage(regPiece.image, 1); //Dummy operation

	RegisteredPiece holePiece;
	holePiece.id = 1;
	holePiece.mask = noReconstructionMask;
	holePiece.image = ImageProcessing::GetFilteredImage(this->targetContextSRS.sourceImg, this->targetContextSRS.sourceMask, true,
													    regPiece.image, regPiece.mask, true, this->srParams.bsParams.holesFilter);
	holePiece.filteredImage = holePiece.image;
	holePiece.ComputeEdgeImages();
	holePiece.ComputeColDistSqrImage(holePiece.image, 1); //Dummy operation

	CFloatImage regImage(regPiece.image.Shape());
	regImage.ClearPixels();

	CByteImage labelsMask = ImageProcessing::ErodeImage(medianReconImgMask, 3, MASK_INVALID);	
	CShape maskShape = medianReconImgMask.Shape();
	CShortImage regLabels(maskShape);
	int nodeCount = maskShape.width * maskShape.height;
	CShape imageShape = regPiece.image.Shape();
	INSIST(imageShape.nBands == holePiece.image.Shape().nBands);
	for(int iNode = 0; iNode < nodeCount; iNode++)
	{
		if(labelsMask[iNode] == MASK_VALID)
		{
			for(int iChannel = 0; iChannel < imageShape.nBands; iChannel++)
			{
				regImage[iNode * imageShape.nBands + iChannel] = 
					regPiece.image[iNode * imageShape.nBands + iChannel];
			}
			regLabels[iNode] = 0;
		}
		else if(noReconstructionMask[iNode] == MASK_VALID)
		{
			for(int iChannel = 0; iChannel < imageShape.nBands; iChannel++)
			{
				regImage[iNode * imageShape.nBands + iChannel] = 
					holePiece.image[iNode * imageShape.nBands + iChannel];
			}
			regLabels[iNode] = 1;
			medianPlaneMap[iNode] = this->targetContextSRS.sourcePlaneMap[iNode];
		}
		else
		{
			regLabels[iNode] = -1;
		}
	}
	
	RegisteredPieces regPieces;
	regPieces.push_back(regPiece);
	regPieces.push_back(holePiece);

	CFloatImage nullImg(regPiece.image.Shape());
	nullImg.ClearPixels();

	CFloatImage blendImg(regPiece.image.Shape());
	BlendSolver blender(regImage, regLabels, regPieces);
	this->srParams.bsParams.fillInHoles = true;
	this->srParams.bsParams.holesFilter = IF_NO_OP;
	this->srParams.bsParams.ignoreHighGradLinks = false;
	this->srParams.bsParams.useStrongBlending = true;
	blender.Solve(blendImg, nullImg, this->srParams.bsParams);

	ImageIO::WriteFile(blendImg, getTargetContextNovelViewFN());

	ImageIO::WriteFile(regImage, getTargetContextRegFN());

	ImageIO::WriteRaw(medianPlaneMap, this->targetContextSRS.sourceCam.GetFrameDepsNameAlt(this->srParams.iFrame));

	CFloatImage newDepthsVis = GetDepthsVis(medianPlaneMap, this->targetContextSRS.sourceCam);
	ImageIO::WriteFile(newDepthsVis, getTargetContextMaskFN());
}

CByteImage StereoReconstruction::constructDepthRangeMask(SRS_Context &srsContext, CVector3 realWorldMin, CVector3 realWorldMax)
{
	const CByteImage &srsMask = srsContext.sourceMask;
	const CShortImage &srsPlaneImg = srsContext.sourcePlaneMap;
	CShape maskShape = srsMask.Shape();
	ENSURE(maskShape == srsPlaneImg.Shape());
	ENSURE((maskShape.width > 0) && (maskShape.height > 0));

	CByteImage depthRangeMask(maskShape);
	depthRangeMask.ClearPixels();

	const Camera &orgSrsCam = this->rig.cameras[srsContext.iSourceCam];
	CVector3 srsWorldMin = orgSrsCam.cameraMats.transformCoordSys(realWorldMin);
	CVector3 srsWorldMax = orgSrsCam.cameraMats.transformCoordSys(realWorldMax);

	uint nodeAddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	for(int x = 0; x < maskShape.width;  x++, nodeAddr++)
	{
		if(srsMask[nodeAddr] == MASK_INVALID) continue;

		int srsPlaneIndex = srsPlaneImg[nodeAddr];
		if(srsPlaneIndex == SEGMENT_OccDepPlaneIndex) continue;

		int srsFrontoPlaneIndex = getFrontoPlaneIndex(x, y, srsContext, srsPlaneIndex, false);
		if(srsFrontoPlaneIndex == SEGMENT_OccDepPlaneIndex) continue;

		double pixelDepth = srsContext.sourceCam.frontoPlanes[srsFrontoPlaneIndex];
		if(InRange(pixelDepth, srsWorldMin[2], srsWorldMax[2]))
		{
			depthRangeMask[nodeAddr] = MASK_VALID;
		}
	}

	return depthRangeMask;
}

///*
void StereoReconstruction::reconstructNV_Median(CByteImage &reconstructionMask,
												CByteImage &medianReconImg, CByteImage &medianReconImgMask, CShortImage &medianPlaneMap)
{
	int erosionMaskWidth = 11;

	//CVector3 firstCamWorldMin(0.0, 0.0, this->rig.cameras[0].frontoPlanes[0]);
	//CVector3 firstCamWorldMax(0.0, 0.0, this->rig.cameras[0].frontoPlanes[0]);
	//CVector3 realWorldMin = this->rig.cameras[0].cameraMats.invertCoordSys(firstCamWorldMin);
	//CVector3 realWorldMax = this->rig.cameras[0].cameraMats.invertCoordSys(firstCamWorldMax);
	
	//reconstructionMask = constructDepthRangeMask(this->targetContextSRS, realWorldMin, realWorldMax);
	//reconstructionMask = ImageProcessing::ErodeImage(reconstructionMask, erosionMaskWidth + 6, MASK_VALID);

	reconstructionMask.ReAllocate(this->targetContextSRS.sourceMask.Shape());
	ImageProcessing::Operate(reconstructionMask, reconstructionMask, MASK_VALID, ImageProcessing::OP_SET);
	CShape reconstructionMaskShape = reconstructionMask.Shape();

	//ImageIO::WriteFile(reconstructionMask, getTargetContextMaskFN());	
	//CByteImage noReconstructionMask = ImageProcessing::GetInvertedMask(reconstructionMask);
	//CFloatImage maskedImg = ImageProcessing::GetMaskedImage(this->targetContextSRS.sourceImg, noReconstructionMask);
	//ImageIO::WriteFile(maskedImg, getTargetContextRegFN());
	//exit(0);

	vector<CByteImage>  reconImgs;
	vector<CByteImage>  reconMasks;
	vector<CShortImage> reconPlaneMaps;
	vector< std::pair<float, int> >     reconSourceDists;

	printf("Creating reg pieces... \n"); fflush(stdout);
	for(uint iSRS = 0; iSRS < this->srParams.srsCamIDs.size(); iSRS++)
	{	
		SRS_Context srsContext = loadSRS_Context(this->srParams.srsCamIDs[iSRS], true,
												 true, this->srParams.reconstructUsingAnalogy,
												 this->refCamMats, true,
												 this->srParams.srsCamScales[iSRS]);

		//srsContext.sourceMask = constructDepthRangeMask(srsContext, realWorldMin, realWorldMax);
		//srsContext.sourceMask = ImageProcessing::ErodeImage(srsContext.sourceMask, erosionMaskWidth, MASK_VALID);
		//srsContext.sourceMask = ImageProcessing::GetInvertedMask(srsContext.sourceMask);

		CFloatImage projImage, projAnalogyImage;
		CByteImage projMask;
		CShortImage projPlaneMap;

		createViewProjection(srsContext, this->targetContextSRS, false,
		 					 projImage, projAnalogyImage, projMask, projPlaneMap);

		int nodeCount = reconstructionMaskShape.width * reconstructionMaskShape.height;
		for(int iNode = 0; iNode < nodeCount; iNode++)
		{
			if(reconstructionMask[iNode] == MASK_INVALID)
			{
				projMask[iNode]     = MASK_INVALID;
				projPlaneMap[iNode] = SEGMENT_OccDepPlaneIndex;
			}
		}
		projImage = ImageProcessing::GetMaskedImage(projImage, projMask);

		CByteImage reconImg = ImageProcessing::FloatToByteImg(projImage);
		reconImgs.push_back(reconImg);
		reconMasks.push_back(projMask);
		reconPlaneMaps.push_back(projPlaneMap); 

		//float dist = abs(srsContext.iSourceCam - this->targetContextSRS.iSourceCam);
		float dist = (float) (srsContext.sourceCam.cameraMats.GetWorldCoordCoP() - 
					  this->targetContextSRS.sourceCam.cameraMats.GetWorldCoordCoP()).Length();
		std::pair<float, int>  reconSourceDist(dist, iSRS);
		reconSourceDists.push_back(reconSourceDist);

		unloadSRS_Context(srsContext);
	}
	printf("\n\tDone.\n"); fflush(stdout);

	std::sort(reconSourceDists.begin(), reconSourceDists.end());
	vector<CByteImage> reconImgsTemp       = reconImgs;
	vector<CByteImage> reconMasksTemp      = reconMasks;
	vector<CShortImage> reconPlaneMapsTemp = reconPlaneMaps;
	reconImgs.clear();
	reconMasks.clear();
	reconPlaneMaps.clear();
	for(uint iRecon = 0; iRecon < reconSourceDists.size(); iRecon++)
	{
		int iReconIndex = reconSourceDists[iRecon].second;
		reconImgs.push_back(reconImgsTemp[iReconIndex]);
		reconMasks.push_back(reconMasksTemp[iReconIndex]);
		reconPlaneMaps.push_back(reconPlaneMapsTemp[iReconIndex]);
	}

	//bug
	ImageProcessing::ComputeMedian(reconImgs, reconMasks, 
		                           medianReconImg, medianReconImgMask, 15);

	ImageProcessing::ComputeMedian(reconPlaneMaps, reconMasks, 
								   medianPlaneMap, medianReconImgMask, 15);
}
//*/


void StereoReconstruction::reconstructNV_MedianUsingGC(CByteImage &reconstructionMask,
													   CByteImage &medianReconImg, CByteImage &medianReconImgMask, CShortImage &medianPlaneMap)
{
	int erosionMaskWidth = 11;

	//CVector3 firstCamWorldMin(0.0, 0.0, this->rig.cameras[0].frontoPlanes[0]);
	//CVector3 firstCamWorldMax(0.0, 0.0, this->rig.cameras[0].frontoPlanes[0]);
	//CVector3 realWorldMin = this->rig.cameras[0].cameraMats.invertCoordSys(firstCamWorldMin);
	//CVector3 realWorldMax = this->rig.cameras[0].cameraMats.invertCoordSys(firstCamWorldMax);
	//
	//reconstructionMask = constructDepthRangeMask(this->targetContextSRS, realWorldMin, realWorldMax);
	reconstructionMask.ReAllocate(this->targetContextSRS.sourceMask.Shape());
	ImageProcessing::Operate(reconstructionMask, reconstructionMask, MASK_VALID, ImageProcessing::OP_SET);
	CShape reconstructionMaskShape = reconstructionMask.Shape();

	//check
	//reconstructionMask = ImageProcessing::ErodeImage(reconstructionMask, erosionMaskWidth + 6, MASK_VALID);

	//ImageIO::WriteFile(reconstructionMask, getTargetContextMaskFN());	
	//CByteImage noReconstructionMask = ImageProcessing::GetInvertedMask(reconstructionMask);
	//CFloatImage maskedImg = ImageProcessing::GetMaskedImage(this->targetContextSRS.sourceImg, noReconstructionMask);
	//ImageIO::WriteFile(maskedImg, getTargetContextRegFN());
	//exit(0);

	RegisteredPieces regPieces;	
	vector<CByteImage> reconMasks;
	vector<CShortImage> reconPlaneMaps;
	vector<CFloatImage> reconImgs;
	vector<IntPair>     reconSourceDists;


	printf("Creating reg pieces... \n"); fflush(stdout);
	for(uint iSRS = 0; iSRS < this->srParams.srsCamIDs.size(); iSRS++)
	{	
		SRS_Context srsContext = loadSRS_Context(this->srParams.srsCamIDs[iSRS], true,
												 false, this->srParams.reconstructUsingAnalogy,
												 this->refCamMats, true,
												 this->srParams.srsCamScales[iSRS]);

		//srsContext.sourceMask = constructDepthRangeMask(srsContext, realWorldMin, realWorldMax);
		//srsContext.sourceMask = ImageProcessing::ErodeImage(srsContext.sourceMask, erosionMaskWidth, MASK_VALID);
		//srsContext.sourceMask = ImageProcessing::GetInvertedMask(srsContext.sourceMask);

		CFloatImage projImage, projAnalogyImage;
		CByteImage projMask;
		CShortImage projPlaneMap;

		createViewProjection(srsContext, this->targetContextSRS, false,
 							 projImage, projAnalogyImage, projMask, projPlaneMap);
		
		int nodeCount = reconstructionMaskShape.width * reconstructionMaskShape.height;
		for(int iNode = 0; iNode < nodeCount; iNode++)
		{
			if(reconstructionMask[iNode] == MASK_INVALID)
			{
				projMask[iNode]     = MASK_INVALID;
				projPlaneMap[iNode] = SEGMENT_OccDepPlaneIndex;
			}
		}
		projImage = ImageProcessing::GetMaskedImage(projImage, projMask);

		reconImgs.push_back(projImage);
		reconMasks.push_back(projMask);
		reconPlaneMaps.push_back(projPlaneMap); 

		RegisteredPiece regPiece;
		regPiece.id = srsContext.iSourceCam;
		regPiece.filteredImage = ImageProcessing::TransferColor(projImage, this->targetContextSRS.sourceImg);
		regPiece.image = projImage;
		regPiece.mask = projMask;
		regPiece.planeMap = projPlaneMap;
		regPieces.push_back(regPiece);

		IntPair reconSourceDist(abs(srsContext.iSourceCam - this->targetContextSRS.iSourceCam), iSRS);
		reconSourceDists.push_back(reconSourceDist);

		unloadSRS_Context(srsContext);
	}
	printf("\n\tDone.\n"); fflush(stdout);

	ImageProcessing::ComputeMedian(reconPlaneMaps, reconMasks, 
								   medianPlaneMap, medianReconImgMask);	

	CFloatImage medianReconImgFlt;
	ImageProcessing::ComputeMedian(reconImgs, reconMasks, 
								   medianReconImgFlt, medianReconImgMask);

	for(uint iPiece = 0; iPiece < regPieces.size(); iPiece++)
	{
		RegisteredPiece &regPiece = regPieces[iPiece];

		//CShape regMaskShape = regPiece.mask.Shape();
		//int nodeCount = regMaskShape.width * regMaskShape.height;
		//for(int iNode = 0; iNode < nodeCount; iNode++)
		//{
		//	if(regPiece.mask[iNode] == MASK_VALID)
		//	{
		//		if(abs(regPiece.planeMap[iNode] - medianPlaneMap[iNode]) > this->srParams.depthMatchThres)
		//		{
		//			regPiece.mask[iNode] = MASK_INVALID;				
		//		}
		//	}
		//}

		//regPiece.ComputeEdgeImages();
		//regPiece.ComputeColDistSqrImage(medianReconImgFlt, 1);

		regPiece.ComputeEdgeImages();
		regPiece.ComputeColDistSqrImage(this->targetContextSRS.sourceImg, 1);
	}

	CFloatImage reconImg;
	CShortImage reconLabels;
	PiecesBlender blender(this->targetContextSRS.sourceImg, regPieces);
	blender.Blend(reconImg, reconLabels, this->srParams.pbParams);
	ImageIO::WriteFile(reconImg, getTargetContextRegFN());


	CFloatImage blendImg(reconImg.Shape());
	BlendSolver bsSolver(reconImg, reconLabels, regPieces);
	BlendSolver::BlendSolverParams bsParams = this->srParams.bsParams;
	bsParams.fillInHoles = false;
	bsSolver.Solve(blendImg, this->targetContextSRS.sourceImg, bsParams);

	medianReconImg = ImageProcessing::FloatToByteImg(blendImg);
}



void StereoReconstruction::createViewProjection(SRS_Context &srcSRS, SRS_Context &targetSRS, bool matchTargetPlaneMap,
												CFloatImage &projImg, CFloatImage &projAnalogyImg, CByteImage &projMask, CShortImage &projPlanes)

{
	CFloatImage &srcImg        = srcSRS.sourceImg;
	CFloatImage &srcAnalogyImg = srcSRS.sourceAnalogyImg;
	CShape srcShape            = srcImg.Shape();
	CShortImage &srcPlaneMap   = srcSRS.sourcePlaneMap;	
	CByteImage &srcMask        = srcSRS.sourceMask;

	CShape targetShape = targetSRS.sourceImg.Shape();
	INSIST(targetShape.nBands == srcShape.nBands);
	CShape targetMaskShape(targetShape.width, targetShape.height, 1);

	CameraMats srcCMs    = srcSRS.sourceCam.cameraMats;
	CameraMats targetCMs = targetSRS.sourceCam.cameraMats;
	srcCMs.Recenter(srcSRS.sourceCam.cameraMats);
	targetCMs.Recenter(srcSRS.sourceCam.cameraMats);

	//CImageOf< vector<ZPixel> > zBuffer(targetMaskShape);
	vector< vector<ZPixel> > zBuffer(targetShape.width * targetShape.height); 
	INSIST(zBuffer.size() == (targetShape.width * targetShape.height));	
	ENSURE(targetShape.nBands == 3);

	uint srsMaskAddr  = 0;
	uint srsPixelAddr = 0;
	for(int v = 0; v < srcShape.height; v++)
	for(int u = 0; u < srcShape.width;  u++, srsMaskAddr++, srsPixelAddr += srcShape.nBands)
	{
		if(srcMask[srsMaskAddr] == MASK_INVALID) continue;

		short srsPlaneIndex = srcSRS.sourcePlaneMap[srsMaskAddr];
		CHECK_RANGE(srsPlaneIndex, 0, srcSRS.sourceCam.planeCount - 1);
		const Plane &srsPlane = srcSRS.sourceCam.planes[srsPlaneIndex];

		CVector2 srsUV((double) u, (double) v);
		CVector3 srsWorldXYZ;
		srcCMs.projUVtoXYZ(srsUV, srsPlane, srsWorldXYZ);
		CVector2 targetUV;
		targetCMs.projXYZtoUV(srsWorldXYZ, targetUV);
		CVector3 targetWorldXYZ = targetCMs.transformCoordSys(srsWorldXYZ);	

		if(targetShape.InBounds(targetUV[0], targetUV[1]))
		{
			ZPixel zPixel;
			zPixel.weight = 1.0; // bug - using matting here

			if(srsPlaneIndex < this->scenePlaneCount)
				zPixel.planeIndex = srsPlaneIndex;
			else
				zPixel.planeIndex = getFrontoPlaneIndex(u, v, srcSRS, srsPlaneIndex, true, targetSRS);

			for(int iBand = 0; iBand < targetShape.nBands; iBand++)
			{
				zPixel.color[iBand]        = srcImg[srsPixelAddr + iBand];
				zPixel.analogyColor[iBand] = srcAnalogyImg[srsPixelAddr + iBand];
			}
			
			zPixel.depth = (float) targetWorldXYZ[2];

			if(InRange(zPixel.depth, 
				       (float) targetSRS.sourceCam.minDepth, 
					   (float) targetSRS.sourceCam.maxDepth) == false)
			{
				continue;
			}
			
			int floorTargetU = (int) targetUV[0];
			int roofTargetU  = (int) (targetUV[0] + 1.0);

			int floorTargetV = (int) targetUV[1];
			int roofTargetV  = (int) (targetUV[1] + 1.0);

			float uRoofFrac  = (float) (targetUV[0] - floorTargetU);
		  	float vRoofFrac  = (float) (targetUV[1] - floorTargetV);
			float uFloorFrac = 1.0f - uRoofFrac;
			float vFloorFrac = 1.0f - vRoofFrac;

			for(int iNeigh = 0; iNeigh < 4; iNeigh++)
			{
				int neighU = INT_MAX, neighV = INT_MAX;
				float neighWeightFrac = 0.0f;

				switch(iNeigh)
				{
				case 0:
					neighU = floorTargetU; neighV = floorTargetV;
					neighWeightFrac = uFloorFrac * vFloorFrac;
					break;
				case 1:
					neighU = floorTargetU; neighV = roofTargetV;
					neighWeightFrac = uFloorFrac * vRoofFrac;
					break;
				case 2:
					neighU = roofTargetU;  neighV = floorTargetV;
					neighWeightFrac = uRoofFrac * vFloorFrac;
					break;
				case 3:
					neighU = roofTargetU;  neighV = roofTargetV;
					neighWeightFrac = uRoofFrac * vRoofFrac;
					break;

				default:
					REPORT_FAILURE("Unknown neigh type");
					break;
				}

				if(neighWeightFrac > FLT_EPSILON)
				{					
					ENSURE(targetShape.InBounds(neighU, neighV));

					ZPixel neighZPixel = zPixel;
					neighZPixel.weight *= neighWeightFrac;
					
					//zBuffer.Pixel(neighU, neighV, 0).push_back(neighZPixel);
					zBuffer[(neighV * targetShape.width) + neighU].push_back(neighZPixel);
				}
			}
		}
	}

	projImg.ReAllocate(targetShape);
	projAnalogyImg.ReAllocate(targetShape);
	projMask.ReAllocate(targetMaskShape);
	projPlanes.ReAllocate(targetMaskShape);

	int validPixelCount = extractProjDataFromZBuffer(zBuffer, targetSRS, matchTargetPlaneMap, 
											  	     projImg, projAnalogyImg, projMask, projPlanes);
	//if(validPixelCount > 0)
	//{
	//	projImg = ImageProcessing::TransferColor(projImg, this->targetContextSRS.sourceImg, 
	//											 projMask, true, 
	//											 this->targetContextSRS.sourceMask, true);
	//}
}


int StereoReconstruction::extractProjDataFromZBuffer(const vector< vector<ZPixel> > &zBuffer, SRS_Context &targetContext, bool matchTargetPlaneMap,
													  CFloatImage &projImg, CFloatImage &projAnalogyImg, CByteImage &projMask, CShortImage &projPlanes)
{
	int validPixelCount = 0;
	
	CShape imgShape = projImg.Shape();
	INSIST(imgShape == projAnalogyImg.Shape());
	CShape maskShape = projMask.Shape();
	INSIST(maskShape == projPlanes.Shape());
	INSIST(maskShape.nBands == 1);
	INSIST(zBuffer.size() == (maskShape.width * maskShape.height));

	const CShortImage &targetPlaneMap = targetContext.sourcePlaneMap;
	if(matchTargetPlaneMap)
	{
		INSIST(targetPlaneMap.Shape() == maskShape);
	}

	projImg.ClearPixels();
	projAnalogyImg.ClearPixels();
	projMask.ClearPixels();
	ImageProcessing::Operate(projPlanes, projPlanes, (short) SEGMENT_OccDepPlaneIndex, ImageProcessing::OP_SET);

	vector< std::pair<float, int> > zPixelIndices;

	uint pixelAddr = 0;
	uint maskAddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	for(int x = 0; x < maskShape.width;  x++, maskAddr++, pixelAddr += imgShape.nBands)
	{
		const vector<ZPixel> &currZPixels = zBuffer[maskAddr];
		int zPixelCount = (int) currZPixels.size(); 
		if(zPixelCount == 0) continue;

		bool useProjData;
		int targetFrontoPlaneIndex;
		if(!matchTargetPlaneMap)
		{
			useProjData = true;
			targetFrontoPlaneIndex = SEGMENT_OccDepPlaneIndex;
		}
		else
		{
			int targetPlaneIndex = targetPlaneMap[maskAddr];
			if(targetPlaneIndex == SEGMENT_OccDepPlaneIndex) continue;

			targetFrontoPlaneIndex = getFrontoPlaneIndex(x, y, targetContext, targetPlaneIndex, false);
			if(targetFrontoPlaneIndex == SEGMENT_OccDepPlaneIndex) continue;

			useProjData = false;
		}
		
		zPixelIndices.resize(zPixelCount);
		for(int iZPixel = 0; iZPixel < zPixelCount; iZPixel++)
		{
			zPixelIndices[iZPixel] = std::make_pair(currZPixels[iZPixel].depth, iZPixel);
		}
		std::sort(zPixelIndices.begin(), zPixelIndices.end());

		Vector3<float> projColor(0.0f, 0.0f, 0.0f);
		Vector3<float> projAnalogyColor(0.0f, 0.0f, 0.0f);
		float pixelFillFrac = 0.0f;
		//bug
		//for(int iZPixel = 0; (iZPixel < zPixelCount) && (pixelFillFrac < 1.0f); iZPixel++)
		for(int iZPixel = 0; iZPixel < std::min(1, zPixelCount); iZPixel++)
		{
			int zPixelIndex = zPixelIndices[iZPixel].second;

			float zPixelWeigh = currZPixels[zPixelIndex].weight;
			SET_IF_LT(zPixelWeigh, 1.0f - pixelFillFrac);
			pixelFillFrac += zPixelWeigh;

			projColor        = projColor        + (currZPixels[zPixelIndex].color        * zPixelWeigh);
			projAnalogyColor = projAnalogyColor + (currZPixels[zPixelIndex].analogyColor * zPixelWeigh);

			if(matchTargetPlaneMap)
			{			
				int projFrontoPlaneIndex = targetContext.sourceCam.GetBestFrontoPlaneIndex(currZPixels[zPixelIndex].depth);
				if(abs(projFrontoPlaneIndex - targetFrontoPlaneIndex) <= this->srParams.depthMatchThres)
				{
					useProjData = true;
				}			
			}
		}

		if(useProjData == true)
		{			
			ENSURE(pixelFillFrac != 0.0f);
			for(int iBand = 0; iBand < imgShape.nBands; iBand++)
			{
				projImg[pixelAddr + iBand]        = projColor[iBand] / pixelFillFrac;
				projAnalogyImg[pixelAddr + iBand] = projAnalogyColor[iBand] / pixelFillFrac;
			}

			int frontZPixelIndex = zPixelIndices[0].second;
			projPlanes[maskAddr] = currZPixels[frontZPixelIndex].planeIndex;

			projMask[maskAddr] = MASK_VALID;

			validPixelCount++;
		}

	}

	return validPixelCount;
}

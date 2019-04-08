#include "StereoReconstruction.h"
#include "ImageIO.h"
#include "ImageProcessing.h"
#include "PiecesBlender.h"
#include "CCLabeler.h"
#include "BlendSolver.h"
#include "FlowHelper.h"
#include "Draw.h"
#include "Disk.h"
#include "Timer.h"


void StereoReconstruction::ComputeMatting(SR_Params reconstructionParams, bool overwriteResults)
{
	ENSURE(this->rig.cameras.size() > 0);
	this->srParams = reconstructionParams;

	if(overwriteResults == false)
	{		
		Camera *targetCam = this->rig.GetCamera(this->srParams.targetCamID);
		//if(Disk::DoesFileExist(GetTargetContextRegFN(targetCam, this->srParams.iFrame, this->rig.params.outDir)))
		//{
		//	return;
		//}
	}

	loadContext(true);

	computeMatte(this->targetContextSRS, reconstructionParams);

	unloadContext();
}

void StereoReconstruction::Reconstruct(SR_Params reconstructionParams, bool overwriteResults)
{
	ENSURE(this->rig.cameras.size() > 0);
	this->srParams = reconstructionParams;

	if(overwriteResults == false)
	{		
		Camera *targetCam = this->rig.GetCamera(this->srParams.targetCamID);
		if(Disk::DoesFileExist(GetTargetContextRegFN(targetCam, this->srParams.iFrame, this->rig.params.outDir)) &&
		   Disk::DoesFileExist(GetTargetContextBlendFN(targetCam, this->srParams.iFrame, this->rig.params.outDir)))
		{
			return;
		}
	}

	loadContext(true);

	reconstruct();

	unloadContext();
}

void StereoReconstruction::ReconstructMask(SR_Params reconstructionParams, bool overwriteResults)
{
	ENSURE(this->rig.cameras.size() > 0);
	this->srParams = reconstructionParams;

	if(overwriteResults == false)
	{
		Camera *targetCam = this->rig.GetCamera(this->srParams.targetCamID);
		if(Disk::DoesFileExist(GetTargetContextMaskFN(targetCam, this->srParams.iFrame, this->rig.params.outDir)))	
		{
			return;
		}
	}

	this->srParams.neighCamIDs.clear();
	this->srParams.neighCamCropExts.clear();
	this->srParams.neighCamCropOrigins.clear();
	this->srParams.neighCamScales.clear();

	loadContext(true);

	reconstructMask();

	unloadContext();
}

void StereoReconstruction::computeSRS_Mask(SRS_Context &srsContext)
{
	if(this->srParams.depthDisConMaskErWidth == 0) return;

	CShortImage &srsPlaneMap = srsContext.sourcePlaneMap;	
	CByteImage  &srsMask     = srsContext.sourceMask;
	CShape maskShape         = srsMask.Shape();

	ENSURE(maskShape == srsPlaneMap.Shape());
	ImageProcessing::Operate(srsMask, srsMask, MASK_VALID, ImageProcessing::OP_SET);

	const int connectivity = 2;
	const int dx[connectivity] = {-1,  0};
	const int dy[connectivity] = { 0, -1};
	
	uint maskAddr  = 0;
	for(int y = 0; y < maskShape.height; y++)
	for(int x = 0; x < maskShape.width;  x++, maskAddr++)
	{
		int planeIndex = srsPlaneMap[maskAddr];
		CHECK_RANGE(planeIndex, SEGMENT_OccDepPlaneIndex, srsContext.sourceCam.planeCount - 1);
		int frontoPlaneIndex = getFrontoPlaneIndex(x, y, srsContext, planeIndex, false);
		
		if(frontoPlaneIndex == SEGMENT_OccDepPlaneIndex)
			srsMask[maskAddr] = MASK_INVALID;
		else
		{
			for(int iNeighbour = 0; iNeighbour < connectivity; iNeighbour++)
			{
				int neighbourX = x + dx[iNeighbour];
				int neighbourY = y + dy[iNeighbour];
				if(maskShape.InBounds(neighbourX, neighbourY) == false) continue;

				int neighPlaneIndex = srsPlaneMap.Pixel(neighbourX, neighbourY, 0);
				CHECK_RANGE(neighPlaneIndex, SEGMENT_OccDepPlaneIndex, srsContext.sourceCam.planeCount - 1);

				if(neighPlaneIndex != planeIndex)
				{
					int neighFrontoPlaneIndex = getFrontoPlaneIndex(neighbourX, neighbourY, srsContext, neighPlaneIndex, false);
					if((neighFrontoPlaneIndex == SEGMENT_OccDepPlaneIndex) ||
					   (abs(neighFrontoPlaneIndex - frontoPlaneIndex) > this->srParams.depthDisConMaskThres))
					{
						srsMask[maskAddr] = MASK_INVALID;
					}
				}
			}
		}
	}

	int erosionWidth = (int) (this->srParams.depthDisConMaskErWidth * srsContext.scale);
	erosionWidth += (erosionWidth % 2 == 0) ? 1 : 0;
	
	srsMask = ImageProcessing::ErodeImage(srsMask, erosionWidth, MASK_INVALID);
}

void StereoReconstruction::reconstruct()
{
	RegisteredPieces regPieces;
	printf("Creating reg pieces... \n"); fflush(stdout);
	for(uint iSRS = 0; iSRS < this->srParams.srsCamIDs.size(); iSRS++)
	{	
		SRS_Context srsContext = loadSRS_Context(this->srParams.srsCamIDs[iSRS], true, 
												 true, this->srParams.reconstructUsingAnalogy,
												 this->refCamMats, true,
			                                     this->srParams.srsCamScales[iSRS]);
		RegisteredPiece regPiece = createRegisteredPiece(srsContext, this->targetContextSRS, false);
		regPieces.push_back(regPiece);

		if(srsContext.iSourceCam != this->targetContextSRS.iSourceCam)
		{
			unloadSRS_Context(srsContext);
		}
	}

	updateFillerSRS_Masks(regPieces);
	printf("\n\tDone.\n"); fflush(stdout);

	CFloatImage reconImg;
	CShortImage reconLabels;
	PiecesBlender blender(this->targetContextSRS.sourceAnalogyImg, regPieces);
	blender.Blend(reconImg, reconLabels, this->srParams.pbParams);
	
	CShape reconImgShape = reconImg.Shape();
	CByteImage reconMask(reconImgShape.width, reconImgShape.height, 1);
	int nodeCount = reconImgShape.width * reconImgShape.height;
	for(int iNode = 0; iNode < nodeCount; iNode++)
	{
		if(reconLabels[iNode] != -1)
		{
			reconMask[iNode] = MASK_VALID;
		}
		else
		{
			reconMask[iNode] = MASK_INVALID;
			for(int iChannel = 0; iChannel < reconImgShape.nBands; iChannel++)
			{
				int pixelAddr = (iNode * reconImgShape.nBands) + iChannel;
				reconImg[pixelAddr] = this->targetContextSRS.sourceImg[pixelAddr];
			}
		}
	}

	ImageIO::WriteFile(reconImg, getTargetContextRegFN());
	ImageIO::WriteFile(reconMask, getTargetContextMaskFN());	
	
	CFloatImage blendImg(reconImg.Shape());
	printf("\nPoisson blending...\n");
	BlendSolver blendSolver(reconImg, reconLabels, regPieces);
	blendSolver.Solve(blendImg, this->targetContextSRS.sourceAnalogyImg, this->srParams.bsParams);
	ImageIO::WriteFile(blendImg, getTargetContextBlendFN());


	//CShortImage srcLabels(reconLabels.Shape());
	//for(int iNode = 0; iNode < nodeCount; iNode++)
	//{
	//	int reconLabel = reconLabels[iNode];
	//	if(reconLabel == -1)
	//		srcLabels[iNode] = -1;
	//	else
	//	{
	//		srcLabels[iNode] = regPieces[reconLabel].id;
	//	}
	//}
	//vector<CVector3> srcColors;
	//srcColors.push_back(CVector3(1.0,  0.0,  0.0));   //IMG_0632
	//srcColors.push_back(CVector3(0.0,  1.0,  0.0));   //IMG_0633
	//srcColors.push_back(CVector3(0.0,  0.0,  1.0));   //IMG_0634
	//srcColors.push_back(CVector3(0.0,  0.0,  1.0));   //IMG_0642
	//srcColors.push_back(CVector3(1.0,  1.0,  0.0));   //IMG_0637
	//srcColors.push_back(CVector3(1.0,  1.0,  0.0));   //IMG_0635
	//srcColors.push_back(CVector3(0.0,  1.0,  1.0));   //IMG_0636
	//srcColors.push_back(CVector3(0.0,  1.0,  1.0));   //IMG_0638
	//srcColors.push_back(CVector3(0.0,  1.0,  1.0));   //IMG_0639
	//srcColors.push_back(CVector3(1.0,  0.0,  1.0));   //IMG_0645
	//srcColors.push_back(CVector3(1.0,  0.0,  1.0));   //IMG_0643
	//srcColors.push_back(CVector3(0.33, 0.66, 1.0));   //IMG_0646
	//srcColors.push_back(CVector3(0.33, 0.66, 1.0));   //IMG_0647
	//srcColors.push_back(CVector3(0.66, 1.0,  0.33));  //IMG_0644
	//srcColors.push_back(CVector3(0.66, 0.33, 1.0));   //IMG_0649
	//srcColors.push_back(CVector3(0.66, 0.33, 1.0));   //IMG_0648
	//srcColors.push_back(CVector3(1.0,  0.33, 0.66));  //IMG_0650
	//srcColors.push_back(CVector3(1.0,  0.66, 0.33));  //IMG_0651		
	//CFloatImage sourcesImg = ImageProcessing::GetSourcesImage(srcLabels, 18, srcColors, true);
	//ImageIO::WriteFile(sourcesImg, getTargetContextNovelViewFN());
}

void StereoReconstruction::updateFillerSRS_Masks(RegisteredPieces &regPieces)
{
	ENSURE(regPieces.size() == this->srParams.srsCamFillerOnly.size());

	CShape targetMaskShape = this->targetContextSRS.sourceMask.Shape();
	CByteImage nonFillerMask(targetMaskShape); 
	nonFillerMask.ClearPixels();

	int nodeCount = targetMaskShape.width * targetMaskShape.height;

	for(uint iReg = 0; iReg < regPieces.size(); iReg++)
	{
		if(this->srParams.srsCamFillerOnly[iReg] == true) continue;

		const CByteImage &regMask = regPieces[iReg].mask;
		for(int iNode = 0; iNode < nodeCount; iNode++)
		{
			SET_IF_GT(nonFillerMask[iNode], regMask[iNode]);			
		}
	}

	#if defined(__PC__) || defined(__DT__) || defined(__PF__)
		nonFillerMask = ImageProcessing::ErodeImage(nonFillerMask, 3, MASK_INVALID);

		RegisteredPiece viewPiece;		
		
		viewPiece.filteredImage = ImageProcessing::GetFilteredImage(this->targetContextSRS.sourceImg, this->targetContextSRS.sourceMask, true,
																    regPieces[0].image, regPieces[0].mask, true,
																	this->srParams.bsParams.holesFilter);
		viewPiece.image = viewPiece.filteredImage;
		viewPiece.mask.ReAllocate(targetMaskShape);
		viewPiece.mask.ClearPixels();
		for(int iNode = 0; iNode < nodeCount; iNode++)
		{
			if(nonFillerMask[iNode] == MASK_INVALID)
				viewPiece.mask[iNode] = MASK_VALID;			
		}
		viewPiece.colDistSqrImg.ReAllocate(targetMaskShape);
		viewPiece.colDistSqrImg.ClearPixels();
		viewPiece.ComputeEdgeImages();
		regPieces.push_back(viewPiece);
	#endif

	nonFillerMask = ImageProcessing::ErodeImage(nonFillerMask, 9, MASK_INVALID);

	for(uint iReg = 0; iReg < regPieces.size(); iReg++)
	{
		if(this->srParams.srsCamFillerOnly[iReg] == false) continue;

		CByteImage &regMask = regPieces[iReg].mask;
		for(int iNode = 0; iNode < nodeCount; iNode++)
		{
			if(nonFillerMask[iNode] == MASK_VALID)
				regMask[iNode] = MASK_INVALID;			
		}
	}
}


void StereoReconstruction::reconstructMask()
{
	ENSURE(this->srParams.reconstructUsingAnalogy == true);

	RegisteredPieces regPieces;
	printf("Creating reg pieces... \n"); fflush(stdout);
	for(uint iSRS = 0; iSRS < this->srParams.srsCamIDs.size(); iSRS++)
	{	
		SRS_Context srsContext = loadSRS_Context(this->srParams.srsCamIDs[iSRS], true,
												 true, true,
												 this->refCamMats, true,
			                                     this->srParams.srsCamScales[iSRS]);

		RegisteredPiece regPiece = createRegisteredPiece(srsContext, this->targetContextSRS, false);

		regPieces.push_back(regPiece);

		unloadSRS_Context(srsContext);
	}
	printf("\n\tDone.\n"); fflush(stdout);

	CShape imgShape = this->targetContextSRS.sourceImg.Shape();
	CFloatImage targetMaskImg(imgShape);
	targetMaskImg.ClearPixels();

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, nodeAddr++)
	for(int band = 0; band < imgShape.nBands; band++, pixelAddr++)
	{
		for(uint iReg = 0; iReg < regPieces.size(); iReg++)
		{
			if(regPieces[iReg].mask[nodeAddr] == MASK_VALID)
			{
				SET_IF_GT(targetMaskImg[pixelAddr], regPieces[iReg].image[pixelAddr]);
			}
		}
		SET_IF_LT(targetMaskImg[pixelAddr], 1.0f);
		SET_IF_GT(targetMaskImg[pixelAddr], 0.0f);
	}

	ImageIO::WriteFile(targetMaskImg, getTargetContextMaskFN());	

	//CByteImage targetMask = ImageProcessing::FloatToByteImg(targetMaskImg);
	//CByteImage targetMaskInv = ImageProcessing::GetInvertedMask(targetMask);
	//CFloatImage targetHoleImg = ImageProcessing::GetMaskedImage(this->targetContextSRS.sourceImg, targetMaskInv);
	//ImageIO::WriteFile(targetHoleImg, getTargetContextBlendFN());

	CFloatImage grayScaleImg = ColorHelper::RGBtoLuminance(this->targetContextSRS.sourceImg);
	CFloatImage grayScaleImgWithMultiChannels(this->targetContextSRS.sourceImg.Shape());
	for(int iBand = 0; iBand < this->targetContextSRS.sourceImg.Shape().nBands; iBand++)
	{
		ImageProcessing::CopyChannel(grayScaleImgWithMultiChannels, grayScaleImg, iBand, 0);
	}
	
	//CFloatImage saturatedImg;
	//ImageIO::ReadFile(saturatedImg, this->rig.GetRigDirName() + this->targetContextSRS.sourceCam.id + ".tif");

	CFloatImage compositeImg = ImageProcessing::GetCompositeImage(grayScaleImgWithMultiChannels, 
																  this->targetContextSRS.sourceImg,
																  targetMaskImg);
	ImageIO::WriteFile(compositeImg, getTargetContextBlendFN());


	compositeImg = ImageProcessing::GetCompositeImage(this->targetContextSRS.sourceImg, 
													  grayScaleImgWithMultiChannels,
													  targetMaskImg);
	ImageIO::WriteFile(compositeImg, getTargetContextRegFN());
}

int StereoReconstruction::getFrontoPlaneIndex(double x, double y, const SRS_Context &srsContext, int planeIndex, 
											  bool returnPlaneIndexInTargetSpace, const SRS_Context &targetContext)
{
	if(planeIndex == SEGMENT_OccDepPlaneIndex)
		return SEGMENT_OccDepPlaneIndex;

	int frontoPlaneIndex;
	if(planeIndex >= this->scenePlaneCount)
		frontoPlaneIndex = planeIndex - this->scenePlaneCount;
	else
	{
		int segX = NEAREST_INT((x + srsContext.cropOrigin[0]) / srsContext.scale);
		int segY = NEAREST_INT((y + srsContext.cropOrigin[1]) / srsContext.scale);
		CShape srsSegShape = srsContext.sourceSeg->smoothImg.Shape();
		SET_IF_LT(segX, srsSegShape.width - 1);
		SET_IF_LT(segY, srsSegShape.height - 1);
				
		int segID = srsContext.sourceSeg->labelImg.Pixel(segX, segY, 0);
		const Segment &currSeg = srsContext.sourceSeg->segments[segID];
		frontoPlaneIndex = currSeg.segData->avgProjFrontoPlaneIndex[planeIndex];
	}

	if((frontoPlaneIndex != SEGMENT_OccDepPlaneIndex) &&
	   (returnPlaneIndexInTargetSpace == true))
	{
		double srsDepth = srsContext.sourceCam.frontoPlanes[frontoPlaneIndex];
		CVector3 srsWorldXYZ(0.0, 0.0, srsDepth);
		CVector3 worldXYZ       = srsContext.sourceCam.cameraMats.invertCoordSys(srsWorldXYZ);
		CVector3 targetWorldXYZ = targetContext.sourceCam.cameraMats.transformCoordSys(worldXYZ);
		frontoPlaneIndex        = targetContext.sourceCam.GetBestFrontoPlaneIndex(targetWorldXYZ[2]);
	}

	return frontoPlaneIndex;
}

void StereoReconstruction::computeProjData(SRS_Context &srcSRS, SRS_Context &targetSRS,
										   CFloatImage &projImg, CFloatImage &projAnalogyImg, CByteImage &projMask)
{
	CFloatImage &srcImg        = srcSRS.sourceImg;
	CFloatImage &srcAnalogyImg = srcSRS.sourceAnalogyImg;
	CShape srcShape            = srcImg.Shape();
	CShortImage &srcPlaneMap   = srcSRS.sourcePlaneMap;	
	CByteImage &srcMask        = srcSRS.sourceMask;

	CShape targetShape = targetSRS.sourceImg.Shape();
	INSIST(targetShape.nBands == srcShape.nBands);
	CShape targetMaskShape(targetShape.width, targetShape.height, 1);

	bool isAnalogyImgDistinct = (srcImg.PixelAddress(0, 0, 0) != srcAnalogyImg.PixelAddress(0, 0, 0));

	projImg.ReAllocate(targetShape);
	projImg.ClearPixels();

	if(!isAnalogyImgDistinct)
		projAnalogyImg = projImg;
	else
	{
		projAnalogyImg.ReAllocate(targetShape);
		projAnalogyImg.ClearPixels();
	}

	projMask.ReAllocate(targetMaskShape);
	projMask.ClearPixels();
	INSIST(MASK_INVALID == 0);	

	CameraMats &srcCMs    = srcSRS.sourceCam.cameraMats;
	CameraMats &targetCMs = targetSRS.sourceCam.cameraMats;

	int validPixelCount = 0;
	uint maskAddr  = 0;
	uint pixelAddr = 0;
	for(int v = 0; v < targetShape.height; v++)
	for(int u = 0; u < targetShape.width;  u++, maskAddr++, pixelAddr += targetShape.nBands)
	{
		short targetPlaneIndex = targetSRS.sourcePlaneMap[maskAddr];
		CHECK_RANGE(targetPlaneIndex, 0, targetSRS.sourceCam.planeCount - 1);
		const Plane &targetPlane = targetSRS.sourceCam.planes[targetPlaneIndex];

		CVector2 targetUV((double) u, (double) v);
		CVector3 worldXYZ;
		targetCMs.projUVtoXYZ(targetUV, targetPlane, worldXYZ);
		CVector2 srsUV;
		srcCMs.projXYZtoUV(worldXYZ, srsUV);

		if(srcShape.InBounds(srsUV[0], srsUV[1]))
		{
			int srsU          = NEAREST_INT(srsUV[0]);
			int srsV          = NEAREST_INT(srsUV[1]);
			uint srsMaskAddr  = srcShape.NodeIndex(srsU, srsV);

			if(srcMask[srsMaskAddr] == MASK_INVALID) continue;

			int srsPlaneIndex = srcPlaneMap[srsMaskAddr];
			CHECK_RANGE(srsPlaneIndex, 0, srcSRS.sourceCam.planeCount - 1);

			bool useSRS = true;
			if((srsPlaneIndex != targetPlaneIndex) ||
			   (srsPlaneIndex >= this->scenePlaneCount))
			{
				int targetFrontoPlaneIndex = getFrontoPlaneIndex(u, v, targetSRS, targetPlaneIndex, false);
				int srsFrontoPlaneIndex    = getFrontoPlaneIndex(srsUV[0], srsUV[1], srcSRS, srsPlaneIndex, true, targetSRS);

				if((targetFrontoPlaneIndex == SEGMENT_OccDepPlaneIndex) ||
				   (srsFrontoPlaneIndex == SEGMENT_OccDepPlaneIndex) ||
				   (abs(srsFrontoPlaneIndex - targetFrontoPlaneIndex) > this->srParams.depthMatchThres))
				{
					useSRS = false;
				}
			}

			if(useSRS)
			{
				validPixelCount++;
				ImageProcessing::MitchellInterlopation(projImg, pixelAddr, srcImg, (float) srsUV[0], (float) srsUV[1]);
				if(isAnalogyImgDistinct)
					ImageProcessing::MitchellInterlopation(projAnalogyImg, pixelAddr, srcAnalogyImg, (float) srsUV[0], (float) srsUV[1]);

				projMask[maskAddr] = MASK_VALID;
			}
		}
	}

	if(validPixelCount > 0)
	{
		projImg = ImageProcessing::TransferColor(projImg, targetSRS.sourceImg, projMask, true, 
												 targetSRS.sourceMask, true);
	}
}



void StereoReconstruction::pcMaskPruner(RegisteredPiece &regPiece)
{
	CShape maskShape = regPiece.mask.Shape();

	CShortImage labelsImg(maskShape);
	int nodeCount = maskShape.width * maskShape.height;
	for(int iNode = 0; iNode < nodeCount; iNode++)
	{
		if(regPiece.mask[iNode] == MASK_VALID)
			labelsImg[iNode] = 0;
		else
			labelsImg[iNode] = -1;
	}
	RegisteredPieces regPieces1;
	regPieces1.push_back(regPiece);
	CCLabeler ccLabeler1(labelsImg, regPieces1);
	int prunedPixelCount = ccLabeler1.PruneComponents(CCLabeler::PRUNE_ALL_BUT_LARGEST);
	printf("%i pixels pruned from the foreground mask.\n", prunedPixelCount);

	CByteImage regMaskInv(maskShape);
	for(int iNode = 0; iNode < nodeCount; iNode++)
	{
		if(regPiece.mask[iNode] == MASK_INVALID)
		{
			regMaskInv[iNode] = MASK_VALID;
			labelsImg[iNode]  = 0;
		}
		else
		{
			regMaskInv[iNode] = MASK_INVALID;
			labelsImg[iNode] = -1;
		}
	}

	RegisteredPiece regPieceInv;
	regPieceInv.mask = regMaskInv;

	//bug
	if((this->targetContextSRS.iSourceCam < 71) ||
		(this->targetContextSRS.iSourceCam > 94))
	{
		RegisteredPieces regPieces2;
		regPieces2.push_back(regPieceInv);
		CCLabeler ccLabeler2(labelsImg, regPieces2);
		prunedPixelCount = ccLabeler2.PruneComponents(CCLabeler::PRUNE_ALL_BUT_LARGEST);
		printf("%i pixels pruned from the background mask.\n", prunedPixelCount);

		for(int iNode = 0; iNode < nodeCount; iNode++)
		{
			if(regMaskInv[iNode] == MASK_INVALID)
			{
				regPiece.mask[iNode] = MASK_VALID;
			}
		}	
	}


	//regPiece.mask = ImageProcessing::ErodeImage(regPiece.mask, 5, MASK_VALID);
}


RegisteredPiece StereoReconstruction::createRegisteredPiece(SRS_Context &srcSRS, SRS_Context &targetSRS, bool useForwardProjection)
{
	RegisteredPiece regPiece;
	regPiece.id = srcSRS.iSourceCam;

	if(useForwardProjection)
	{
		computeProjData(srcSRS, targetSRS, 
						regPiece.filteredImage, regPiece.image, regPiece.mask);
	}
	else
	{
		CShortImage srsProjPlaneMap;		
		createViewProjection(srcSRS, targetSRS, true,
							 regPiece.filteredImage, regPiece.image, regPiece.mask, srsProjPlaneMap);
	}

	regPiece.ComputeEdgeImages();

	CShape regPieceShape = regPiece.image.Shape();
	regPiece.colDistSqrImg.ReAllocate(regPiece.mask.Shape());
	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < regPieceShape.height; y++)
	for(int x = 0; x < regPieceShape.width;  x++, nodeAddr++, pixelAddr += regPieceShape.nBands)
	{		
		if(regPiece.mask[nodeAddr] == MASK_INVALID) 
		{
			regPiece.colDistSqrImg[nodeAddr] = FLT_MAX;
			continue;
		}

		regPiece.colDistSqrImg[nodeAddr] = 0.0f;

		float targetsWeightSum = 0.0f;
		for(uint iTargetImg = 0; iTargetImg < this->neighProjImgs.size() + 1; iTargetImg++)
		{
			CFloatImage targetImg;
			CByteImage targetMask;
			float targetWeight;

			if(iTargetImg < this->neighProjImgs.size())
			{
				targetImg    = this->neighProjImgs[iTargetImg];
				targetMask   = this->neighProjMasks[iTargetImg];
				targetWeight = 1.0;
			}
			else
			{
				targetImg = targetSRS.sourceImg;
				targetMask = targetSRS.sourceMask;
				targetWeight = 1.0;
			}

		    if(targetMask[nodeAddr] == MASK_VALID)
			{
				targetsWeightSum += targetWeight;
				for(int iChannel = 0; iChannel < regPieceShape.nBands; iChannel++)
				{
					float channelDiff = targetImg[pixelAddr + iChannel] - regPiece.filteredImage[pixelAddr + iChannel];
					regPiece.colDistSqrImg[nodeAddr] += targetWeight * (channelDiff * channelDiff);
				}
			}
		}
		ENSURE(targetsWeightSum > 0.0f);
		regPiece.colDistSqrImg[nodeAddr] /= targetsWeightSum;
	}

	//CFloatImage camDistImgSqr(depthDistSqrImg.Shape());
	//ImageProcessing::Operate(camDistImgSqr, camDistImgSqr, srcSRS.weight * srcSRS.weight, ImageProcessing::OP_SET);
	//ImageProcessing::Operate(camDistImgSqr, camDistImgSqr, 1.0f, ImageProcessing::OP_MUL);
	//ImageProcessing::Operate(regPiece.colDistSqrImg, regPiece.colDistSqrImg, camDistImgSqr, ImageProcessing::OP_ADD);
	
	//ImageProcessing::Operate(depthDistSqrImg, depthDistSqrImg, 100.0f, ImageProcessing::OP_MUL);
	//ImageProcessing::Operate(regPiece.colDistSqrImg, regPiece.colDistSqrImg, depthDistSqrImg, ImageProcessing::OP_ADD);

	return regPiece;
}


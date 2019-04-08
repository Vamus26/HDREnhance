#include "StereoReconstruction.h"
#include "Timer.h"
#include "Disk.h"
#include "ImageIO.h"

void StereoReconstruction::loadNeighContext()
{

	ENSURE(this->srParams.neighCamIDs.size() == this->srParams.neighCamCropExts.size());
	ENSURE(this->srParams.neighCamIDs.size() == this->srParams.neighCamCropOrigins.size());
	ENSURE(this->srParams.neighCamIDs.size() == this->srParams.neighCamScales.size());

	for(uint iNeigh = 0; iNeigh < this->srParams.neighCamIDs.size(); iNeigh++)
	{
		SRS_Context neighContext = loadSRS_Context(this->srParams.neighCamIDs[iNeigh], true,
												   true, false,
												   this->refCamMats, true, 
			                                       this->srParams.neighCamScales[iNeigh],
												   this->srParams.neighCamCropOrigins[iNeigh], 
												   this->srParams.neighCamCropExts[iNeigh]);

		CFloatImage projImg;
		CFloatImage projAnalogyImg;
		CByteImage  projMask;

		computeProjData(neighContext, this->targetContextSRS, 
						projImg, projAnalogyImg, projMask);

		this->neighProjImgs.push_back(projImg);
		this->neighProjMasks.push_back(projMask);

		unloadSRS_Context(neighContext);
	}
}

void StereoReconstruction::loadContext(bool loadSegData)
{
	printf("Loading SR context for - %s\n", this->srParams.targetCamID.c_str()); 

	loadFrameSegmentations(this->srParams.iFrame);

	bool loadAnalogyImg = false;
	if(this->rig.IsVideoFrame(this->srParams.targetCamID) == false)
	{
		loadAnalogyImg = this->srParams.reconstructUsingAnalogy;
	}

	this->targetContextSRS = loadSRS_Context(this->srParams.targetCamID, loadSegData,
										     false, loadAnalogyImg,
										     CameraMats(), false, 
										     this->srParams.targetScale, this->srParams.targetCropOrigin, this->srParams.targetCropExt);

	this->refCamMats = this->targetContextSRS.sourceCam.cameraMats;

	this->targetContextSRS.sourceCam.Recenter(this->refCamMats);

	loadNeighContext();
}


void StereoReconstruction::loadAnalogyMask(SRS_Context &srsContext)
{
	string maskFN = srsContext.sourceCam.GetFrameAnalogyMaskName(this->srParams.iFrame);

	CFloatImage analogyMaskImg;
	ImageIO::ReadFile(analogyMaskImg, maskFN);	
	if(analogyMaskImg.Shape().nBands == 3)
		analogyMaskImg = ColorHelper::RGBtoLuminance(analogyMaskImg);

	CShape maskShape = analogyMaskImg.Shape();
	ENSURE(maskShape == srsContext.sourceMask.Shape());
	int nodeCount = maskShape.width * maskShape.height;

	for(int iNode = 0; iNode < nodeCount; iNode++)
	{
		if(analogyMaskImg[iNode] == 0.0)
		{
			srsContext.sourceMask[iNode] = MASK_INVALID;
		}
	}
}

SRS_Context StereoReconstruction::loadSRS_Context(string camID, bool loadSegData,
												  bool computeDisConMask, bool loadAnalogyImg,
												  const CameraMats &refCameraMats, bool recenterSRS_Cam,
												  float scale, CVector2 cropOrigin, CVector2 cropExt)
{
	SRS_Context srsContext;
	srsContext.scale = scale;
	srsContext.cropOrigin = cropOrigin; srsContext.cropExt = cropExt;

	srsContext.sourceCam = *(this->rig.GetCamera(camID, srsContext.iSourceCam));
	CHECK_RANGE(srsContext.iSourceCam, 0, (int) this->rig.cameras.size() - 1);

	
	//string sourceDepthsFN = srsContext.sourceCam.GetFrameDepsNameAlt(this->srParams.iFrame);	

	if(loadSegData)
	{
		string sourceDepthsFN = srsContext.sourceCam.GetFrameDepsName(this->srParams.iFrame);	
		ImageIO::ReadRaw(srsContext.sourcePlaneMap, sourceDepthsFN);
	}
	else
	{
		CShape maskShape = srsContext.sourceCam.GetFrameShape(0);
		maskShape.nBands = 1;
		srsContext.sourcePlaneMap.ReAllocate(maskShape);
		ImageProcessing::Operate(srsContext.sourcePlaneMap, srsContext.sourcePlaneMap, 
								 (short) SEGMENT_OccDepPlaneIndex, ImageProcessing::OP_SET);
	}

	if(srsContext.scale != 1.0f)
	{
		INSIST(srsContext.scale > 1.0f);

		srsContext.sourceCam.cameraMats.ScaleMats(srsContext.scale);

		string sourceSmallImgFN = srsContext.sourceCam.GetFrameName(this->srParams.iFrame);
		CShape smallImgShape = ImageIO::GetImageShape(sourceSmallImgFN);

		string sourceLargeImgFN = srsContext.sourceCam.GetFrameNameLarge(this->srParams.iFrame);
		CShape largeImgShape = ImageIO::GetImageShape(sourceLargeImgFN);
		
		float largeImgScale = largeImgShape.width / (float) smallImgShape.width;
		INSIST(largeImgScale >= 1.0f);
		INSIST(fabs(largeImgScale - (largeImgShape.height / (float) smallImgShape.height)) <= 0.01f);		
		
		float largeImgToSrsScale = srsContext.scale / largeImgScale;
		CFloatImage sourceImgTemp;
		ImageIO::ReadFile(sourceImgTemp, sourceLargeImgFN);

		CFloatImage sourceAnalogyImgTemp;
		if(!loadAnalogyImg)
			sourceAnalogyImgTemp = sourceImgTemp;
		else
		{
			string sourceLargeAnalogyImgFN = srsContext.sourceCam.GetFrameAnalogyNameLarge(this->srParams.iFrame);
			ImageIO::ReadFile(sourceAnalogyImgTemp, sourceLargeAnalogyImgFN);
		}
		INSIST(sourceAnalogyImgTemp.Shape() == sourceImgTemp.Shape());

		//bug
		if(this->rig.IsVideoFrame(srsContext.iSourceCam) == false)
		{
			for(int y = 0; y < largeImgShape.height - 1; y++)
			for(int x = 0; x < largeImgShape.width - 1;  x++)
			for(int channel = 0; channel < largeImgShape.nBands; channel++)
			{
				sourceImgTemp.Pixel(x, y, channel) = sourceImgTemp.Pixel(x + 1, y + 1, channel);
				if(loadAnalogyImg)
				{
					sourceAnalogyImgTemp.Pixel(x, y, channel) = sourceAnalogyImgTemp.Pixel(x + 1, y + 1, channel);
				}
			}			
		}
		ImageProcessing::ScaleImage(srsContext.sourceImg, sourceImgTemp, largeImgToSrsScale, false);

		if(loadAnalogyImg)
			ImageProcessing::ScaleImage(srsContext.sourceAnalogyImg, sourceAnalogyImgTemp, largeImgToSrsScale, false);
		else
			srsContext.sourceAnalogyImg = srsContext.sourceImg;

		srsContext.sourcePlaneMap = ImageProcessing::ScaleImageWithoutSampling(srsContext.sourcePlaneMap, srsContext.scale);		
	}
	else
	{
		string sourceImgFN = srsContext.sourceCam.GetFrameName(this->srParams.iFrame);
		//string sourceImgFN = GetTargetContextNovelViewFN(&srsContext.sourceCam, 0, this->rig.params.outDir);
		ImageIO::ReadFile(srsContext.sourceImg, sourceImgFN);	

		if(!loadAnalogyImg)
			srsContext.sourceAnalogyImg = srsContext.sourceImg;
		else
		{
			string sourceAnalogyImgFN = srsContext.sourceCam.GetFrameAnalogyName(this->srParams.iFrame);
			ImageIO::ReadFile(srsContext.sourceAnalogyImg, sourceAnalogyImgFN);
		}			
	}

	CShape planeMapShape = srsContext.sourcePlaneMap.Shape();
	CShape imgShape = srsContext.sourceImg.Shape();
	INSIST(planeMapShape.nBands == 1);
	if(planeMapShape.SameIgnoringNBands(imgShape) == false)
	{
		INSIST(abs(imgShape.width - planeMapShape.width) <= 1);
		INSIST(abs(imgShape.height - planeMapShape.height) <= 1);

		int newW = min(imgShape.width,  planeMapShape.width);
		int newH = min(imgShape.height, planeMapShape.height);

		srsContext.sourceImg        = ImageProcessing::CropImage(srsContext.sourceImg,        0, 0, newW, newH);
		srsContext.sourceAnalogyImg = ImageProcessing::CropImage(srsContext.sourceAnalogyImg, 0, 0, newW, newH);
		srsContext.sourcePlaneMap   = ImageProcessing::CropImage(srsContext.sourcePlaneMap,   0, 0, newW, newH);	
	}

	if((srsContext.cropExt != CVector2::Null()) &&
	   ((srsContext.cropExt[0] != srsContext.sourceImg.Shape().width) ||
	    (srsContext.cropExt[1] != srsContext.sourceImg.Shape().height)))
	{
		srsContext.sourceCam.cameraMats.CropMats(srsContext.cropOrigin[0], srsContext.cropOrigin[1],
												 srsContext.cropExt[0],    srsContext.cropExt[1]);

		srsContext.sourceImg = ImageProcessing::CropImage(srsContext.sourceImg, 
														   (int) srsContext.cropOrigin[0], (int) srsContext.cropOrigin[1],
														   (int) srsContext.cropExt[0], (int) srsContext.cropExt[1]);

		srsContext.sourceAnalogyImg = ImageProcessing::CropImage(srsContext.sourceAnalogyImg, 
															     (int) srsContext.cropOrigin[0], (int) srsContext.cropOrigin[1],
														         (int) srsContext.cropExt[0], (int) srsContext.cropExt[1]);

		srsContext.sourcePlaneMap = ImageProcessing::CropImage(srsContext.sourcePlaneMap,
															   (int) srsContext.cropOrigin[0], (int) srsContext.cropOrigin[1],
															   (int) srsContext.cropExt[0], (int) srsContext.cropExt[1]);
	}


	srsContext.sourceMask.ReAllocate(srsContext.sourcePlaneMap.Shape());
	ImageProcessing::Operate(srsContext.sourceMask, srsContext.sourceMask, MASK_VALID, ImageProcessing::OP_SET);	

	if(loadSegData)
	{
		srsContext.sourceSeg = loadSegmentation(srsContext.iSourceCam, this->srParams.iFrame, true, false);
		INSIST(srsContext.sourceSeg != NULL);
		INSIST(srsContext.sourceSeg->fullyComputed == true);
		INSIST(srsContext.sourceSeg->segments.size() > 0);
	}
	else
	{
		srsContext.sourceSeg = NULL;
	}

	INSIST(srsContext.sourceAnalogyImg.Shape() == srsContext.sourceImg.Shape());

	if(computeDisConMask)
		computeSRS_Mask(srsContext);

	if((loadAnalogyImg) && (this->srParams.useAnalogyMask))
	{
		loadAnalogyMask(srsContext);
	}	

	if(recenterSRS_Cam)
	{
		srsContext.sourceCam.Recenter(refCameraMats);
	}

	return srsContext;
}

void StereoReconstruction::unloadContext()
{
	unloadSRS_Context(this->targetContextSRS);

	unloadFrameSegmentations(this->srParams.iFrame);
}

void StereoReconstruction::unloadSRS_Context(SRS_Context &srsContext)
{
	unloadSegmentation(srsContext.iSourceCam, this->srParams.iFrame, false);
	srsContext.sourceSeg = NULL;
}

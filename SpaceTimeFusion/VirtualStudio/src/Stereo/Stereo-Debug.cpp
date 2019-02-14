#include "Stereo.h"
#include "ImageIO.h"
#include "Draw.h"
#include "FeatureRenderer.h"

void Stereo::dumpScenePts(CFloatImage &image, int camID, const vector<CVector3> &scenePts)
{
	CameraMats camMats = this->rig.cameras[camID].cameraMats;
	CShape imgShape = image.Shape();

	CVector3 col(1.0, 0.0, 0.0);
	for(uint iPoint = 0; iPoint < scenePts.size(); iPoint++)
	{
		CVector2 currUV;
		camMats.projXYZtoUV(scenePts[iPoint], currUV);
		//ENSURE(imgShape.InBounds(currUV[0], currUV[1]));
		Draw::DrawPoint(image, 
			            NEAREST_INT(currUV[0]), NEAREST_INT(currUV[1]),
						2, col);
	}
}


void Stereo::dumpScenePts(CFloatImage &image, int camID, const PointCloud &scenePts)
{

	CameraMats camMats = this->rig.cameras[camID].cameraMats;
	CShape imgShape = image.Shape();

	CVector3 col(1.0, 0.0, 0.0);
	for(uint iPoint = 0; iPoint < scenePts.size(); iPoint++)
	{
		CVector2 currUV;
		camMats.projXYZtoUV(scenePts[iPoint].pos, currUV);
		//ENSURE(imgShape.InBounds(currUV[0], currUV[1]));
		Draw::DrawPoint(image, 
			            NEAREST_INT(currUV[0]), NEAREST_INT(currUV[1]),
						2, col);
	}
}


void Stereo::dumpEpipolarLineVis(CFloatImage &refCamImg, CFloatImage &neighCamImg,
								 int refCamID, int neighCamID, 
								 int x, int y, bool flipY)
{
	const Camera &refCam   = this->rig.cameras[refCamID];
	const Camera &neighCam = this->rig.cameras[neighCamID];

	CameraMats refCamMats   = refCam.cameraMats;
	CameraMats neighCamMats = neighCam.cameraMats;
	refCamMats.Recenter(refCam.cameraMats);
	neighCamMats.Recenter(refCam.cameraMats);

	CShape refImgShape = refCamImg.Shape();
	CHECK_RANGE(x, 0, refImgShape.width  - 1);
	CHECK_RANGE(y, 0, refImgShape.height - 1);
	if(flipY)
		y = refImgShape.height - y - 1;

	CVector3 col(1.0, 0.0, 0.0);
	Draw::DrawPoint(refCamImg, x, y, 2, col);

	CVector3 refViewUVZ((double) x, (double) y, 0.0);
	for(int iFrontoPlane = 0; iFrontoPlane < refCam.frontoPlaneCount; iFrontoPlane++)
	{
		refViewUVZ[2] = refCam.frontoPlanes[iFrontoPlane];
		
		CVector3 worldXYZ;
		refCamMats.projUVZtoXYZ(refViewUVZ, worldXYZ);

		CVector2 neighViewUV;
		neighCamMats.projXYZtoUV(worldXYZ, neighViewUV);

		Draw::DrawPoint(neighCamImg, 
					    NEAREST_INT((float) neighViewUV[0]), 
					    NEAREST_INT((float) neighViewUV[1]), 
					    1, col);
	}
}


void Stereo::dumpEpipolarLineVis(int refCamID, int neighCamID, 
				                 int x, int y, bool flipY)
{
	CFloatImage refCamImg   = this->camSegmentations[refCamID]->smoothImg.Clone();
	CFloatImage neighCamImg = this->camSegmentations[neighCamID]->smoothImg.Clone();

	dumpEpipolarLineVis(refCamImg, neighCamImg,
						refCamID, neighCamID, 
						x, y, flipY);

	ImageIO::WriteFile(refCamImg,   "refCam.tga");
	ImageIO::WriteFile(neighCamImg, "neighCamLine.tga");	
}
	

CByteImage Stereo::getPlanesOutOfBoundsMask(int iScenePlane)
{
	ENSURE(this->currView != NULL);
	ENSURE(iScenePlane < this->scenePlaneCount);

	const vector<Segment> &currViewSegs = this->currView->seg->segments;
	const CIntImage &currViewLabels = this->currView->seg->labelImg;
	CShape maskShape = currViewLabels.Shape();

	CByteImage mask(maskShape);
	mask.ClearPixels();

	uint nodeAddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	for(int x = 0; x < maskShape.width;  x++, nodeAddr++)
	{
		int segLabel = currViewLabels[nodeAddr];
		const Segment &currSeg = currViewSegs[segLabel];

		bool segValid = true;
		int iScenePlaneStart = iScenePlane;
		int iScenePlaneEnd   = iScenePlane;
		if(iScenePlane == -1) 
		{
			iScenePlaneStart = 0; 
			iScenePlaneEnd = this->scenePlaneCount - 1;
		}

		for(int iScenePlane = iScenePlaneStart; iScenePlane <= iScenePlaneEnd; iScenePlane++)
		{
			if(currSeg.segData->avgProjFrontoPlaneIndex[iScenePlane] == SEGMENT_OccDepPlaneIndex)
			{
				segValid = false;
				break;
			}
		}

		if(segValid)
		{
			mask[nodeAddr] = MASK_VALID;
		}
	}

	return mask;
}

CByteImage Stereo::getSegsNotVisMask(int iView)
{
	ENSURE(this->currView != NULL);

	const vector<Segment> &currViewSegs = this->currView->seg->segments;
	const CIntImage &currViewLabels = this->currView->seg->labelImg;
	CShape maskShape = currViewLabels.Shape();

	CByteImage mask(maskShape);
	mask.ClearPixels();

	uint nodeAddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	for(int x = 0; x < maskShape.width;  x++, nodeAddr++)
	{
		int segLabel = currViewLabels[nodeAddr];
		const Segment &currSeg = currViewSegs[segLabel];
		if(currSeg.segData->alwaysInViewBounds[iView])
			mask[nodeAddr] = MASK_VALID;
	}

	return mask;
}

void Stereo::visScenePlaneProjections(int iView)
{
	ENSURE(this->scenePlaneCount > 0);

	CShape imgShape = this->currView->smoothImg.Shape();
	CFloatImage neighProjImg(imgShape);
	CByteImage  neighProjMask(imgShape.width, imgShape.height, 1);
	CIntImage   neighProjLabels(imgShape.width, imgShape.height, 1);

	for(int iScenePlane = 0; iScenePlane < this->scenePlaneCount; iScenePlane++)
	{
		computeAvgProjDepths(iScenePlane);
	}

	for(int iScenePlane = 0; iScenePlane < this->scenePlaneCount; iScenePlane++)
	//for(int iScenePlane = this->scenePlaneCount; iScenePlane < this->currView->cam.planeCount; iScenePlane++)
	{
		createNeighProjection(iView, iScenePlane,
							  neighProjImg, neighProjLabels, neighProjMask);
		char fileName[1024];
		sprintf(fileName, "view-%03i-plane-%03i.tga", iView, iScenePlane);
		ImageIO::WriteFile(neighProjImg, fileName);
	}

	for(int iScenePlane = 0; iScenePlane < this->scenePlaneCount; iScenePlane++)
	{	
		CByteImage planeOutOfBoundMask = getPlanesOutOfBoundsMask(iScenePlane);

		char fileName[1024];
		sprintf(fileName, "plane_mask-%03i.tga", iScenePlane);
		ImageIO::WriteFile(planeOutOfBoundMask, fileName);		
	}

	exit(0);
}


void Stereo::visSFM_Geo()
{
	//this->depthPlanes.clear();
	//for(float i = 0.001f; i < 0.1f; i += 0.001f)
	//	this->depthPlanes.push_back(i);

	int camID1 = 0;
	int camID2 = 30;

	loadSegmentation(camID1, 0, true, true);
	loadSegmentation(camID2, 0, true, true);

	CFloatImage refCamImg   = this->camSegmentations[camID1]->smoothImg.Clone();
	CFloatImage neighCamImg = this->camSegmentations[camID2]->smoothImg.Clone();

	dumpEpipolarLineVis(refCamImg, neighCamImg, camID1, camID2, 451, 72);
	dumpEpipolarLineVis(refCamImg, neighCamImg, camID1, camID2, 338, 14);
	dumpEpipolarLineVis(refCamImg, neighCamImg, camID1, camID2, 484, 199);
	dumpEpipolarLineVis(refCamImg, neighCamImg, camID1, camID2, 364, 310);
	dumpEpipolarLineVis(refCamImg, neighCamImg, camID1, camID2, 466, 340);

	//dumpEpipolarLineVis(refCamImg, neighCamImg, camID1, camID2, 364, 50);
	//dumpEpipolarLineVis(refCamImg, neighCamImg, camID1, camID2, 354, 201);
	//dumpEpipolarLineVis(refCamImg, neighCamImg, camID1, camID2, 360, 391);
	//dumpEpipolarLineVis(refCamImg, neighCamImg, camID1, camID2, 362, 28);
	//dumpEpipolarLineVis(refCamImg, neighCamImg, camID1, camID2, 367, 309);



	this->rig.cameras[camID1].ReadViewPts();

	//dumpScenePts(refCamImg,   camID1, this->rig.cameras[camID1].viewPoints);
	//dumpScenePts(neighCamImg, camID2, this->rig.cameras[camID1].viewPoints);

	ImageIO::WriteFile(refCamImg, "refCam.tga");
	ImageIO::WriteFile(neighCamImg, "neighCamLine.tga");

	//Segmentation *cam1Seg = this->camSegmentations[camID1];
	//CShape maskShape = cam1Seg->labelImg.Shape();
	//CByteImage mask(maskShape);
	//uint nodeAddr = 0;
	//for(int y = 0; y < maskShape.height; y++)
	//for(int x = 0; x < maskShape.width; x++, nodeAddr++)
	//{
	//	int segID = cam1Seg->labelImg[nodeAddr];
	//	const Segment &currSeg = cam1Seg->segments[segID];
	//	if(currSeg.avgCol.Length() < 0.2)
	//		mask[nodeAddr] = MASK_INVALID;
	//	else
	//		mask[nodeAddr] = MASK_VALID;
	//}
	//ImageIO::WriteFile(mask, "mask.tga");

	exit(0);
}

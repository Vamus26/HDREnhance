#include "Stereo.h"
#include "Timer.h"
#include "ImageProcessing.h"
#include "ImageIO.h"

const double STEREO_MinScenePlaneProjPlaneAngCos = 0.1;

void Stereo::computeCurrViewStaticData()
{
	this->currView->seg->ComputeSpatialNeighs();

	int iFrame = this->currView->iFrame;
	int iCam   = this->currView->iCam;

	printf("Computing static data for frame-%05i, cam-%s\n", iFrame, this->currView->cam.id.c_str());

	for(int iScenePlane = 0; iScenePlane < this->currView->cam.scenePlaneCount; iScenePlane++)
	{
		computeAvgProjDepths(iScenePlane);
	}

	Timer timer(true);
	for(int iView = 0; iView < (int) this->neighViews.size(); iView++)
	{		
		for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
		{
			computeStaticData(iView, iPlane);
			printf(".");
			fflush(stdout);
		}
		printf("\n");
		timer.DumpSeconds("Neigh done.");
	}			
	normalizeDatacosts();

	this->currView->seg->fullyComputed = true;
	this->currView->seg->SaveSegmentsStaticData(this->currView->cam, iFrame);
}

void Stereo::computeStaticData(int iView, int iPlane)
{
	CHECK_RANGE(iPlane, 0, this->currView->cam.planeCount - 1);
	CHECK_RANGE(iView,  0, (int) this->neighViews.size() - 1);
	ENSURE(this->currView->seg->fullyComputed == false);

	CShape imgShape = this->currView->smoothImg.Shape();
	CShape maskShape(imgShape.width, imgShape.height, 1);

	CFloatImage neighProjImg(imgShape);
	CByteImage  neighProjMask(maskShape);
	CIntImage   neighProjLabels(maskShape);

	createNeighProjection(iView, iPlane,
						  neighProjImg, neighProjLabels, neighProjMask);

	computeNeighOverlapStats(iView, iPlane,
							 neighProjImg, neighProjLabels, neighProjMask);
}


void Stereo::computeNeighOverlapStats(int iView, int iPlane,
							 		  const CFloatImage &neighProjImg,
									  const CIntImage &neighProjLabels, 
									  const CByteImage &neighProjMask)
{
	computeDatacosts(iView, iPlane,
					 neighProjImg, neighProjMask);

	vector<Segment> &currViewSegs  = this->currView->seg->segments;
	CIntImage &currViewLabels      = this->currView->seg->labelImg;
	CShape currViewMaskShape       = currViewLabels.Shape();
	ENSURE(neighProjLabels.Shape() == currViewMaskShape);

	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		currSeg->viewNeighs->StartViewPlaneBooking();
	}

	uint maskAddr = 0;
	for(int y = 0; y < currViewMaskShape.height; y++)
	for(int x = 0; x < currViewMaskShape.width;  x++, maskAddr++)
	{
		if(neighProjMask[maskAddr] == MASK_VALID) 
		{
			int segLabel = currViewLabels[maskAddr];
			Segment &currSeg = currViewSegs[segLabel];

			//if not projected out of bounds of the neighbouring view
			if(currSeg.segData->dataCost[iView][iPlane] > 0.0f) 
			{
				int overlapSegLabel = neighProjLabels[maskAddr];
				currSeg.viewNeighs->FoundOverlapPixel(overlapSegLabel);
			}
		}
	}

	int iSeg = 0;
	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++, iSeg++)
	{
		currSeg->viewNeighs->DoneViewPlaneBooking(iView, iPlane);	
	}
}


void Stereo::computeDatacosts(int iView, int iPlane,
							  const CFloatImage &neighProjImg,
							  const CByteImage  &neighProjMask)
{

	vector<Segment> &currViewSegs = this->currView->seg->segments;

	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		currSeg->segData->dataCost[iView][iPlane] = 0.0f;
		ENSURE(currSeg->auxInt == 0);
	}
	
	sumPixelDatacosts(iView, iPlane,
					  neighProjImg, neighProjMask);

	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		if(currSeg->auxInt != currSeg->size)
		{
			currSeg->segData->dataCost[iView][iPlane] = 0.0f;
			if((iPlane >= this->currView->cam.scenePlaneCount) ||
			   (currSeg->segData->avgProjFrontoPlaneIndex[iPlane] != SEGMENT_OccDepPlaneIndex))
			{
				currSeg->segData->alwaysInViewBounds[iView] = false;
			}
		}
		else
		{
			currSeg->segData->dataCost[iView][iPlane] /= currSeg->size;
			currSeg->segData->dataCost[iView][iPlane] += FLT_EPSILON;
		}

		if((this->stereoParams.onlyUseScenePlanes) &&
		   (iPlane >= this->scenePlaneCount))
		{
			currSeg->segData->dataCost[iView][iPlane] = 0.0f;			
		}

		currSeg->auxInt = 0;
	}
}

void Stereo::sumPixelDatacosts(int iView, int iPlane,
							   const CFloatImage &neighProjImg,
					           const CByteImage  &neighProjMask)
{
	CIntImage &currViewLabels     = this->currView->seg->labelImg;
	CFloatImage &currViewImg      = this->currView->smoothImg;	
	vector<Segment> &currViewSegs = this->currView->seg->segments;

	CShape currViewShape = currViewImg.Shape();
	ENSURE(neighProjImg.Shape() == currViewShape);

	float noiseVarDenom = -2.0f * this->stereoParams.noiseVar * currViewShape.nBands;

	#if defined(__GROTTO__) || defined(__DT__)
		if(this->currView->cam.id.substr(0, 3).compare(
		   this->neighViews[iView].cam.id.substr(0, 3)) != 0)
		{
			noiseVarDenom *= 4;
		}
	#endif

	uint nodeAddr = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < currViewShape.height; y++)
	for(int x = 0; x < currViewShape.width;  x++, nodeAddr++)
	{
		if(neighProjMask[nodeAddr] == MASK_INVALID) 
		{
			pixelAddr += currViewShape.nBands;
			continue;
		}

		int nodeLabel    = currViewLabels[nodeAddr];
		Segment &nodeSeg = currViewSegs[nodeLabel];
		nodeSeg.auxInt++;

		//check - weigh each color channel differently
		float dist = 0.0f;
		for(int iBand = 0; iBand < currViewShape.nBands; iBand++, pixelAddr++)
		{
			float diff = currViewImg[pixelAddr] - neighProjImg[pixelAddr];
			//check - try exp of abs diff
			dist += diff * diff;
		}

		float matchWeight = exp(dist / noiseVarDenom);
		nodeSeg.segData->dataCost[iView][iPlane] += matchWeight;
	}
}

void Stereo::normalizeDatacosts()
{	
	float datacostExp = this->stereoParams.dataCostExp;
	ENSURE(datacostExp > 0.0f);

	float defDataCostFrac    = 0.99f;
	float defDataCostFracInv = 1.0f - defDataCostFrac;
	float defaultDataCost    = (1.0f / this->currView->cam.planeCount) * defDataCostFrac;

	vector<Segment> &currViewSegs = this->currView->seg->segments;
	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)	
	{
		for(uint iView = 0; iView < this->neighViews.size(); iView++)
		{
			int viewBestPlaneID    = SEGMENT_OccDepPlaneIndex;
			float viewBestDatacost = currSeg->GetBestDatacost(iView,  viewBestPlaneID);	
			
			if(currSeg->segData->alwaysInViewBounds[iView] == false) //some plane projection is out of view bounds
			{				
				for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
				{
					float oldcost = currSeg->segData->dataCost[iView][iPlane];
					float newCost;

					if((iPlane < this->currView->cam.scenePlaneCount) &&
					   (currSeg->segData->avgProjFrontoPlaneIndex[iPlane] == SEGMENT_OccDepPlaneIndex))
					{
					   newCost = 0.0f;
					}
					else
					{
						if(oldcost == 0.0f) //current depth projection is out of bound
							newCost = defaultDataCost + (defDataCostFracInv * viewBestDatacost * genProb());
						else
							newCost = defaultDataCost + (defDataCostFracInv * oldcost);
					}

					currSeg->segData->dataCost[iView][iPlane] = newCost;
				}
			}
			else
			{
				ENSURE(viewBestDatacost >= FLT_EPSILON);
				for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
				{
					float oldcost = currSeg->segData->dataCost[iView][iPlane];
					float newcost = pow((oldcost / viewBestDatacost), datacostExp);
					currSeg->segData->dataCost[iView][iPlane] = newcost;
				}
			}
		}
	}
}


void Stereo::createNeighProjection(int iView, int iPlane, 
								   CFloatImage &neighProjImg,
								   CIntImage   &neighProjLabels,
								   CByteImage  &neighProjMask)
{
	const Plane &projPlane = this->currView->cam.planes[iPlane];

	const View &neighView = this->neighViews[iView];
	const CIntImage &neighLabelImg = neighView.seg->labelImg;

	const CIntImage &currLabelImg       = currView->seg->labelImg;
	const vector<Segment> &currSegments = currView->seg->segments;

	CShape currViewShape = this->currView->seg->smoothImg.Shape();
	ENSURE(currViewShape == neighProjImg.Shape());
	CShape neighViewShape = neighView.smoothImg.Shape();
	ENSURE((neighViewShape.width > 0) && (neighViewShape.height > 0));

	const CameraMats &currViewCMs  = this->currView->cam.cameraMats;
	const CameraMats &neighViewCMs = neighView.cam.cameraMats;	

	neighProjImg.ClearPixels();
	ENSURE(MASK_INVALID == 0);
	neighProjMask.ClearPixels();
	ImageProcessing::Operate(neighProjLabels, neighProjLabels, SEGMENT_OccDepPlaneIndex, ImageProcessing::OP_SET);

	uint maskAddr = 0;
	uint pixelAddr = 0;
	for(int v = 0; v < currViewShape.height; v++)
	for(int u = 0; u < currViewShape.width;  u++, maskAddr++, pixelAddr += currViewShape.nBands)
	{
		if(iPlane < this->scenePlaneCount)
		{
			int segID = currLabelImg[maskAddr];
			const Segment &currSeg = currSegments[segID];
			if(currSeg.segData->avgProjFrontoPlaneIndex[iPlane] == SEGMENT_OccDepPlaneIndex)
			{
				continue;
			}
		}

		CVector2 imgUV((double) u, (double) v);
		CVector3 worldXYZ;
		currViewCMs.projUVtoXYZ(imgUV, projPlane, worldXYZ);
		if(InRange(worldXYZ[2], this->currView->cam.minDepth, this->currView->cam.maxDepth))
		{
			CVector2 neighViewUV;
			neighViewCMs.projXYZtoUV(worldXYZ, neighViewUV);			
			if(neighViewShape.InBounds(neighViewUV[0], neighViewUV[1]))
			{
				int neighViewU = NEAREST_INT(neighViewUV[0]);
				int neighViewV = NEAREST_INT(neighViewUV[1]);
				uint sourceMaskAddr  = neighViewShape.NodeIndex(neighViewU, neighViewV);

				//uint sourcePixelAddr = sourceMaskAddr * neighViewShape.nBands;
				//for(int iBand = 0; iBand < currViewShape.nBands; iBand++, sourcePixelAddr++)
				//{
				//	neighProjImg[pixelAddr + iBand] = neighView.smoothImg[sourcePixelAddr];
				//}
				ImageProcessing::BilinearIterpolation(neighProjImg, pixelAddr, 
					                                  neighView.smoothImg, (float) neighViewUV[0], (float) neighViewUV[1], false);
				neighProjLabels[maskAddr] = neighLabelImg[sourceMaskAddr];
				neighProjMask[maskAddr]   = MASK_VALID;
			}
		}
		else
		{
			ENSURE(iPlane <  this->currView->cam.scenePlaneCount);
		}
	}
}

void Stereo::computeAvgProjDepths(int iScenePlane)
{
	CHECK_RANGE(iScenePlane, 0, this->currView->cam.scenePlaneCount - 1);

	const CameraMats &currViewCMs   = this->currView->cam.cameraMats;
	const CIntImage &currViewLabels = this->currView->seg->labelImg;	
	vector<Segment> &currViewSegs   = this->currView->seg->segments;	

	const Plane &currScenePlane = this->currView->cam.scenePlanes[iScenePlane];

	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		currSeg->segData->avgProjDepth[iScenePlane] = 0.0f;
	}

	CShape maskShape = currViewLabels.Shape();
	uint maskAddr = 0;
	for(int v = 0; v < maskShape.height; v++)
	for(int u = 0; u < maskShape.width;  u++, maskAddr++)
	{
		int nodeLabel    = currViewLabels[maskAddr];
		Segment &nodeSeg = currViewSegs[nodeLabel];

		CVector2 imgUV((double) u, (double) v);
		CVector3 worldXYZ;
		currViewCMs.projUVtoXYZ(imgUV, currScenePlane, worldXYZ);

		CVector3 pixelRayDirc = currViewCMs.getRayDirc(imgUV);
		double scenePlaneRayDircAngCos = fabs(currScenePlane.normal.dot(pixelRayDirc));

		if((scenePlaneRayDircAngCos >= STEREO_MinScenePlaneProjPlaneAngCos) &&
		   (InRange(worldXYZ[2], this->currView->cam.minDepth, this->currView->cam.maxDepth)))
			nodeSeg.segData->avgProjDepth[iScenePlane] += (float) worldXYZ[2];
		else
			nodeSeg.segData->avgProjDepth[iScenePlane] = FLT_MAX;
	}

	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		if(currSeg->segData->avgProjDepth[iScenePlane] != FLT_MAX)
		{
			currSeg->segData->avgProjDepth[iScenePlane] /= currSeg->size;
			currSeg->segData->avgProjFrontoPlaneIndex[iScenePlane] = 
				currView->cam.GetBestFrontoPlaneIndex(currSeg->segData->avgProjDepth[iScenePlane]);
		}
		else
		{
			currSeg->segData->avgProjFrontoPlaneIndex[iScenePlane] = SEGMENT_OccDepPlaneIndex;
		}
	}
}

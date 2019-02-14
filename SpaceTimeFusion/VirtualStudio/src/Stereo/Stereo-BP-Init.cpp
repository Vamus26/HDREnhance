#include "Stereo.h"

void Stereo::initBP_setMaskConstraints()
{
	vector<Segment> &currViewSegs = this->currView->seg->segments;
	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		if(currSeg->avgCol.Length() < 0.2)
		{
			int lastPlaneIndex = this->currView->cam.planeCount - 1;

			for(uint iView = 0; iView < this->neighViews.size(); iView++)
			for(int iPlane = 0; iPlane < lastPlaneIndex; iPlane++)
			{
				currSeg->segData->dataCost[iView][iPlane] = 0.00000f;
			}
			
			for(uint iView = 0; iView < this->neighViews.size(); iView++)
			{
				currSeg->segData->dataCost[iView][lastPlaneIndex] = 1.0f;
			}
		}
	}
}


void Stereo::initBP_setScenePtsConstraints()
{
	const Camera &orgRefCam = this->rig.cameras[this->currView->iCam];
	this->currView->cam.ReadViewPts();
	this->currView->cam.RecenterViewPts(orgRefCam.cameraMats);
	
	vector<Segment> &currViewSegs = this->currView->seg->segments;
	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		ENSURE(currSeg->auxData == NULL);
		currSeg->auxData = (void *) new vector<CVector3>;
		ENSURE(currSeg->auxData != NULL);
	}

	const vector<CVector3> &currViewPts = this->currView->cam.viewPoints;
	const CameraMats &currCamMats       = this->currView->cam.cameraMats;
	const CIntImage &currViewLabels     = this->currView->seg->labelImg;
	CShape maskShape = currViewLabels.Shape();

	for(uint iPoint = 0; iPoint < currViewPts.size(); iPoint++)
	{
		CVector3 currViewPt = currViewPts[iPoint];

		if(InRange(currViewPt[2], this->currView->cam.minDepth, this->currView->cam.maxDepth) == false)
			continue;

		CVector2 currUV;
		currCamMats.projXYZtoUV(currViewPt, currUV);
		if(maskShape.InBounds(currUV[0], currUV[1]) == false) 
		{
			REPORT("Warning: Scene point projected out of bounds - (%2.2lf, %2.2lf).\n", currUV[0], currUV[1]);
			continue;
		}

		int segID = currViewLabels.Pixel(NEAREST_INT(currUV[0]),
										 NEAREST_INT(currUV[1]),
										 0);
		Segment &currSeg = currViewSegs[segID];

		((vector<CVector3> *) currSeg.auxData)->push_back(currViewPt);
	}

	float *segBelief = new float[this->currView->cam.planeCount];
	ENSURE(segBelief != NULL);

	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		initBP_setScenePtsConstraints(currSeg, segBelief);
		delete ((vector<CVector3> *) currSeg->auxData);
		currSeg->auxData = NULL;
	}

	delete [] segBelief;
}

void Stereo::initBP_setTrackConstraints()
{
	if(this->rig.IsVideoFrame(this->currView->iCam) == false) return;

	CShape currViewShape = this->currView->smoothImg.Shape();
	Segmentation *currViewSeg = this->currView->seg;	

	for(vector<Track>::const_iterator userDefinedTrack = this->rig.params.userDefinedTracks.begin();
		userDefinedTrack != this->rig.params.userDefinedTracks.end();
		userDefinedTrack++)
	{
		if(this->rig.IsCamCoveredByTrack(this->currView->iCam, *userDefinedTrack) == false) continue;

		CVector2 trackPoint = this->rig.GetTrackPoint(this->currView->iCam, *userDefinedTrack);
		INSIST(currViewShape.InBounds(trackPoint[0], trackPoint[1]));
		trackPoint[1] = currViewShape.height - 1 - trackPoint[1];

		int planeID = userDefinedTrack->planeID;
		CHECK_RANGE(planeID, 0, this->scenePlaneCount - 1);

		int segID = currViewSeg->labelImg.Pixel(NEAREST_INT(trackPoint[0]),
												NEAREST_INT(trackPoint[1]),
												0);
		Segment &segment = currViewSeg->segments[segID];
		for(int iView = 0; iView < currViewSeg->viewCount; iView++)
		{
			memset(segment.segData->dataCost[iView], 0, sizeof(*segment.segData->dataCost[iView]) * 
													    currViewSeg->planeCount);
			segment.segData->dataCost[iView][planeID] = 1.0f;
		}
	}
}

void Stereo::initBP_setScenePtsConstraints(vector<Segment>::iterator currSeg, float *beliefMemBuf)
{	
	vector<CVector3> *predictedSegPoints = (vector<CVector3> *) currSeg->auxData;
	if(predictedSegPoints->size() == 0) return;

	memset(beliefMemBuf, 0, sizeof(*beliefMemBuf) * this->currView->cam.planeCount);

	double distVar = this->stereoParams.pcsParams.ransacParams.ransacThres;
	double expDenom = -2.0 * (distVar * distVar);

	for(uint iPoint = 0; iPoint < predictedSegPoints->size(); iPoint++)
	{
		CVector3 currPoint = predictedSegPoints->at(iPoint);

		if(this->currView->cam.scenePlaneCount == 0)
		{
			int iFrontoPlaneID = this->currView->cam.GetBestFrontoPlaneIndex(currPoint[2]);
			INSIST(iFrontoPlaneID != SEGMENT_OccDepPlaneIndex);
			beliefMemBuf[iFrontoPlaneID] += 1.0f;
		}
		else
		{
			for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
			{
				double distFromPt = FLT_MAX; 

				if(iPlane < this->currView->cam.scenePlaneCount)
				{
					if(currSeg->segData->avgProjFrontoPlaneIndex[iPlane] != SEGMENT_OccDepPlaneIndex)
					{
						distFromPt = this->currView->cam.scenePlanes[iPlane].DistFromPt(currPoint);
					}
				}
				else if(this->stereoParams.onlyUseScenePlanes == false)
				{
					int iFrontPlaneIndex = iPlane - this->currView->cam.scenePlaneCount;
					distFromPt = fabs(this->currView->cam.frontoPlanes[iFrontPlaneIndex] - currPoint[2]);
				}

				beliefMemBuf[iPlane] += (float) exp(distFromPt * distFromPt / expDenom);
			}
		}
	}

	for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
	{
		beliefMemBuf[iPlane] /= predictedSegPoints->size();
	}

	float ptPredictionDataCostFrac    = 0.95f;
	float ptPredictionDataCostFracInv = 1.0f - ptPredictionDataCostFrac;

	for(uint iView = 0; iView < this->neighViews.size(); iView++)
	for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
	{
		currSeg->segData->dataCost[iView][iPlane] *= ptPredictionDataCostFracInv;
		currSeg->segData->dataCost[iView][iPlane] += (ptPredictionDataCostFrac * beliefMemBuf[iPlane]);
	}
}

void Stereo::initBP_data()
{
	initBP_setScenePtsConstraints();
	initBP_setTrackConstraints();

	vector<Segment> &currViewSegs = this->currView->seg->segments;

	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		float beliefDenom = 0.0f;
		for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
		{
			currSeg->segData->belief[iPlane] = currSeg->segData->GetPlaneDatacost(iPlane);
			CHECK_RANGE(currSeg->segData->belief[iPlane], 0.0f, 1.0f);
			beliefDenom += currSeg->segData->belief[iPlane];
		}

		float bestBelief         = 0.0f;
		currSeg->segData->iLabel = SEGMENT_OccDepPlaneIndex;
		for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
		{
			if(beliefDenom > FLT_MIN)
				currSeg->segData->belief[iPlane] /= beliefDenom;
			else
				currSeg->segData->belief[iPlane] = 1.0f / this->currView->cam.planeCount;
			currSeg->segData->belief[iPlane] += STEREO_MinBelief;

			if(bestBelief < currSeg->segData->belief[iPlane])
			{
				if((iPlane >= this->scenePlaneCount) ||
				   (currSeg->segData->avgProjFrontoPlaneIndex[iPlane] != SEGMENT_OccDepPlaneIndex))
				{
					bestBelief = currSeg->segData->belief[iPlane];
					currSeg->segData->iLabel = iPlane;
				}
			}

			for(int iNeigh = 0; iNeigh < currSeg->spatialNeighs.neighCount; iNeigh++)
			{
				currSeg->spatialNeighs.message[iNeigh][iPlane]    = 1.0f / this->currView->cam.planeCount;
				currSeg->spatialNeighs.newMessage[iNeigh][iPlane] = 1.0f / this->currView->cam.planeCount;
			}
		}
		ENSURE(currSeg->segData->iLabel != SEGMENT_OccDepPlaneIndex);

		//projBelief and message should be initialized to the right val in constructors
	}

	const Camera &currCam = this->currView->cam;
	int currFrame = this->currView->iFrame;
	this->currView->seg->SaveSegmentsDynamicData(currCam, currFrame);
	this->currView->seg->UpdatePrevIterDynamicData(currCam, currFrame);
}


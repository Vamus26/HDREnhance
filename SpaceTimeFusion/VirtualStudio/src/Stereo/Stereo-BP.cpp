#include "Stereo.h"
#include "Timer.h"

void Stereo::bp_iteration()
{
	printf("Belief prop iteration for cam-%s, frame-%05i\n", 
			this->currView->cam.id.c_str(), this->currView->iFrame);
	fflush(stdout);	

	if(this->stereoParams.onlyProcessPhotos)
	{
		INSIST(this->rig.IsVideoFrame(this->currView->iCam) == false);
	}

	float *tempMessage = new float[this->currView->cam.planeCount];

	ENSURE(this->stereoParams.spatialKernelSize > 0);
	int spatialKernelSize = min(this->stereoParams.spatialKernelSize, this->currView->cam.frontoPlaneCount - 1);
	float *gKernel = new float[spatialKernelSize + 1];
	float gKernelDenom = spatialKernelSize * spatialKernelSize / log(0.01f); 
	ENSURE(gKernelDenom < 0.0f);
	float gKernelSum = 0.0f;
	for(int i = 0; i <= spatialKernelSize; i++)
	{
		float dist = (float) (i * i);
		gKernel[i] = exp(dist / gKernelDenom);	
		gKernelSum += gKernel[i];
	}
	for(int i = 0; i <= spatialKernelSize; i++)
	{
		gKernel[i] /= gKernelSum;
	}
	
	for(uint iView = 0; iView < this->neighViews.size(); iView++)
	{	
		projectSegments(iView);
	}

	vector<Segment> &currViewSegs = this->currView->seg->segments;
	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		SpatialNeighs &currSpatialNeighs = currSeg->spatialNeighs;

		for(int iNeigh = 0; iNeigh < currSpatialNeighs.neighCount; iNeigh++)
		{
			int spatialNeighID       = currSpatialNeighs.neighID[iNeigh];
			Segment &spatialNeighSeg = currViewSegs[spatialNeighID];
			float spatialNeighWeight = currSpatialNeighs.neighWeight[iNeigh];

			SpatialNeighs &neighSpatialNeighs = spatialNeighSeg.spatialNeighs;
			ViewNeighs *neighViewNeighs       = spatialNeighSeg.viewNeighs;
			
			for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
			{
				tempMessage[iPlane] = 1.0f;

				//neigh messages
				for(int jNeigh = 0; jNeigh < neighSpatialNeighs.neighCount; jNeigh++)
				{
					int neighSpatialNeighID = neighSpatialNeighs.neighID[jNeigh];
					if(neighSpatialNeighID != currSeg->id)
					{					
						tempMessage[iPlane] *= neighSpatialNeighs.message[jNeigh][iPlane];
					}
				}

				//neigh datacost
				for(int iViewNeigh = 0; iViewNeigh < neighViewNeighs->viewCount; iViewNeigh++)
				{
					tempMessage[iPlane] *= neighViewNeighs->projBelief[iViewNeigh][iPlane];
				}

				tempMessage[iPlane] *= this->stereoParams.dataCostScale;
			}

			float msgDenom = 0.0f;
			for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
			{
				float iMsg = 0.0f;

				int iFrontoPlane;
				if(iPlane < this->currView->cam.scenePlaneCount)
					iFrontoPlane = currSeg->segData->avgProjFrontoPlaneIndex[iPlane];
				else
					iFrontoPlane = iPlane - this->currView->cam.scenePlaneCount;

				if(iFrontoPlane != SEGMENT_OccDepPlaneIndex)
				{				
					int start = max(0, iFrontoPlane - spatialKernelSize);
					int end   = min(this->currView->cam.frontoPlaneCount - 1, 
								    iFrontoPlane + spatialKernelSize);

					for(int jFrontoPlane = start; jFrontoPlane <= end; jFrontoPlane++)
					{
						int jPlane = jFrontoPlane + this->currView->cam.scenePlaneCount;
						int frontoPlaneDist = abs(jFrontoPlane - iFrontoPlane);
						iMsg += gKernel[frontoPlaneDist] * tempMessage[jPlane];
					}

					for(int jPlane = 0; jPlane < this->scenePlaneCount; jPlane++)
					{
						int jFrontoPlane = spatialNeighSeg.segData->avgProjFrontoPlaneIndex[jPlane];

						if(jFrontoPlane != SEGMENT_OccDepPlaneIndex)
						{
							if(jPlane == iPlane)
								iMsg += tempMessage[jPlane];
							else
							{							
								int frontoPlaneDist = abs(jFrontoPlane - iFrontoPlane);
								if(frontoPlaneDist <= spatialKernelSize)
								{
									iMsg += gKernel[frontoPlaneDist] * tempMessage[jPlane];
								}
							}
						}
					}
				}

				currSpatialNeighs.newMessage[iNeigh][iPlane] = iMsg;
				msgDenom += iMsg;
			}

			for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
			{			
				float msgElem;
				if(msgDenom > FLT_MIN)
					msgElem = currSpatialNeighs.newMessage[iNeigh][iPlane] / msgDenom;
				else
					msgElem = 1.0f / this->currView->cam.planeCount;
				msgElem *= spatialNeighWeight;
				msgElem += (1.0f - spatialNeighWeight) / this->currView->cam.planeCount;
				currSpatialNeighs.newMessage[iNeigh][iPlane] = msgElem;
			}
		}
	}

	//belief 
	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		float denom = 0.0f;
		SegmentData *currSegData = currSeg->segData;
		ViewNeighs  *currViewNeighs = currSeg->viewNeighs;		
		SpatialNeighs &currSpatialNeighs = currSeg->spatialNeighs;

		for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
		{
			currSegData->belief[iPlane] = 1.0f; 

			//data cost
			for(int iViewNeigh = 0; iViewNeigh < currViewNeighs->viewCount; iViewNeigh++)
			{
				currSegData->belief[iPlane] *= currViewNeighs->projBelief[iViewNeigh][iPlane];
			}

			//incoming messages
			for(int iNeigh = 0; iNeigh < currSpatialNeighs.neighCount; iNeigh++)
			{
				float msgElem = currSpatialNeighs.newMessage[iNeigh][iPlane];
				currSpatialNeighs.message[iNeigh][iPlane] = msgElem;
				currSegData->belief[iPlane] *= msgElem;
			}

			denom += currSegData->belief[iPlane];
		}	

		float bestBelief    = SEGMENT_OccDatacost;
		currSegData->iLabel = SEGMENT_OccDepPlaneIndex;
		for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
		{	
			if(denom > FLT_MIN)
				currSegData->belief[iPlane] /= denom;
			else
				currSegData->belief[iPlane] = 1.0f / this->currView->cam.planeCount;
			currSegData->belief[iPlane] += STEREO_MinBelief;

			if(currSegData->belief[iPlane] > bestBelief)
			{
				if((iPlane >= this->currView->cam.scenePlaneCount) ||
				   (currSegData->avgProjFrontoPlaneIndex[iPlane] != SEGMENT_OccDepPlaneIndex))
				{
					bestBelief = currSegData->belief[iPlane];
					currSegData->iLabel = iPlane;
				}
			}
		}
		ENSURE(currSegData->iLabel != SEGMENT_OccDepPlaneIndex);
	}

	delete tempMessage;
	delete gKernel;
}

void Stereo::projectSegments(int iView)
{
	vector<Segment> &currViewSegs = this->currView->seg->segments;
	const View &nieghView = this->neighViews[iView];

	ENSURE(nieghView.avgFrontoPlaneDist >= FLT_EPSILON);

	float *projBeliefDenom   = new float[currViewSegs.size()];
	float *projBeliefSum     = new float[currViewSegs.size()];
	float *projOccBeliefSum  = new float[currViewSegs.size()];
	float **occBelief        = new float*[currViewSegs.size()]; //belief that I'm not occluding anyone	
	for(uint iSeg = 0; iSeg < currViewSegs.size(); iSeg++)
	{
		occBelief[iSeg] = new float[this->currView->cam.planeCount];
	}

	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++)
	{
		currSeg->segData->notOccProb[iView] = 0.0f;
		for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
		{
			currSeg->viewNeighs->projBelief[iView][iPlane] = 1.0f;
		}
	}

	if(this->stereoParams.onlyProcessPhotos)
	{
		if(this->rig.IsVideoFrame(nieghView.iCam) == true)
		{
			return;
		}
	}

	for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
	{	
		memset(projBeliefDenom,  0, sizeof(*projBeliefDenom)  * currViewSegs.size());
		memset(projBeliefSum,    0, sizeof(*projBeliefSum)    * currViewSegs.size());
		memset(projOccBeliefSum, 0, sizeof(*projOccBeliefSum) * currViewSegs.size());

		const Plane &currPlane = this->currView->cam.planes[iPlane];

		int iSeg = 0;
		for(vector<Segment>::iterator currSeg = currViewSegs.begin();
			currSeg != currViewSegs.end(); 
			currSeg++, iSeg++)
		{
			ViewNeighs *currViewNeighs = currSeg->viewNeighs;
			int overlapSegCount = currViewNeighs->segCount[iView][iPlane];
			if(overlapSegCount == 0) continue;

			if(iPlane < this->scenePlaneCount)
			{
				INSIST(currSeg->segData->avgProjFrontoPlaneIndex[iPlane] != SEGMENT_OccDepPlaneIndex);
			}

			CVector2 segImageLoc(currSeg->point[0], currSeg->point[1]);
			CVector3 segCurrViewWorldLoc;			
			this->currView->cam.cameraMats.projUVtoXYZ(segImageLoc, currPlane, segCurrViewWorldLoc);
			CVector3 segNeighViewWorldLoc = nieghView.cam.cameraMats.transformCoordSys(segCurrViewWorldLoc);
				
			int iFrontoPlaneInNeighView = nieghView.cam.GetBestFrontoPlaneIndex(segNeighViewWorldLoc[2]);
			if(iFrontoPlaneInNeighView == SEGMENT_OccDepPlaneIndex) continue;

			int iPlaneInNeighView;
			if(iPlane < this->scenePlaneCount)
				iPlaneInNeighView = iPlane;
			else
				iPlaneInNeighView = iFrontoPlaneInNeighView + this->scenePlaneCount;

			for(int iOverlap = 0; iOverlap < overlapSegCount; iOverlap++)
			{
				int overlapSize           = currViewNeighs->segOverlapCount[iView][iPlane][iOverlap];
				float overlapWeight       = (float) overlapSize / currSeg->size;
				int overlapSegID          = currViewNeighs->segID[iView][iPlane][iOverlap];
				const Segment &overlapSeg = nieghView.seg->segments[overlapSegID];

				projBeliefDenom[iSeg] += overlapWeight;

				projBeliefSum[iSeg] += overlapSeg.segData->belief[iPlaneInNeighView] * overlapWeight;

				int overlapSegPlaneID = overlapSeg.segData->iLabel;
				ENSURE(overlapSegPlaneID != SEGMENT_OccDepPlaneIndex);

				int overlapSegFrontoPlaneID;
				if(overlapSegPlaneID < this->scenePlaneCount)
					overlapSegFrontoPlaneID = overlapSeg.segData->avgProjFrontoPlaneIndex[overlapSegPlaneID];
				else
					overlapSegFrontoPlaneID = overlapSegPlaneID - this->scenePlaneCount;									
				ENSURE(overlapSegFrontoPlaneID != SEGMENT_OccDepPlaneIndex);

				if(overlapSegFrontoPlaneID > iFrontoPlaneInNeighView + (STEREO_OccDepthTolerance / nieghView.avgFrontoPlaneDist))
				{
					float occBeliefSumIncr = 0.0f;

					if(overlapSegPlaneID < this->scenePlaneCount)
						occBeliefSumIncr += overlapSeg.segData->belief[overlapSegPlaneID];
					else
					{
						for(int iScenePlane = 0; iScenePlane < this->scenePlaneCount; iScenePlane++)
						{
							int avgFrontoPlaneIndex = overlapSeg.segData->avgProjFrontoPlaneIndex[iScenePlane];
							if((avgFrontoPlaneIndex != SEGMENT_OccDepPlaneIndex) &&
							   abs(avgFrontoPlaneIndex - overlapSegFrontoPlaneID) <= 1)
							{
								occBeliefSumIncr += overlapSeg.segData->belief[iScenePlane];
							}
						}
					}

					occBeliefSumIncr += overlapSeg.segData->belief[overlapSegFrontoPlaneID + this->scenePlaneCount];

					if(overlapSegFrontoPlaneID > 0)
						occBeliefSumIncr += overlapSeg.segData->belief[overlapSegFrontoPlaneID + this->scenePlaneCount - 1];

					if(overlapSegFrontoPlaneID < nieghView.cam.frontoPlaneCount - 1)
						occBeliefSumIncr += overlapSeg.segData->belief[overlapSegFrontoPlaneID + this->scenePlaneCount + 1];

					projOccBeliefSum[iSeg] += occBeliefSumIncr * overlapWeight;
				}
			}
		}

		iSeg = 0;
		for(vector<Segment>::iterator currSeg = currViewSegs.begin();
			currSeg != currViewSegs.end(); 
			currSeg++, iSeg++)
		{
			if(projBeliefDenom[iSeg] > FLT_MIN)
			{
				ENSURE(fabs(projBeliefDenom[iSeg] - 1.0f) < 0.001);
				projBeliefSum[iSeg]    /= projBeliefDenom[iSeg];
				projOccBeliefSum[iSeg] /= projBeliefDenom[iSeg];

				currSeg->segData->notOccProb[iView] += projBeliefSum[iSeg];
				currSeg->viewNeighs->projBelief[iView][iPlane] = projBeliefSum[iSeg];
				occBelief[iSeg][iPlane] = max(0.0f, 1.0f - projOccBeliefSum[iSeg]);
			}
			else
			{
				currSeg->viewNeighs->projBelief[iView][iPlane] = STEREO_MinBelief;	
				occBelief[iSeg][iPlane] = 1.0f - STEREO_MinBelief;
			}

			if(currSeg->segData->alwaysInViewBounds[iView] == false)
			{
				currSeg->viewNeighs->projBelief[iView][iPlane] = 1.0f / this->currView->cam.planeCount;
			}
		}
	}

	int iSeg = 0;
	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
		currSeg != currViewSegs.end(); 
		currSeg++, iSeg++)
	{
		SET_IF_LT(currSeg->segData->notOccProb[iView], 1.0f);
		float notOccProb = currSeg->segData->notOccProb[iView];
		
		float denomOcc  = 0.0f;
		float denomProj = 0.0f;
		for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
		{
			denomOcc  += occBelief[iSeg][iPlane];
			denomProj += currSeg->viewNeighs->projBelief[iView][iPlane];
		}

		for(int iPlane = 0; iPlane < this->currView->cam.planeCount; iPlane++)
		{
			if(denomProj > FLT_MIN)
				currSeg->viewNeighs->projBelief[iView][iPlane] /= denomProj;
			else
				currSeg->viewNeighs->projBelief[iView][iPlane] = 1.0f / this->currView->cam.planeCount;

			if(denomOcc > FLT_MIN)
				occBelief[iSeg][iPlane] /= denomOcc;
			else
				occBelief[iSeg][iPlane] = 1.0f / this->currView->cam.planeCount;

			float oldProjBelief = currSeg->viewNeighs->projBelief[iView][iPlane];
			float newProjBelief = notOccProb * oldProjBelief * currSeg->segData->dataCost[iView][iPlane];
			newProjBelief += (1.0f - notOccProb) * occBelief[iSeg][iPlane];
			currSeg->viewNeighs->projBelief[iView][iPlane] = newProjBelief;			
		}
	}

	delete [] projBeliefDenom;
	delete [] projBeliefSum;
	delete [] projOccBeliefSum;
	for(uint iSeg = 0; iSeg < currViewSegs.size(); iSeg++)
	{
		delete [] occBelief[iSeg];
	}
	delete [] occBelief;
}


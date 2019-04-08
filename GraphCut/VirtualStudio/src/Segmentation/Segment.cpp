#include "Segment.h"
#include <algorithm>

Segment::Segment()
{
	this->id         = -1;
	this->size       = 0;
	this->point      = Vector2<float>(0.0f, 0.0f);
	this->avgCol     = Vector3<float>(0.0f, 0.0f, 0.0f);
	this->auxInt     = 0;
	this->segData    = NULL;
	this->viewNeighs = NULL;
	this->auxData    = NULL;
}

Segment::~Segment()
{
	if(this->segData != NULL)
		delete this->segData;

	if(this->viewNeighs != NULL)
		delete this->viewNeighs;

	ENSURE(this->auxData == NULL);
}

void Segment::AllocBuffers(int viewCount, int planeCount, int scenePlaneCount)
{
	ENSURE(this->segData    == NULL);
	ENSURE(this->viewNeighs == NULL);

	this->segData    = new SegmentData(viewCount, planeCount, scenePlaneCount, true);
	this->viewNeighs = new ViewNeighs(viewCount,  planeCount);	
	this->spatialNeighs.planeCount = planeCount;
}

float Segment::GetBestDatacost(int iView, int &bestPlaneIndex)
{
	ENSURE(this->segData != NULL);

	bestPlaneIndex  = SEGMENT_OccDepPlaneIndex;
	float bestPlaneCost = SEGMENT_OccDatacost;
	for(int iPlane = 0; iPlane < this->segData->planeCount; iPlane++)
	{
		float currPlaneCost = this->segData->dataCost[iView][iPlane];
		if(currPlaneCost > bestPlaneCost)
		{
			bestPlaneCost = currPlaneCost;
			bestPlaneIndex = iPlane;
		}
	}

	if((bestPlaneIndex >= 0) && 
	   (bestPlaneIndex < this->segData->scenePlaneCount))
	{
		ENSURE(this->segData->avgProjFrontoPlaneIndex[bestPlaneIndex] != SEGMENT_OccDepPlaneIndex);
	}

	return bestPlaneCost;
}


float Segment::GetWorstDatacost(int iView, int &worstPlaneIndex)
{
	ENSURE(this->segData != NULL);
	ENSURE(this->segData->planeCount > 0);

	worstPlaneIndex  = 0;
	float worstPlaneCost = this->segData->dataCost[iView][0];
	for(int iPlane = 1; iPlane < this->segData->planeCount; iPlane++)
	{
		float currPlaneCost = this->segData->dataCost[iView][iPlane];
		if(currPlaneCost < worstPlaneCost)
		{
			worstPlaneCost  = currPlaneCost;
			worstPlaneIndex = iPlane;
		}
	}

	return worstPlaneCost;
}


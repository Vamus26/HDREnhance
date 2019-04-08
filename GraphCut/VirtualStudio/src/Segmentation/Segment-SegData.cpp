#include "Segment.h"
#include <algorithm>

SegmentData::SegmentData(int viewCount, int planeCount, int scenePlaneCount, bool allocStaticData)
{
	this->viewCount       = viewCount;
	this->planeCount      = planeCount;
	this->scenePlaneCount = scenePlaneCount;
	this->iLabel          = SEGMENT_OccDepPlaneIndex;

	if(allocStaticData == false)
	{
		this->dataCost = NULL;
		this->alwaysInViewBounds = NULL;
	}
	else
	{
		this->dataCost     = new float*[this->viewCount];		
		ENSURE(this->dataCost != NULL);		
		for(int iView = 0; iView < this->viewCount; iView++)
		{		
			this->dataCost[iView] = new float[this->planeCount];
			ENSURE(this->dataCost[iView] != NULL);
			memset(this->dataCost[iView], 0, this->planeCount * sizeof(float));
		}

		this->alwaysInViewBounds = new bool[this->viewCount];
		ENSURE(this->alwaysInViewBounds != NULL);
		for(int iView = 0; iView < this->viewCount; iView++)
		{
			this->alwaysInViewBounds[iView] = true;			
		}
	}

	
	if(this->scenePlaneCount == 0)
	{
		this->avgProjDepth = NULL;
		this->avgProjFrontoPlaneIndex = NULL;
	}
	else
	{
		this->avgProjDepth = new float[this->scenePlaneCount];
		ENSURE(this->avgProjDepth != NULL);

		this->avgProjFrontoPlaneIndex = new int[this->scenePlaneCount];
		ENSURE(this->avgProjFrontoPlaneIndex != NULL);
	}

	this->belief = new float[this->planeCount];
	ENSURE(this->belief != NULL);
	for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
	{
		this->belief[iPlane] = 1.0f / this->planeCount;
	}

	this->notOccProb = new float[viewCount];
	ENSURE(this->notOccProb != NULL);
	for(int iView = 0; iView < this->viewCount; iView++)
	{
		this->notOccProb[iView] = 1.0f;
	}
}

SegmentData::~SegmentData()
{
	if(this->dataCost != NULL)
	{
		for(int iView = 0; iView < this->viewCount; iView++)
		{
			delete [] this->dataCost[iView];
		}
		delete [] this->dataCost;
	}

	if(this->avgProjDepth != NULL)
	{
		ENSURE(this->avgProjFrontoPlaneIndex != NULL);
		delete [] this->avgProjFrontoPlaneIndex;
		delete [] this->avgProjDepth;
	}

	if(this->belief != NULL)
		delete [] this->belief;

	if(this->notOccProb != NULL)
		delete [] this->notOccProb;

	if(this->alwaysInViewBounds != NULL)
		delete [] this->alwaysInViewBounds;
}

float SegmentData::getPlaneDatacostProd(int iPlane)
{
	ENSURE(this->dataCost != NULL);
	CHECK_RANGE(iPlane, 0, this->planeCount - 1);

	float segDataCost   = 1.0f;
	int validViewsFound = 0;
	for(int iView = 0; iView < this->viewCount; iView++)
	{
		float viewDatacost = this->dataCost[iView][iPlane];
		if(viewDatacost != SEGMENT_OccDatacost)
		{
			validViewsFound++;
			segDataCost *= viewDatacost;
		}
	}

	if(validViewsFound == 0)
		return SEGMENT_OccDatacost;
	else
		return pow(segDataCost, 1.0f / validViewsFound);
}

float SegmentData::getPlaneDatacostAvg(int iPlane)
{
	ENSURE(this->dataCost != NULL);
	CHECK_RANGE(iPlane, 0, this->planeCount - 1);

	float segDataCost   = 0.0f;
	int validViewsFound = 0;
	for(int iView = 0; iView < this->viewCount; iView++)
	{
		float viewDatacost = this->dataCost[iView][iPlane];
		if(viewDatacost != SEGMENT_OccDatacost)
		{
			validViewsFound++;
			segDataCost += viewDatacost;
		}
	}

	if(validViewsFound == 0)
		return SEGMENT_OccDatacost;
	else
		return segDataCost / validViewsFound;
}

float SegmentData::getPlaneDatacostMed(int iPlane)
{
	ENSURE(this->dataCost != NULL);
	CHECK_RANGE(iPlane, 0, this->planeCount - 1);

	vector<float> segDataCosts;
	segDataCosts.reserve(this->viewCount);

	int validViewsFound = 0;
	for(int iView = 0; iView < this->viewCount; iView++)
	{
		float viewDatacost = this->dataCost[iView][iPlane];
		if(viewDatacost != SEGMENT_OccDatacost)
		{
			validViewsFound++;
			segDataCosts.push_back(viewDatacost);
		}
	}

	float segDataCost;

	switch(validViewsFound)
	{
	case 0:
		segDataCost = SEGMENT_OccDatacost;
		break;

	case 1:
		segDataCost = segDataCosts[0];
		break;

	case 2:
		segDataCost = (segDataCosts[0] + segDataCosts[1]) / 2.0f;
		break;

	default:
		std::sort(segDataCosts.begin(), segDataCosts.end());
		int medIndex = (int) segDataCosts.size() / 2;
		segDataCost = segDataCosts[medIndex];
		break;
	}

	return segDataCost;		
}

int SegmentData::GetStaticDataFileSize()
{
	ENSURE(this->dataCost != NULL);
	int fileSize = 0;
	fileSize += this->viewCount * this->planeCount * sizeof(float); //datacosts
	fileSize += this->viewCount * sizeof(bool); //alwaysInViewBounds
	return fileSize;
}

void SegmentData::SerializeStaticData(char **memBuffPtr)
{
	ENSURE(this->dataCost != NULL);	
	ENSURE(this->viewCount > 0);
	ENSURE(this->planeCount > 0);

	char *memBuffCopy = *memBuffPtr;	

	for(int iView  = 0; iView  < this->viewCount;  iView++)
	{
		WRITE_BOOL(memBuffPtr, this->alwaysInViewBounds[iView]);
	}

	for(int iView  = 0; iView  < this->viewCount;  iView++)
	for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
	{
		WRITE_FLOAT(memBuffPtr, this->dataCost[iView][iPlane]);
	}

	ENSURE((*memBuffPtr - memBuffCopy) == GetStaticDataFileSize());
}

void SegmentData::UnserializeStaticData(char **memBuffPtr)
{
	ENSURE(this->dataCost != NULL);
	ENSURE(this->viewCount > 0);
	ENSURE(this->planeCount > 0);

	char *memBuffCopy = *memBuffPtr;

	for(int iView  = 0; iView  < this->viewCount;  iView++)
	{
		READ_BOOL(memBuffPtr, this->alwaysInViewBounds[iView]);
	}

	for(int iView  = 0; iView  < this->viewCount;  iView++)
	for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
	{
		READ_FLOAT(memBuffPtr, this->dataCost[iView][iPlane]);
	}

	ENSURE((*memBuffPtr - memBuffCopy) == GetStaticDataFileSize());
}

int SegmentData::GetBeliefFileSize()
{
	return (this->planeCount      * sizeof(float)) + //belief
		   (this->viewCount       * sizeof(float)) + //notOccProb
		   (this->scenePlaneCount * sizeof(float)) + //avgDepths
		   (this->scenePlaneCount * sizeof(int));    //avgProjFrontoPlaneIndex
}

void SegmentData::SerializeBelief(char **memBuffPtr)
{
	char *memBuffCopy = *memBuffPtr;

	for(int iScenePlane = 0; iScenePlane < this->scenePlaneCount; iScenePlane++)
	{
		WRITE_FLOAT(memBuffPtr, this->avgProjDepth[iScenePlane]);
		WRITE_INT(  memBuffPtr, this->avgProjFrontoPlaneIndex[iScenePlane]);
	}

	for(int iView = 0; iView < this->viewCount; iView++)
	{
		WRITE_FLOAT(memBuffPtr, this->notOccProb[iView]);
	}

	float beliefSum = 0.0f;
	for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
	{
		WRITE_FLOAT(memBuffPtr, this->belief[iPlane]);
		beliefSum += this->belief[iPlane];
	}

	CHECK_RANGE(fabs(beliefSum - 1.0f), 0.0f, 0.01f);

	ENSURE((*memBuffPtr - memBuffCopy) == GetBeliefFileSize());
}

void SegmentData::UnserializeBelief(char **memBuffPtr)
{
	char *memBuffCopy = *memBuffPtr;

	for(int iScenePlane = 0; iScenePlane < this->scenePlaneCount; iScenePlane++)
	{
		READ_FLOAT(memBuffPtr, this->avgProjDepth[iScenePlane]);
		READ_INT(  memBuffPtr, this->avgProjFrontoPlaneIndex[iScenePlane]);
	}

	for(int iView = 0; iView < this->viewCount; iView++)
	{
		READ_FLOAT(memBuffPtr, this->notOccProb[iView]);
	}

	float beliefSum       = 0.0f;
	float bestBelief      = SEGMENT_OccDatacost;
	int bestBeliefPlaneID = SEGMENT_OccDepPlaneIndex;
	for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
	{
		READ_FLOAT(memBuffPtr, this->belief[iPlane]);
		beliefSum += this->belief[iPlane];
		if(bestBelief < this->belief[iPlane])
		{
			//check - gen-planes
			if((iPlane >= this->scenePlaneCount) ||
			   (this->avgProjFrontoPlaneIndex[iPlane] != SEGMENT_OccDepPlaneIndex))
			{
				bestBelief = this->belief[iPlane];
				bestBeliefPlaneID = iPlane;
			}
		}
	}
	CHECK_RANGE(fabs(beliefSum - 1.0f), 0.0f, 0.01f);
	ENSURE(bestBeliefPlaneID != SEGMENT_OccDepPlaneIndex);

	this->iLabel = bestBeliefPlaneID;

	ENSURE((*memBuffPtr - memBuffCopy) == GetBeliefFileSize());
}

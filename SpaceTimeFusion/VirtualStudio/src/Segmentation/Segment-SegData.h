#include "Definitions.h"
#ifndef __SEGMENT_SEG_DATA_H__
#define __SEGMENT_SEG_DATA_H__

class SegmentData
{
public:
	float **dataCost;	
	float *belief;
	float *notOccProb;
	float *avgProjDepth; //check - get rid of this array
	int   *avgProjFrontoPlaneIndex;
	bool  *alwaysInViewBounds;

	int    viewCount;
	int    planeCount;	
	int    scenePlaneCount;
	int    iLabel;

	SegmentData(int viewCount, int planeCount, int scenePlaneCount,
		        bool allocStaticData);
	~SegmentData();	

	float GetPlaneDatacost(int iPlane)
	{
		return getPlaneDatacostProd(iPlane);
	}

	int  GetStaticDataFileSize();
	void SerializeStaticData(char **memBuffPtr);
	void UnserializeStaticData(char **memBuffPtr);

	int  GetBeliefFileSize();
	void SerializeBelief(char **memBuffPtr);
	void UnserializeBelief(char **memBuffPtr);

private:
	float getPlaneDatacostProd(int iPlane);
	float getPlaneDatacostAvg(int iPlane);
	float getPlaneDatacostMed(int iPlane);
};

#endif

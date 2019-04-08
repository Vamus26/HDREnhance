#include "Definitions.h"

#ifndef __SEGMENT_VIEW_NEIGH_H__
#define __SEGMENT_VIEW_NEIGH_H__

typedef unordered_map<int, unsigned short> SegOverlapBookeeper;

class ViewNeighs
{
private:
	SegOverlapBookeeper *segOverlapBookeeper;

public:	
	int            ***segID;
	unsigned short ***segOverlapCount;
	unsigned short **segCount;
	float          **projBelief;

	int planeCount;
	int viewCount;

	ViewNeighs(int viewCount, int planeCount);
	~ViewNeighs();

	void StartViewPlaneBooking();
	void FoundOverlapPixel(int segID);
	void DoneViewPlaneBooking(int iView, int iPlane);

	int GetStaticDataFileSize();
	void SerializeStaticData(char **memBuffPtr);
	void UnserializeStaticData(char **memBuffPtr);
};

#endif

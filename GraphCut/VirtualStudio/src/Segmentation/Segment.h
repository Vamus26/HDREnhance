#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include "Definitions.h"
#include "Image.h"
#include "Segment-mem.h"
#include "Segment-Bounds.h"
#include "Segment-SegData.h"
#include "Segment-ViewNeighs.h"
#include "Segment-SpatialNeighs.h"
#include "Transform-3x3.h"
#include "Transform-2x2.h"


const static float SEGMENT_OccDatacost      = 0.0f;
const static int   SEGMENT_OccDepPlaneIndex = -1;

class Segment
{
public:
	int id;
	int size;
	Bounds bounds;
	Vector2<float> point;
	Vector3<float> avgCol;
	int auxInt;
	SegmentData *segData;
	SpatialNeighs spatialNeighs;
	ViewNeighs *viewNeighs;
	void *auxData;

public: //Segment.cpp
	Segment();
	~Segment();
	void AllocBuffers(int viewCount, int planeCount, int scenePlaneCount);
	float GetBestDatacost(int iView, int &bestPlaneIndex);
	float GetWorstDatacost(int iView, int &worstPlaneIndex);

public: //Segment-IO.h
	int GetStaticDataFileSize();
	void SerializeStaticData(char **memBuffPtr);
	void UnserializeStaticData(char **memBuffPtr);

public: //Segment-Debug.cpp
	void Dump() const;
	void CheckSizeAndColorConsistency(const CIntImage &labelImg, const CFloatImage &smoothImg) const;
	void CheckBoundTighness(const CIntImage &labelImg) const;
	CByteImage GetSegVis(const CIntImage &labelImg) const;
};

#endif

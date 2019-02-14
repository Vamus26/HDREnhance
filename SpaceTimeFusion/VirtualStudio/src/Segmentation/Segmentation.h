#ifndef __SEGMENTATION_H__
#define __SEGMENTATION_H__

#include "Definitions.h"
#include "Segment.h"
#include "Image.h"
#include "Camera.h"

class Segmentation
{
public:
	CFloatImage smoothImg;
	CIntImage   labelImg;
	vector<Segment> segments;
	int viewCount;
	int planeCount;
	int scenePlaneCount;
	bool fullyComputed;

public: //Segmentation.cpp
	Segmentation();
	void CreateSegments();
	void CreateSegments(int segCount);
	void AllocSegmentBuffers(int viewCount, int planeCount, int scenePlaneCount);
	void ComputeSegAvgColors();
	void ComputeSpatialNeighs();
	void CreateSegmentPoints();

private: //Segmentation.cpp

public: //Segmentation-IO.cpp
	bool Load(const Camera &camera, int iFrame, bool loadBeliefDataOnly);
	void Save(const Camera &camera, int iFrame);

	void SaveSegmentsStaticData(const Camera &camera, int iFrame);
	void SaveSegmentsDynamicData(const Camera &camera, int iFrame);	

	static void UpdatePrevIterDynamicData(const Camera &camera, int iFrame);
	static bool CurrIterDynamicDataExists(const Camera &camera, int iFrame);

private:
	int  getStaticDataFileSize();
	void serializeStaticData(char **memBuffPtr);
	void unserializeStaticData(char **memBuffPtr);	

	char* getBeliefsSerialization(int &memSize);
	void  unserializeBeliefs(char **memBuffPtr);

	char* getMsgesSerialization(int &memSize);
	void  unserializeMsges(char **memBuffPtr);

	void loadSegmentsStaticData(const Camera &camera, int iFrame); 

	void saveSegmentsBeliefs(const Camera &camera, int iFrame); 
	void loadSegmentsBeliefs(const Camera &camera, int iFrame); 

	void saveSegmentsMessages(const Camera &camera, int iFrame); 
	void loadSegmentsMessages(const Camera &camera, int iFrame); 


public: // Segmentation-Debug.cpp
	void CheckSegmentationConsistency(int minSegSize, int maxSegSize);
	CByteImage GetBoundaryImg();
	CFloatImage GetDatacostImg(int iPlane);
	CFloatImage GetSegmentationImg();
	void DumpSegAndNeighVis(int segID);
	int GetSegID(int x, int y, bool flipY = true);
	CFloatImage GetNotOccProbImg(int iView);
};

#endif

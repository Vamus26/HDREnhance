#ifndef __STEREO_H__
#define __STEREO_H__

#include "Definitions.h"
#include "Rig.h"
#include "CamFrameContext.h"
#include "Segmenter.h"
#include "PCloudSegmenter.h"
#include "Plane.h"
#include "ScenePoints.h"

const static float STEREO_OccDepthTolerance = 4;
const float STEREO_MinBelief = 0.0000001f;

class Stereo
{
public:
	typedef struct _StereoParams
	{
		CVector3 pcLowerBound;
		CVector3 pcUpperBound;
		float noiseVar;
		float dataCostScale;
		float dataCostExp;
		int bpIters;
		bool processInMem;
		int spatialKernelSize;
		float pixelDistBetweenFrontoPlanes;
		bool onlyProcessPhotos;
		bool onlyUseScenePlanes;

		Segmenter::SegmenterParams segmenterParams;
		PCloudSegmenter::PCloudSegmenterParams pcsParams;

		static _StereoParams Default();
		void Dump(string prefix);
	} StereoParams;	

public:
	View         *currView;
	vector<View> neighViews;
	Rig          rig;

	StereoParams    stereoParams;	
	int             scenePlaneCount;

protected:

	Segmentation **camSegmentations;

public: //Stereo.cpp
	Stereo();
	~Stereo();	

	void ComputeStereo();

	void ComputeStereoIter();
	void CleanUpStereo();
	void AdvanceStereoIter();

	void BackUpBP_Data();

	void Step();
	
	CFloatImage GetCurrViewDepthsVis();

	CShortImage GetCurrViewPlaneIndexImage();

	CFloatImage GetDepthsVis(const CShortImage &planeIndexImg, 
							 const Camera &camera,
							 bool usePureDepths = true);

	CFloatImage GetDepthsVis(const CShortImage &planeIndexImg, bool usePureDepths = true);


private: //Stereo.cpp

	void processCurrCamFrame();

	void computeFrameStereo(int iFrame);

	void updatePrevIterDynamicData(int iFrame);

	void writeDepthImages();


private: //Stereo-StaticData.cpp
	void computeCurrViewStaticData();

	void computeStaticData(int iView, int iPlane);

	void computeDatacosts(int iView, int iPlane,
						   const CFloatImage &neighProjImg,
						   const CByteImage  &neighProjMask);

	void sumPixelDatacosts(int iView, int iPlane,
						   const CFloatImage &neighProjImg,
						   const CByteImage  &neighProjMask);

	void normalizeDatacosts();

	void computeNeighOverlapStats(int iView, int iPlane,
							 	  const CFloatImage &neighProjImg,
							      const CIntImage &neighProjLabels, 
								  const CByteImage &neighProjMask);

	void createNeighProjection(int iView, int iPlane,
							   CFloatImage &neighProjImg, 
							   CIntImage   &neighProjLabels,
							   CByteImage  &neighProjMask);

	void computeAvgProjDepths(int iScenePlane);

protected: //Stereo-StaticData.cpp	

private:
	void initBP_setScenePtsConstraints();
	void initBP_setTrackConstraints();
	void initBP_setMaskConstraints();
	void initBP_setScenePtsConstraints(vector<Segment>::iterator currSeg, float *beliefMemBuf);

	void initBP_data();
	void bp_iteration();
	void projectSegments(int iView);

protected: //Stereo-Load.cpp
	void loadFrameSegmentations(int iFrame);
	void unloadFrameSegmentations(int iFrame);
	void loadCamFrameContext(int iCam, int iFrame, int iIter, bool createNeighSegmentationsIfAbsent = false);
	Segmentation *loadSegmentation(int iCam, int iFrame, bool loadBeliefDataOnly, bool createSegmentationIfAbsent);
	void unloadCurrCamFrameContext();	
	void unloadSegmentation(int iCam, int iFrame,bool saveDynamicData);

public: //Stereo-Init.cpp
	void Init(StereoParams stereoParams, Rig::RigParams rigParams);

private: //Stereo-Init.cpp
	void loadPlanes();

	void getCamPairForFrontoPlaneSearch(const Camera **leftCamPtr, 
							            const Camera **rightCamPtr);
	void readFrontoPlanesFile();

	void findScenePlanes(vector<Plane> &scenePlanes, const ScenePoints &scenePoints);
	void extractUserDefinedPlanes(vector<Plane> &scenePlanes);
	void writeScenePlanesFile(const vector<Plane> &scenePlanes);

public: //Stereo-Init.cpp
	static void ReadFrontoPlanesFile(vector<double> &frontoPlanes, string frontoPlanesFN);

private: //Stereo-FileNames.cpp
	string getScenePlanesFN();
public: //Stereo-FileNames.cpp
	static string GetStereoDirName(Rig &rig);	

protected: //Stereo-Debug.cpp
	void dumpEpipolarLineVis(int refCamID, int neighCamID, 
		                     int x, int y, bool flipY = true);

	void dumpEpipolarLineVis(CFloatImage &refImg, CFloatImage &neighImg,
						     int refCamID, int neighCamID, 
		                     int x, int y, bool flipY = true);

	void dumpScenePts(CFloatImage &image, int camID, const vector<CVector3> &scenePts);
	void dumpScenePts(CFloatImage &image, int camID, const PointCloud &scenePts);

	CByteImage getSegsNotVisMask(int iPlane);

	CByteImage getPlanesOutOfBoundsMask(int iView);

	void visScenePlaneProjections(int iView);	

	void visSFM_Geo();
};

#endif

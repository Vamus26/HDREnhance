#include "Definitions.h"

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "CameraDefs.h"
#include "CameraMats.h"
#include "Image.h"
#include "CameraNeigh.h"
#include "Plane.h"
#include "ScenePoints.h"

class ScenePoints;

const static double CAMERA_MinDepth = 0.000001;

class Camera
{
public:
	string path;
	string id;	
	int frameCount;
	CameraNeighs cameraNeighs;
	vector<CVector3> viewPoints;
	CameraMats cameraMats;
	vector<double>  frontoPlanes;
	vector<Plane>   scenePlanes;
	vector<Plane>   planes;
	int             frontoPlaneCount;
	int             planeCount;
	int             scenePlaneCount;
	double          minDepth;
	double          maxDepth;

public: //Camera.cpp
	
	Camera();
	Camera(string path);
	void Load(string path);
	void Recenter(const CameraMats refCamMats);
	void PrintCamNeighs();
	void WriteViewPts();
	void ScaleView(float scale);

	void ReadViewPts();
	void RecenterViewPts(const CameraMats refCamMats);	

	void BackUpBP_Data(int iFrame) const;
	
private: //Camera.cpp
	void init();
	

public: //Camera-Accessors.cpp
	CShape GetFrameShape(int frame, float scale = 1.0f) const;
		
public: //Camera-Checks.cpp
	void CheckForBasicData();

public: //Camera-Filename.cpp
	string GetFrameName(int iFrame) const;
	string GetFrameNameCrop(int iFrame) const;
	string GetFrameNameLarge(int iFrame) const;
	string GetFrameAnalogyName(int iFrame) const;
	string GetFrameAnalogyMaskName(int iFrame) const;
	string GetFrameAnalogyNameLarge(int iFrame) const;
	string GetFrameSmoothName(int iFrame) const;
	string GetFrameSegName(int iFrame) const;
	string GetFrameBeliefsName(int iFrame) const;
	string GetFrameMsgesName(int iFrame) const;
	string GetFramePrevBeliefsName(int iFrame) const;
	string GetFramePrevMsgesName(int iFrame) const;
	string GetFrameBakPrevBeliefsName(int iFrame) const;
	string GetFrameBakPrevMsgesName(int iFrame) const;
	string GetFrameLabelName(int iFrame) const;
	string GetFrameLabelEdgesName(int iFrame) const;
	string GetFrameDepsVisName(int iFrame) const;
	string GetFrameDepsName(int iFrame) const;
	string GetFrameDepsNameAlt(int iFrame) const;
	string GetFrameFlowDataCostFN(int iFrame, int offsetX, int offsetY) const;
	string GetFramesDirName() const;
	string GetDepsDirName() const;
	string GetSegDirName() const;
	string GetSegBakDirName() const;
	string GetFlowDirName() const;	
	string GetTimeStampsFN() const;
	string GetCameraMatsFN() const;
	string GetCameraViewPtsFN() const;
	string GetFrameFlowName(int iFrame) const;
	string GetFrameRevFlowName(int iFrame) const;
	string GetFrameFlowNameCrop(int iFrame) const;
	string GetFrameRevFlowNameCrop(int iFrame) const;
	string GetFrameFlowNameLarge(int iFrame) const;
	string GetFrameRevFlowNameLarge(int iFrame) const;
	string GetScenePlanesFN(int iFrame) const;
	string GetFrontoPlanesFN() const;
	string GetFrameReconMapName(int iFrame) const;

private: //Camera-Filename.cpp	
	string getFramesPattern();	

public: //Camera-Planes.cpp
	void LoadPlanes(int iFrame);
	void CreatePlanes(int iFrame,
					  const vector<Plane> &globalScenePlanes, 
					  const ScenePoints &scenePoints,
					  CVector3 pcLowerBound, CVector3 pcUpperBound,
					  double pixelDistBetweenFrontoPlanes,
					  vector<Camera *> neighCams); 


	static void ReadFrontoPlanesFile(vector<double> &fPlanes, string frontoPlanesFN);

	int GetBestFrontoPlaneIndex(double depth) const;

private: //Camera-Planes.cpp

	void createScenePlanes(const vector<Plane> &globalPlanes); 

	void createFrontoParallelPlanes(const ScenePoints &scenePoints,
									CVector3 pcLowerBound, CVector3 pcUpperBound, 
									double pixelDistBetweenFrontoPlanes,
									vector<Camera *> neighCams);

	void createPlanes();

	void writeScenePlanesFile(int iFrame);		
	void writeFrontoPlanesFile();

	void getMinMaxDepthFromBounds(const ScenePoints &scenePoints, CVector3 pcLowerBound, CVector3 pcUpperBound,
								  double &minDepth, double &maxDepth);

public:  //Camera-Planes.cpp
	void WriteScenePlanesFile(string fileName);
	void WriteFrontoPlanesFile(string fileName);
	void readFrontoPlanesFile();
	void readScenePlanesFile(int iFrame);
};



#endif

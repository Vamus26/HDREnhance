#ifndef __RIG_H__
#define __RIG_H__

#include "Definitions.h"
#include "Camera.h"
#include "ScenePoints.h"

typedef struct _CamPair
{
	string cam1ID;
	string cam2ID;
	float dist;
} CamPair;


typedef struct _MV_Correspondance
{
	vector<string> camIDs;
	vector<CVector2> points;
} MV_Correspondance;


typedef struct _Track
{
	int planeID;
	vector<string> camIDs;
	vector<CVector2> points;
} Track;

typedef vector<MV_Correspondance> MV_Correspondances;

class Rig
{
public:
	typedef struct _RigParams
	{
		vector<string> cameraNames;
		vector<CamPair> camNeighPairs;
		FrameSeq frameSeq;
		bool halfSizeCamMats;
		string outDir;
		bool useGeoCamDists;
		int targetCamIndex;

		vector<MV_Correspondances> userDefinedPlanes;
		vector<Track> userDefinedTracks;

		static _RigParams Default();

		void Dump(const string prefix);
	} RigParams;

public:
	int frameCount;
	vector<Camera> cameras;
	RigParams params;
	bool *videoFrameMap;

public: //Rig.cpp
	Rig();
	~Rig();
	void Load(RigParams rigParams);	
	void RecenterCameras(string centerCamID);
	void RecenterCameras(const CameraMats centerCamMats);
	Camera* GetCamera(string camName, int &camIndex);
	Camera* GetCamera(string camName);
	void OutputFrame(int iFrame);
	void OutputFMatrices();
	void PrintCamNeighsByCalib(int maxNeighs);
	void PrintCamNeighsByPointCloud(int maxNeighs);

	bool IsVideoFrame(int camIndex);
	bool IsVideoFrame(string camID);

private: //Rig.cpp
	void loadCameras();
	void setupCamNeighs();
	void normalizeCamNeighDists();
	void halfSizeCamMats();

public: //Rig-Checks.cpp
	void CheckForConsistentFCs(); //Consistent frame counts in all cams
	void CheckAllCamMatsLoaded();
	void CheckAllCamNeighs();

public: //Rig-FileNames.cpp
	string GetRigDirName();	
	string GetScenePtsFN();

private: //Rig-FileNames.cpp
	string getOutFrameName(int iFrame, int iCam);
	string getFMatrixFN(string cam1ID, string cam2ID);


public: //Rig-Planes.cpp
	void LoadPlanes(int iFrame);
	void CreatePlanes(int iFrame,
					  const vector<Plane> &globalScenePlanes, 
					  const ScenePoints &scenePoints,
					  CVector3 pcLowerBound, CVector3 pcUpperBound,
					  double pixelDistBetweenFrontoPlanes);


public: //Rig-CameraPath.cpp
	void GenSmoothCameraPath(int winSize, int winStdDev, int iter, int interFrameCount);
	void smoothCameraMats(vector<CameraMats> &cameraMats, int winSize, int winStdDev, int iter);
	void visCameraMats(vector<CameraMats> &cameraMats, string fileName);
	void visCameraMatPairs(vector<CameraMats> &cameraMats1, vector<CameraMats> &cameraMats2, string fileName);

public: //Rig-Debug.cpp
	void VisScenePlanes();
	double GetMedianNeighVideoFrameDist();
	void VisTracks();	
	CFloatImage GetFramePointVis(int camID, int iFrame, CVector2 pointLoc);	

	bool IsCamCoveredByTrack(int camID, const Track &track);
	CVector2 GetTrackPoint(int camID, const Track &track);
};

#endif

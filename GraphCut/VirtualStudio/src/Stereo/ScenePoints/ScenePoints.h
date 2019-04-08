#ifndef __SCENE_POINTS_H__
#define __SCENE_POINTS_H__

#include "Definitions.h"
#include "Transform-3x3.h"
#include <fstream>
#include "Camera.h"
#include "PointCloud.h"

class Camera;

class ScenePoint
{
public:
	CVector3 pos;
	CVector3 col;
	vector<IntPair> viewFeaList; //<camID, siftID>

	void Write(ofstream &outStream);
	void Read(ifstream &inStream, bool drewMode);
};

class ScenePoints : public vector<ScenePoint>
{
public:

	ScenePoints Prune(double maxDistFromMedian) const;
	
	ScenePoints Prune(int minViewCount) const;

	ScenePoints Prune(CVector3 col, float maxDist) const;

	ScenePoints Prune(CVector3 pcLowerBound, CVector3 pcUpperBound) const;

	void Write(const char *fileName);
	void Write(string fileName)
	{
		Write(fileName.c_str());
	}

	void WritePly(string plyFN,
				  vector<CVector3> cameraLocs = vector<CVector3>(),
				  bool drawCameraLocs = false,
				  CVector3 pcLowerBound = CVector3(0.0, 0.0, 0.0),
				  CVector3 pcUpperBound = CVector3(0.0, 0.0, 0.0),
				  bool drawBounds = false,
				  const vector<double> &frontoPlanes = vector<double>(),
				  bool drawFrontoPlanes = false);

	void Read(const char *fileName);
	
	void Read(string fileName)
	{
		Read(fileName.c_str());
	}	
	
	void PurgeViewList();

	void GenerateViewPoints(vector<Camera> &cameras);

	void GeneratePointCloud(PointCloud &pointCloud) const;

	void TransformCoordSys(const CameraMats &cameraMats);

	ScenePoints GetVisibleScenePoints(Camera* const camera) const;

	void GetBounds(CVector3 &pcLowerBound, CVector3 &pcUpperBound);
};

#endif

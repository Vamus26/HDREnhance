#ifndef __CAMERA_MATS_H__
#define __CAMERA_MATS_H__

#include "Definitions.h"
#include "Transform-2x2.h"
#include "Transform-3x3.h"
#include "Transform-4x4.h"
#include "Image.h"
#include "Plane.h"
#include <fstream>

class CameraMats
{
public:
	CTransform3x3 intrinsicMat;
	CTransform3x3 rotMat;
	CVector3      transVec;	
	double k1, k2;	
	bool cameraMatsLoaded;

private:
	CTransform3x3 intrinsicMatInv;
	CTransform3x3 rotMatInv;
	CTransform4x4 projMat;
	CTransform4x4 projMatInv;
	CVector3      cameraCoord;
	bool interMatsComputed;

public:
	CameraMats();

	void ComputeInterMats();
	CTransform4x4 GetExtrinsicMat() const;
	CTransform4x4 GetProjMat() const;
	CTransform4x4 GetProjMatInv() const;
	CTransform3x3 GetRotMatInv() const;
	CTransform3x3 GetIntrinsicMatInv() const;
	CVector3      GetWorldCoordCoP() const;

	void Recenter(const CameraMats refCamMats);

	void InvertAxisY();

	void ScaleMats(float scale);
	void CropMats(double x1, double y1, double width, double height);

	CameraMats operator+(const CameraMats &cm) const;
	CameraMats operator/(double div) const;

public: //CameraMats-proj.cpp
	void projXYZtoUV(const CVector3 &xyzCoord, CVector2 &uvCoord) const;
	void projUVZtoXYZ(const CVector3 &uvzCoord, CVector3 &xyzCoord) const;
	CVector3 getRayDirc(const CVector2 &uvCoord) const;
	void projUVtoXYZ(const CVector2 &uvCoord, const Plane &plane, CVector3 &xyzCoord) const;
	CVector3 transformCoordSys(const CVector3 &xyzCoord) const;	
	CVector3 invertCoordSys(CVector3 &xyzCoord) const;	
	CTransform3x3 GenerateFMatrix(CameraMats neighCamMats, bool centerOrigin) const;
	static CTransform3x3 CenterFMatrixCoordSys(CTransform3x3 fmatrix, CShape leftShape, CShape rightShape);

public: //CameraMats-IO.cpp
	void ReadCamera(string filename);
	void ReadCamera(ifstream &inStream);
	void ReadCameraLarry(ifstream &inStream);
	void ReadCameraNoahDrew(ifstream &inStream, int imgWidth, int imgHeight, bool drewMode);	
	void ReadExtrinsicParams(ifstream &inStream);
	void ReadIntrinsicParamsZhang(ifstream &inStream);
	void ReadDistortion(ifstream &inStream);

	void WriteCamera(ofstream &outStream);
	void WriteCamera(string fileName);

	void Dump() const;

public: // CameraMats-Checks.cpp
	void CheckRotMat();
};


#endif

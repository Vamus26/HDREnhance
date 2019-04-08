#include "Camera.h"
#include "Disk.h"
#include "ImageIO.h"


void Camera::init()
{
	this->frontoPlaneCount = this->scenePlaneCount = this->planeCount = 0;
	this->minDepth = DBL_MAX; this->maxDepth = -DBL_MAX;
}

Camera::Camera() 
{
	init();
}

Camera::Camera(string cameraDir)
{
	init();
	Load(cameraDir);
}

void Camera::Load(string cameraDir)
{
	this->path = cameraDir;
	this->id = Disk::GetBaseDirName(this->path);

	CheckForBasicData();

	string framesPattern = getFramesPattern();
	this->frameCount = Disk::NumOfMatchesInPattern(framesPattern);

	string cameraMatsFN = GetCameraMatsFN();
	if(Disk::DoesFileExist(cameraMatsFN))
	{
	  this->cameraMats.ReadCamera(cameraMatsFN);
	}
}

void Camera::Recenter(const CameraMats refCamMats)
{
	this->cameraMats.Recenter(refCamMats);	
	RecenterViewPts(refCamMats);
}


void Camera::PrintCamNeighs()
{
	printf("Cam-%s neighs:\n", this->id.c_str());
	for(uint iNeigh = 0; iNeigh < this->cameraNeighs.size(); iNeigh++)
	{
		printf("\tcam-%s at dist - %f\n",
				this->cameraNeighs[iNeigh].id.c_str(), 
				this->cameraNeighs[iNeigh].dist);
	}
}

void Camera::WriteViewPts()
{
	string fileName = GetCameraViewPtsFN();
	ofstream outStream(fileName.c_str());
	ENSURE(outStream.is_open());

	outStream << this->viewPoints.size() << "\n";
	for(uint iPoint = 0; iPoint < this->viewPoints.size(); iPoint++)
	{
		this->viewPoints[iPoint].Write(outStream);
	}
	
	outStream.close();
}

void Camera::ReadViewPts()
{
	ENSURE(this->viewPoints.size() == 0);

	string fileName = GetCameraViewPtsFN();
	if(Disk::DoesFileExist(fileName))
	{
		ifstream inStream(fileName.c_str());
		ENSURE(inStream.is_open());

		int pointCount = -1;
		inStream >> pointCount;
		ENSURE(pointCount != -1);

		for(int iPoint = 0; iPoint < pointCount; iPoint++)
		{
			CVector3 viewPoint;
			viewPoint.Read(inStream);
			this->viewPoints.push_back(viewPoint);
		}

		inStream.close();
	}
}

void Camera::RecenterViewPts(const CameraMats refCamMats)
{
	for(uint iPoint = 0; iPoint < this->viewPoints.size(); iPoint++)
	{
		this->viewPoints[iPoint] = refCamMats.transformCoordSys(this->viewPoints[iPoint]);	
	}
}


void Camera::ScaleView(float scale)
{
	CHECK_RANGE(scale, 0.1f, 5.0f);
	ENSURE(scale != 1.0f);
	ENSURE(this->frameCount > 0);

	for(int iFrame = 0; iFrame < this->frameCount; iFrame++)
	{
		string frameName = GetFrameName(iFrame);
		string frameSmoothName = GetFrameSmoothName(iFrame);
		ENSURE(Disk::DoesFileExist(frameSmoothName) == false);

		CFloatImage frameImg;
		ImageIO::ReadFile(frameImg, frameName, scale);
		ImageIO::WriteFile(frameImg, frameName);	
	}

	this->cameraMats.ScaleMats(scale);
	string camMatsName = GetCameraMatsFN();
	this->cameraMats.WriteCamera(camMatsName);
}

void Camera::BackUpBP_Data(int iFrame) const
{
	string segBakDirName = GetSegBakDirName();
	Disk::MakeDir(segBakDirName);

	string beliefsFN = GetFramePrevBeliefsName(iFrame);
	string bakBeliefsFN = GetFrameBakPrevBeliefsName(iFrame);

	string msgesFN      = GetFramePrevMsgesName(iFrame);
	string bakMsgesFN   = GetFrameBakPrevMsgesName(iFrame);	

	Disk::CopyFile(beliefsFN, bakBeliefsFN);
	Disk::CopyFile(msgesFN, bakMsgesFN);
}
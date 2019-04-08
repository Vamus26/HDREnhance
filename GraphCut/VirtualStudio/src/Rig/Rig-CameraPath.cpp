#include "Rig.h"
#include "matrix.h"
#include "defines.h"
#include "Disk.h"
#include "ImageIO.h"

void Rig::GenSmoothCameraPath(int winSize, int winStdDev, int iters, int interFrameCount)
{	
	INSIST(winSize > 0);

	vector<CameraMats> vidCameraMats;
	vidCameraMats.reserve(this->cameras.size());
	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		if(IsVideoFrame(iCam))
		{
			vidCameraMats.push_back(this->cameras[iCam].cameraMats);
		}
	}

	vector<CameraMats> vidCameraMatsSmooth = vidCameraMats;	
	smoothCameraMats(vidCameraMatsSmooth, winSize, winStdDev, iters);

	visCameraMats(vidCameraMats, "cameraPath-org.ply");
	visCameraMats(vidCameraMatsSmooth, "cameraPath-smooth.ply");
	visCameraMatPairs(vidCameraMats, vidCameraMatsSmooth, "cameraPaths.ply");

	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		char camID_Buf[100];		
		sprintf(camID_Buf, "vid-1%03i", iCam);
		string camID = camID_Buf;

		string camDirName = GetRigDirName() + camID + OS_PATH_DELIMIT_STR;
		//INSIST(Disk::DoesDirExist(camDirName) == false);
		Disk::MakeDir(camDirName);

		Camera newCam(camDirName);
		string newFrameFN = newCam.GetFrameName(0);
		string oldFrameFN = this->cameras[iCam].GetFrameName(0);
		CFloatImage frameImage;
		ImageIO::ReadFile(frameImage, oldFrameFN); 
		ImageIO::WriteFile(frameImage, newFrameFN);
		
		string scenePlanesFN = newCam.GetScenePlanesFN(0);
		this->cameras[iCam].readScenePlanesFile(0);
		this->cameras[iCam].WriteScenePlanesFile(scenePlanesFN);

		string frontoPlanesFN = newCam.GetFrontoPlanesFN();
		this->cameras[iCam].readFrontoPlanesFile();
		this->cameras[iCam].WriteFrontoPlanesFile(frontoPlanesFN);

		newCam.cameraMats = vidCameraMatsSmooth[iCam];
		newCam.cameraMats.WriteCamera(newCam.GetCameraMatsFN());
	}
}

void Rig::smoothCameraMats(vector<CameraMats> &cameraMats, int winSize, int winStdDev, int iters)
{
	INSIST((int) cameraMats.size() > winSize);
	vector<CameraMats> cameraMatsCopy = cameraMats;
	INSIST(iters > 0);
	INSIST(winSize > 0);
	INSIST(winStdDev > 0);

	for(int iIter = 0; iIter < iters; iIter++)
	{
		for(int iCam = 1; iCam < (int) cameraMats.size() - 1; iCam++)
		{
			int minCamIndex = iCam - (winSize / 2);
			SET_IF_GT(minCamIndex, 0);

			int maxCamIndex = iCam + (winSize / 2);
			SET_IF_LT(maxCamIndex, (int) cameraMats.size() - 1);

			CTransform3x3 intrinsicMat = CTransform3x3::Null();
			double quat[4] = {0.0, 0.0, 0.0, 0.0};
			CVector3 worldCoord(0.0, 0.0, 0.0);
			float neighWeighSum = 0.0f;
			for(int iNeigh = minCamIndex; iNeigh <= maxCamIndex; iNeigh++)
			{
				float neighWeight = fabs((float) iCam - iNeigh);
				neighWeight *= neighWeight;
				neighWeight = exp(neighWeight/-(2.0f * winStdDev * winStdDev));

				intrinsicMat = intrinsicMat + (cameraMats[iNeigh].intrinsicMat * neighWeight);

				double currRotMat[9];			
				currRotMat[0] = cameraMats[iNeigh].rotMat[0][0];
				currRotMat[1] = cameraMats[iNeigh].rotMat[0][1];
				currRotMat[2] = cameraMats[iNeigh].rotMat[0][2];
				currRotMat[3] = cameraMats[iNeigh].rotMat[1][0];
				currRotMat[4] = cameraMats[iNeigh].rotMat[1][1];
				currRotMat[5] = cameraMats[iNeigh].rotMat[1][2];
				currRotMat[6] = cameraMats[iNeigh].rotMat[2][0];
				currRotMat[7] = cameraMats[iNeigh].rotMat[2][1];
				currRotMat[8] = cameraMats[iNeigh].rotMat[2][2];

				double currQuat[4];
				matrix_to_quaternion(currRotMat, currQuat);

				quat[0] += (currQuat[0] * neighWeight);
				quat[1] += (currQuat[1] * neighWeight);
				quat[2] += (currQuat[2] * neighWeight);
				quat[3] += (currQuat[3] * neighWeight);

				worldCoord = worldCoord + (cameraMats[iNeigh].GetWorldCoordCoP() * neighWeight);

				neighWeighSum += neighWeight;
			}
			INSIST(neighWeighSum > 0.0f);

			cameraMatsCopy[iCam].intrinsicMat = intrinsicMat * (1.0 / neighWeighSum);

			worldCoord = worldCoord / neighWeighSum;

			quat[0] /= neighWeighSum;
			quat[1] /= neighWeighSum;
			quat[2] /= neighWeighSum;
			quat[3] /= neighWeighSum;
			double qualNormLen = sqrt(SQ(quat[0]) + SQ(quat[1]) + SQ(quat[2]) + SQ(quat[3]));
			quat[0] /= qualNormLen;
			quat[1] /= qualNormLen;
			quat[2] /= qualNormLen;
			quat[3] /= qualNormLen;

			double rotMat[9];
			quaternion_to_matrix(quat, rotMat);
			
			cameraMatsCopy[iCam].rotMat[0][0] = rotMat[0];
			cameraMatsCopy[iCam].rotMat[0][1] = rotMat[1];
			cameraMatsCopy[iCam].rotMat[0][2] = rotMat[2];
			cameraMatsCopy[iCam].rotMat[1][0] = rotMat[3];
			cameraMatsCopy[iCam].rotMat[1][1] = rotMat[4];
			cameraMatsCopy[iCam].rotMat[1][2] = rotMat[5];
			cameraMatsCopy[iCam].rotMat[2][0] = rotMat[6];
			cameraMatsCopy[iCam].rotMat[2][1] = rotMat[7];
			cameraMatsCopy[iCam].rotMat[2][2] = rotMat[8];

			cameraMatsCopy[iCam].transVec = cameraMatsCopy[iCam].rotMat * (worldCoord * -1.0);

			cameraMatsCopy[iCam].ComputeInterMats();
		}

		cameraMats = cameraMatsCopy;
	}
}

void Rig::visCameraMats(vector<CameraMats> &cameraMats, string plyFN)
{
	ofstream outStream(plyFN.c_str());
	ENSURE(outStream.is_open());

	int numOfPoints = (int) cameraMats.size() * 2;

	outStream << "ply\n";
	outStream << "format ascii 1.0\n";
	outStream << "element vertex " << numOfPoints << "\n";
	outStream << "property float x\n";
	outStream << "property float y\n";
	outStream << "property float z\n";
	outStream << "property uchar diffuse_red\n";
	outStream << "property uchar diffuse_green\n";
	outStream << "property uchar diffuse_blue\n";
	outStream << "end_header\n";
	
	//bug
	CVector3 imgCoord(426.5, 240.0, 1.0);
	for(uint iCam = 0; iCam < cameraMats.size(); iCam++)
	{
		int red   = 0;
		int green = 255;
		int blue  = 0;

		CVector3 worldCoord = cameraMats[iCam].GetWorldCoordCoP();
		outStream << worldCoord[0] << " " <<
					 worldCoord[1] << " " <<
					 -worldCoord[2] << " " <<    
					 red << " " << green << " " << blue << "\n";		
		CVector3 rayDirc = cameraMats[iCam].GetRotMatInv() * (cameraMats[iCam].GetIntrinsicMatInv() * imgCoord);
		CVector3 dircPt = worldCoord + (rayDirc * 0.01);
		red   = 0;
		green = 0;
		blue  = 255;
		outStream <<  dircPt[0] << " " <<
					  dircPt[1] << " " <<
					  -dircPt[2] << " " <<    
					  red << " " << green << " " << blue << "\n";
	}

	outStream.close();
}

void Rig::visCameraMatPairs(vector<CameraMats> &cameraMats1, vector<CameraMats> &cameraMats2, string plyFN)
{
	INSIST(cameraMats1.size() == cameraMats2.size());

	ofstream outStream(plyFN.c_str());
	ENSURE(outStream.is_open());

	int numOfPoints = (int) cameraMats1.size() * 4;

	outStream << "ply\n";
	outStream << "format ascii 1.0\n";
	outStream << "element vertex " << numOfPoints << "\n";
	outStream << "property float x\n";
	outStream << "property float y\n";
	outStream << "property float z\n";
	outStream << "property uchar diffuse_red\n";
	outStream << "property uchar diffuse_green\n";
	outStream << "property uchar diffuse_blue\n";
	outStream << "end_header\n";

	//bug
	CVector3 imgCoord(426.5, 240.0, 1.0);
	for(uint iCam = 0; iCam < cameraMats1.size(); iCam++)
	{
		int red   = 255;
		int green = 0;
		int blue  = 0;
		CVector3 worldCoord = cameraMats1[iCam].GetWorldCoordCoP();
		outStream << worldCoord[0] << " " <<
					 worldCoord[1] << " " <<
					 -worldCoord[2] << " " <<    
					 red << " " << green << " " << blue << "\n";
		CVector3 rayDirc = cameraMats1[iCam].GetRotMatInv() * (cameraMats1[iCam].GetIntrinsicMatInv() * imgCoord);
		CVector3 dircPt = worldCoord + (rayDirc * 0.01);
		red   = 0;
		green = 255;
		outStream <<  dircPt[0] << " " <<
					  dircPt[1] << " " <<
					  -dircPt[2] << " " <<    
					  red << " " << green << " " << blue << "\n";

		red   = 0;
		green = 0;
		blue  = 255;
		worldCoord = cameraMats2[iCam].GetWorldCoordCoP();		
		outStream << worldCoord[0] << " " <<
					 worldCoord[1] << " " <<
					 -worldCoord[2] << " " <<    
					 red << " " << green << " " << blue << "\n";
		rayDirc    = cameraMats2[iCam].GetRotMatInv() * (cameraMats2[iCam].GetIntrinsicMatInv() * imgCoord);
		dircPt = worldCoord + (rayDirc * 0.01);
		red   = 255;
		green = 0;
		blue  = 255;
		outStream <<  dircPt[0] << " " <<
					  dircPt[1] << " " <<
					  -dircPt[2] << " " <<    
					  red << " " << green << " " << blue << "\n";
	}

	outStream.close();
}
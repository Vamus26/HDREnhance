#include "SS_Setup.h"
#include <fstream>
#include "Disk.h"
#include "Camera.h"
#include "ImageIO.h"
#include "Rig.h"
#include "RigCalibrator.h"

void SS_Setup::Start(SS_SetupParams ssSetupParams, RigCalibrator::RigCalibratorParams rcParams)
{
	this->rigCalibParams = rcParams;
	this->params = ssSetupParams;
	loadSceneFNs();	
	processCams();
	calibrateScene();
	writeOptFile();
}


void SS_Setup::loadSceneFNs()
{
	ENSURE(this->sceneFrameFNs.size() == 0);

	ifstream inStream(this->params.listFN.c_str());
	ENSURE(inStream.is_open());
	string lineBuff;
	while(std::getline(inStream, lineBuff))
	{
		vector<string> lineTokens;	
		Tokenize(lineBuff, lineTokens, " \t");
		ENSURE(lineTokens.size() > 0);

		string sceneFrameFN = Disk::GetFileNameID(lineTokens[0]) + CAMERA_ImageExt;
		this->sceneFrameFNs.push_back(sceneFrameFN);
		
		string camID = Disk::GetFileNameID(Disk::GetBaseFileName(lineTokens[0]));
		this->camIDs.push_back(camID);
		
		if(lineTokens.size() > 1)
		{
			float scale = (float) atof(lineTokens[1].c_str());
			CHECK_RANGE(scale, 0.1f, 5.0f);
			this->sceneFrameScales.push_back(scale);
		}
		else
		{
			this->sceneFrameScales.push_back(1.0f);
		}
	}
	inStream.close();

	ENSURE(this->sceneFrameFNs.size() == this->sceneFrameScales.size());

	ENSURE(this->sceneFrameFNs.size() > 0);
}

void SS_Setup::processCams()
{
	ENSURE(this->sceneFrameFNs.size() > 0);

	printf("Processing cameras....\n");

	for(uint iCam = 0; iCam < this->sceneFrameFNs.size(); iCam++)
	{
		string sceneFN = this->sceneFrameFNs[iCam];
		string camID = this->camIDs[iCam];
		string camDirName = this->params.targetDir + camID + OS_PATH_DELIMIT_STR;
		Disk::MakeDir(camDirName);

		this->camDirNames.push_back(camDirName);

		CFloatImage sceneFrame;
		ImageIO::ReadFile(sceneFrame, sceneFN);

		Camera currCam(camDirName);
		string camFrameFN = currCam.GetFrameName(0);
		//ENSURE(CAMERA_ImageExt.compare(".tif") == 0);
		//ImageIO::WriteFileTiff(sceneFrame, camFrameFN, CAMERA_ImageCompression); 
		ImageIO::WriteFile(sceneFrame, camFrameFN); 

		printf("\t %s\n", camID.c_str());
	}

	printf("\tDone.\n");
}

void SS_Setup::calibrateScene()
{
	ENSURE(this->camDirNames.size() > 0);

	printf("Calibrating scene....\n");

	Rig::RigParams rigParams = Rig::RigParams::Default();
	rigParams.cameraNames = this->camDirNames;
	for(uint iCam = 1; iCam < this->sceneFrameFNs.size(); iCam++)
	{

		string sceneFN = this->sceneFrameFNs[iCam];
		string camID   = this->camIDs[iCam];

		string prevSceneFN = this->sceneFrameFNs[iCam - 1];
		string prevCamID   = this->camIDs[iCam - 1];

		CamPair camPair;
		camPair.cam1ID = camID;
		camPair.cam2ID = prevCamID;
		camPair.dist = 1.0f;

		rigParams.camNeighPairs.push_back(camPair);
	}

	this->rig.Load(rigParams);
	
	if(this->params.drewMode)
		this->rigCalibParams.useDrewCalib = true;		
	else
		this->rigCalibParams.useNoahCalib = true;
	this->rigCalibParams.calibFN = this->params.targetDir + this->params.noahBundleFN;

	RigCalibrator rigCalibrator(&this->rig);
	rigCalibrator.Calibrate(this->rigCalibParams);

	//Gets rid of cameras that didn't load (i.e. zero focal length)
	this->camDirNames.clear();
	this->camIDs.clear();
	vector<string> sceneFrameFNsBak = this->sceneFrameFNs;
	this->sceneFrameFNs.clear();
	for(uint iCam = 0; iCam < this->rig.cameras.size(); iCam++)
	{
		if(this->rig.cameras[iCam].cameraMats.cameraMatsLoaded == true)
		{
			this->camIDs.push_back(this->rig.cameras[iCam].id);
			this->camDirNames.push_back(this->rig.cameras[iCam].path);
			this->sceneFrameFNs.push_back(sceneFrameFNsBak[iCam]);		

			float viewScale = this->sceneFrameScales[iCam];
			if(viewScale != 1.0f)
			{
				printf("Scaling - %s by %02.02fx\n", this->rig.cameras[iCam].id.c_str(), viewScale);
				this->rig.cameras[iCam].ScaleView(viewScale);
			}
		}
	}

	printf("\tDone.\n");
}

void SS_Setup::writeOptFile()
{
	ENSURE(this->camDirNames.size() > 0);
	ENSURE(this->params.neighSpacing > 0);

	ofstream outStream(this->params.optFN.c_str());
	ENSURE(outStream.is_open());

	int photoTrailPrefixCount = 0;
	for(uint iCam = 0; iCam < this->camDirNames.size(); iCam++)
	{
		outStream << "Rig.camera=" << this->camDirNames[iCam] << "\n";

		string camID = this->camIDs[iCam];
		bool isCurrCamVideoFrame = this->rig.IsVideoFrame(camID);
		if(isCurrCamVideoFrame == false)
		{
			photoTrailPrefixCount++;
		}
	}

	outStream << "\n";

	ENSURE(this->camDirNames.size() == this->camIDs.size());
	for(int iCam = 0; iCam < (int) this->camDirNames.size(); iCam++)
	{
		string camID   = this->camIDs[iCam];
		bool isCurrCamVideoFrame = this->rig.IsVideoFrame(camID);

		if(iCam > 0)
		{
			string prevCamID = this->camIDs[iCam - 1];
			bool isPrevCamVideoFrame = this->rig.IsVideoFrame(prevCamID);
			if(isCurrCamVideoFrame == isPrevCamVideoFrame)
			{
				outStream << "Rig.camNeighPair " << prevCamID << " " << camID << " 1.0f\n";
			}
		}
	}

	outStream << "\n";

	for(int iCam = 0; iCam < (int) this->camDirNames.size(); iCam++)
	{
		string camID   = this->camIDs[iCam];
		bool isCurrCamVideoFrame = this->rig.IsVideoFrame(camID);

		if(isCurrCamVideoFrame)
		{
			if(iCam >= this->params.neighSpacing)
			{
				string neighCamID = this->camIDs[iCam - this->params.neighSpacing];
				bool isNeighCamVideoFrame = this->rig.IsVideoFrame(neighCamID);
				if(isNeighCamVideoFrame)
				{
					if(((iCam - photoTrailPrefixCount) / this->params.neighSpacing) % 2 == 1)
					{
						outStream << "Rig.camNeighPair " << neighCamID << " " << camID << " 1.0f\n";
					}
				}
			}
		}
	}

	outStream.close();
}



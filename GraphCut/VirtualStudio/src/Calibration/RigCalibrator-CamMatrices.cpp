#include "RigCalibrator.h"
#include <fstream>

void RigCalibrator::genCamMats()
{
	printf("\n\tGenerating camera matrices...\n");

	for(uint iCam = 0; iCam < this->rig->cameras.size(); iCam++)
	{
		printf("\t\tGenerating matrices for - %s\n", this->rig->cameras[iCam].id.c_str());

		string outCmd = getCalibCmdPrefix(iCam);
		for(int iFrame = 0; iFrame < this->params.framesToFind; iFrame++)
		{
			outCmd += " " + getModelFileName(iCam, iFrame); 
		}
		for(int iFrame = 0; iFrame < this->params.framesToFind; iFrame++)
		{
			outCmd += " " + getPointsFileName(iCam, iFrame); 
		}

		ENSURE(SilentSysCall(outCmd.c_str()) == true);
	}
}

void RigCalibrator::loadCamMats()
{
	printf("\n\tLoading camera matrices...\n");

	this->frameCameraMats.resize(this->rig->cameras.size());
	for(uint iCam = 0; iCam < this->rig->cameras.size(); iCam++)
	{
		printf("\t\tLoading matrices for - %s\n", this->rig->cameras[iCam].id.c_str());

		string camMatsFN = getCalibResultFN(iCam);
		ifstream inStream(camMatsFN.c_str());
		ENSURE(inStream.is_open());

		CameraMats firstFrameCameraMats;
		firstFrameCameraMats.ReadIntrinsicParamsZhang(inStream);
		firstFrameCameraMats.ReadExtrinsicParams(inStream);
		this->frameCameraMats[iCam].push_back(firstFrameCameraMats);

		for(int iFrame = 1; iFrame < this->params.framesToFind; iFrame++)
		{
			CameraMats currFrameCameraMats = firstFrameCameraMats;
			currFrameCameraMats.ReadExtrinsicParams(inStream);
			this->frameCameraMats[iCam].push_back(currFrameCameraMats);
		}

		inStream.close();
	}
}


void RigCalibrator::recalMatsUsingRefCam()
{
	int iRefCamIndex = -1;
	for(uint iCam = 0; iCam < this->rig->cameras.size(); iCam++)
	{
		if(this->params.refCamID.compare(this->rig->cameras[iCam].id) == 0)
		{
			iRefCamIndex = (int) iCam;
			break;
		}
	}
	ENSURE(iRefCamIndex != -1);
	

	for(int iFrame = 0; iFrame < this->params.framesToFind; iFrame++)
	{
		CameraMats refCamMats = this->frameCameraMats[iRefCamIndex][iFrame];
		for(uint iCam = 0; iCam < this->rig->cameras.size(); iCam++)
		{
			CameraMats currCamMats   = this->frameCameraMats[iCam][iFrame];

			CTransform3x3 rotProduct = currCamMats.rotMat * refCamMats.rotMat;
			currCamMats.rotMat       = rotProduct.Transpose();
			currCamMats.transVec     = currCamMats.transVec - 
				                       (rotProduct.Transpose() * refCamMats.transVec);

			this->frameCameraMats[iCam][iFrame] = currCamMats;
		}
	}
}

void RigCalibrator::writeFinalCamMats()
{
	printf("\n\tWriting camera matrices...\n");

	for(uint iCam = 0; iCam < this->rig->cameras.size(); iCam++)
	{
		Camera &currCam = this->rig->cameras[iCam];
		printf("\t\tWriting matrices for - %s\n", currCam.id.c_str());

		ENSURE(this->params.framesToFind > 0);
		CameraMats avgCameraMats = this->frameCameraMats[iCam][0];
		for(int iFrame = 1; iFrame < this->params.framesToFind; iFrame++)
		{
			avgCameraMats = avgCameraMats + this->frameCameraMats[iCam][iFrame];
		}
		avgCameraMats = (avgCameraMats / ((double) this->params.framesToFind));

		currCam.cameraMats = avgCameraMats;
		string camMatsFN = currCam.GetCameraMatsFN();
		avgCameraMats.WriteCamera(camMatsFN);		
	}
}

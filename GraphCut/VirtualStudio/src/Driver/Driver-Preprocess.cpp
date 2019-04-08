#include "Driver.h"
//#include "StreamPixIO.h"
#include "RigCalibrator.h"
#include "SS_Setup.h"

void Driver::calibrateRigCams()
{
	Rig rig;
	if(this->params.rigParams.halfSizeCamMats == true)
	{
		REPORT("Warning. Turning off halfSizeCamMats rig option.\n");
		this->params.rigParams.halfSizeCamMats = false;
	}
	rig.Load(this->params.rigParams);

	RigCalibrator rigCalibrator(&rig);
	rigCalibrator.Calibrate(this->params.rigCalibratorParams);
}


void Driver::extractSeqData()
{
	/*
	Rig rig;
	rig.Load(this->params.rigParams);

	int minFramesInSeqFile = INT_MAX;
	for(uint iCam = 0; iCam < rig.cameras.size(); iCam++)
	{
		int framesInCamSeq = StreamPixIO::GetFramesInSeqFile(rig.cameras[iCam]);
		SET_IF_LT(minFramesInSeqFile, framesInCamSeq);
	}
	ENSURE(minFramesInSeqFile != INT_MAX);

	for(uint iCam = 0; iCam < rig.cameras.size(); iCam++)
	{
		int framesInCamSeq = StreamPixIO::GetFramesInSeqFile(rig.cameras[iCam]);
		ENSURE(framesInCamSeq > 0);
		ENSURE(framesInCamSeq >= minFramesInSeqFile);
		int seqStartOffset = framesInCamSeq - minFramesInSeqFile;

		FrameSeq frameSeqToExtract = rig.params.frameSeq;
		if(frameSeqToExtract.startIndex == -1)
		{
			frameSeqToExtract.startIndex = seqStartOffset;
			frameSeqToExtract.endIndex   = framesInCamSeq - 1;
		}
		else
		{
			frameSeqToExtract.startIndex += seqStartOffset;
			frameSeqToExtract.endIndex   += seqStartOffset;
		}

		StreamPixIO::ExtractSeqData(rig.cameras[iCam],
									frameSeqToExtract,
									CAMERA_ImageCompression);
	}
	*/
}

void Driver::setupStaticScene()
{
	SS_Setup ssSetup;
	ssSetup.Start(this->params.ssSetupParams, this->params.rigCalibratorParams);
}



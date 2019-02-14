#include "RigCalibrator.h"
#include "Disk.h"
#include "ImageIO.h"

RigCalibrator::RigCalibrator(Rig *rigToCalibrate) : rig(rigToCalibrate)
{
	ENSURE(this->rig->frameCount > 0);
	ENSURE(this->rig->cameras.size() > 0);
	this->calibDirName = RigCalibrator::getCalibDirName();	
}

void RigCalibrator::Calibrate(RigCalibratorParams rigCalibParams)
{	
	this->params = rigCalibParams;

	if((rigCalibParams.useNoahCalib) || (rigCalibParams.useDrewCalib))
	{
		ENSURE(rigCalibParams.useNoahCalib != rigCalibParams.useDrewCalib);
		ENSURE(!rigCalibParams.useLarryCalib);
		loadNoahDrewCalibFile(rigCalibParams.minViewsForScenePoint, rigCalibParams.useDrewCalib);
		return;
	}
	else if(rigCalibParams.useLarryCalib)
	{
		loadLarryCalibFile();
		return;
	}

	printf("Calibrating cameras...\n");
	ENSURE(this->params.refCamID.size() > 0);
	Disk::MakeDir(this->calibDirName);

	if(this->params.frameSeqToSearch.startIndex == -1)
	{
		this->params.frameSeqToSearch.startIndex = 0;
		this->params.frameSeqToSearch.endIndex   = this->rig->frameCount - 1;
	}
	else
	{
		CHECK_FS_RANGE(this->params.frameSeqToSearch, 0, this->rig->frameCount - 1);
	}

	ENSURE(this->params.framesToFind <= 
		   (this->params.frameSeqToSearch.endIndex - 
		    this->params.frameSeqToSearch.startIndex + 1)); 

	findValidFrames();
	genCamMats();
	loadCamMats();
	recalMatsUsingRefCam();
	writeFinalCamMats();
}




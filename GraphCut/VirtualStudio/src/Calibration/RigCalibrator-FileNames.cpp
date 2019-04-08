#include "RigCalibrator.h"
#include "Disk.h"

const string RIG_CALIBRATOR_CalibDirName   = "Calib-temp" + OS_PATH_DELIMIT_STR;
const string RIG_CALIBRATOR_ExtractPtOptFN = "extractPointsOpts.txt";
const string RIG_CALIBRATOR_ExtractPtOutFN = "extractPointsOut.txt";
const string RIG_CALIBRATOR_CalibImageExt  = ".tga";
const string RIG_CALIBRATOR_ExtractPtBinFN = "C:\\projects\\VirtuCam\\tools\\EasyCalib\\CalibExtractPoints.exe";
const string RIG_CALIBRATOR_CalibBinFN     = "C:\\projects\\VirtuCam\\tools\\EasyCalib\\CalibZhangCmd.exe";

string RigCalibrator::getCalibDirName()
{
	ENSURE(this->rig->cameras.size() > 0);
	string baseDir = Disk::GetBasePathName(rig->cameras[0].path);
	return (baseDir + RIG_CALIBRATOR_CalibDirName);
}

string RigCalibrator::getExtractPtOptFN()
{
	return this->calibDirName + RIG_CALIBRATOR_ExtractPtOptFN;
}

string RigCalibrator::getExtractPtOutFN()
{
	return this->calibDirName + RIG_CALIBRATOR_ExtractPtOutFN;
}

string RigCalibrator::getExtractPtBinFN()
{
	return RIG_CALIBRATOR_ExtractPtBinFN;
}

string RigCalibrator::getCalibResultFN(int iCam)
{
	return (this->calibDirName + 
			this->rig->cameras[iCam].id.c_str() +
			"-CalibParams.txt");
}

string RigCalibrator::getCalibCmdPrefix(int iCam)
{
	return (RIG_CALIBRATOR_CalibBinFN + 
		    " -result " + getCalibResultFN(iCam) + 
			" -input ");
}

string RigCalibrator::getCalibImageName(int camIndex, int validFrameIndex)
{
	static char fileNameBuf[1024];

	sprintf(fileNameBuf, "%s%s-frame%03i%s", 
			this->calibDirName.c_str(), 
			this->rig->cameras[camIndex].id.c_str(),
			validFrameIndex,
			RIG_CALIBRATOR_CalibImageExt.c_str()); 

	string fileName = fileNameBuf;
	return fileName;
}

string RigCalibrator::getPointsFileName(int camIndex, int validFrameIndex)
{
	static char fileNameBuf[1024];

	sprintf(fileNameBuf, "%spoints-%s-frame%03i.txt", 
			this->calibDirName.c_str(), 
			this->rig->cameras[camIndex].id.c_str(),
			validFrameIndex); 

	string fileName = fileNameBuf;
	return fileName;
}

string RigCalibrator::getModelFileName(int camIndex, int validFrameIndex)
{
	static char fileNameBuf[1024];

	sprintf(fileNameBuf, "%smodel-%s-frame%03i.txt", 
			this->calibDirName.c_str(), 
			this->rig->cameras[camIndex].id.c_str(),
			validFrameIndex); 

	string fileName = fileNameBuf;
	return fileName;
}

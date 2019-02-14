#include "Rig.h"
#include "Disk.h"

string Rig::GetRigDirName()
{
	ENSURE(this->cameras.size() > 0);
	return Disk::GetBasePathName(this->cameras[0].path);
}

string Rig::GetScenePtsFN()
{
	return GetRigDirName() + "scenePts.dat";	
}

string Rig::getOutFrameName(int iFrame, int iCam)
{
	static char fileName[1024];
	string fnPrefix = this->params.outDir + this->cameras[iCam].id + "-";

	sprintf(fileName, "%s%05i%s",
			fnPrefix.c_str(),
			iFrame,
			CAMERA_ImageExt.c_str());
	string outFrameFN = fileName;

	return outFrameFN;
}



string Rig::getFMatrixFN(string cam1ID, string cam2ID)
{
	return this->params.outDir + cam1ID + "-" + cam2ID + ".txt";
}
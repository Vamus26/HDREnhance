#include "Stereo.h"
#include <string>

const string STEREO_ScenePlanesFN          = "ScenePlanes.txt";
const string STEREO_StereoDirName          = "Stereo-temp2" + OS_PATH_DELIMIT_STR; //bug

string Stereo::getScenePlanesFN()
{
	return Stereo::GetStereoDirName(rig) + STEREO_ScenePlanesFN;
}


string Stereo::GetStereoDirName(Rig &rig)
{
	string baseDirName = rig.GetRigDirName();
	return baseDirName + STEREO_StereoDirName;
}


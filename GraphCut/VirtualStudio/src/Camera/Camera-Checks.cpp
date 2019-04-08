#include "Camera.h"
#include "Disk.h"

void Camera::CheckForBasicData()
{
	ENSURE(this->path.size() > 0);
	if(Disk::DoesDirExist(this->path) == false)
	{
		REPORT_FAILURE("The following camera dir not found - %s", this->path.c_str()); 
	}

	Disk::MakeDir(GetFramesDirName());
	Disk::MakeDir(GetSegDirName());
	Disk::MakeDir(GetDepsDirName());
	Disk::MakeDir(GetFlowDirName());
}

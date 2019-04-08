#include "Rig.h"

void Rig::CheckForConsistentFCs() //Consistent frame counts in all cams
{
	ENSURE(this->cameras.size() > 0);

	for(uint iCam = 1; iCam < this->cameras.size(); iCam++)
	{
		ENSURE(this->cameras[iCam].frameCount == this->cameras[iCam - 1].frameCount);
	}
}

void Rig::CheckAllCamMatsLoaded()
{
	ENSURE(this->cameras.size() > 0);
	for(uint iCam = 1; iCam < this->cameras.size(); iCam++)
	{
		ENSURE(this->cameras[iCam].cameraMats.cameraMatsLoaded);
	}
}

void Rig::CheckAllCamNeighs()
{
	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		CameraNeighs &camNeighs = this->cameras[iCam].cameraNeighs;
		
		//bug
		//ENSURE(camNeighs.size() > 0);
		for(uint iNeigh = 0; iNeigh < camNeighs.size(); iNeigh++)
		{
			CHECK_RANGE(camNeighs[iNeigh].dist, 0.0f, 1.0f);
			int neighCamIndex = -1;
			const Camera *neighCam = this->GetCamera(camNeighs[iNeigh].id, neighCamIndex);
			ENSURE(neighCam != NULL);
		}
	}
}

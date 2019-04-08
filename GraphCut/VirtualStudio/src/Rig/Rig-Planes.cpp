#include "Rig.h"
#include "Disk.h"
#include "ScenePoints.h"


void Rig::LoadPlanes(int iFrame)
{
	printf("Loading camera planes...\n"); fflush(stdout);
	INSIST(this->cameras.size() > 0);

	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		this->cameras[iCam].LoadPlanes(iFrame);
	}

	int minFrontoPlaneCount   = INT_MAX;
	int maxFrontoPlaneCount   = INT_MIN;
	float avgFrontoPlaneCount = 0.0f;

	string minPlaneCamID;
	string maxPlaneCamID;

	int rigScenePlaneCount = this->cameras[0].scenePlaneCount;
	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		printf("."); fflush(stdout);

		INSIST(this->cameras[iCam].scenePlaneCount == rigScenePlaneCount);

		int currCamFrontoPlaneCount = this->cameras[iCam].frontoPlaneCount;
		avgFrontoPlaneCount += currCamFrontoPlaneCount;

		INSIST(currCamFrontoPlaneCount > 0);
		if(minFrontoPlaneCount > currCamFrontoPlaneCount)
		{
			minFrontoPlaneCount = currCamFrontoPlaneCount;
			minPlaneCamID = this->cameras[iCam].id;
		}

		if(maxFrontoPlaneCount < currCamFrontoPlaneCount)
		{
			maxFrontoPlaneCount = currCamFrontoPlaneCount;
			maxPlaneCamID = this->cameras[iCam].id;
		}
	}
	avgFrontoPlaneCount /= this->cameras.size();

	printf("\n[min, max, avg] fronto plane count - [%i:%s, %i:%s, %f]\n", 
			minFrontoPlaneCount, minPlaneCamID.c_str(),
			maxFrontoPlaneCount, maxPlaneCamID.c_str(),
			avgFrontoPlaneCount);
}

void Rig::CreatePlanes(int iFrame,
					  const vector<Plane> &globalScenePlanes, 
					  const ScenePoints &scenePoints,
					  CVector3 pcLowerBound, CVector3 pcUpperBound,
					  double pixelDistBetweenFrontoPlanes)
{
	INSIST(this->cameras.size() > 0);

	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{		
		const CameraNeighs &currCamNeighs = this->cameras[iCam].cameraNeighs;

		//bug
		if(currCamNeighs.size() == 0) continue;

		INSIST(currCamNeighs.size() > 0);

		vector<Camera *> neighCams;
		for(uint iNeigh = 0; iNeigh < currCamNeighs.size(); iNeigh++)
		{
			string neighCamID = currCamNeighs[iNeigh].id;			
			Camera *neighCam = GetCamera(neighCamID);
			INSIST(neighCam != NULL);
			neighCams.push_back(neighCam);
		}

		printf("Creating fronto parallel planes for %s\n", this->cameras[iCam].id.c_str());

		this->cameras[iCam].CreatePlanes(iFrame, 
										 globalScenePlanes,
										 scenePoints,
										 pcLowerBound, pcUpperBound,
										 pixelDistBetweenFrontoPlanes, 
										 neighCams);
	}

	int rigScenePlaneCount = this->cameras[0].scenePlaneCount;
	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		//bug
		if(this->cameras[iCam].cameraNeighs.size() == 0) continue;

		INSIST(this->cameras[iCam].scenePlaneCount == rigScenePlaneCount);
		INSIST(this->cameras[iCam].frontoPlaneCount > 0);
	}
}

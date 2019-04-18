#include "Rig.h"
#include "ImageIO.h"
#include "Disk.h"
#include <algorithm>

Rig::Rig()
{
	this->frameCount = 0;
	this->videoFrameMap = NULL;
}

Rig::~Rig()
{
	if(this->videoFrameMap != NULL)
	{
		delete [] this->videoFrameMap;
	}
}

void Rig::Load(RigParams rigParams)
{
	this->params = rigParams;
	//this->params.frameSeq.startIndex = 0;
	//this->params.frameSeq.endIndex = 1;
	Disk::MakeDir(this->params.outDir);
	loadCameras();
	if(this->params.frameSeq.startIndex  == -1)
	{
		this->params.frameSeq.startIndex = 0;
		this->params.frameSeq.endIndex   = std::max(0, (this->frameCount - 1));
	}
}

void Rig::loadCameras()
{
	ENSURE(this->params.cameraNames.size() > 0);

	printf("Loading cameras...\n");
	for(uint iCamera = 0; iCamera < this->params.cameraNames.size(); iCamera++)
	{
		printf("."); fflush(stdout);
		Camera newCamera(this->params.cameraNames[iCamera]);		
		this->cameras.push_back(newCamera);
	}
	printf("\n");

	setupCamNeighs();
	if(this->params.halfSizeCamMats)
		halfSizeCamMats();

	CheckForConsistentFCs(); //ensures all cameras have the same FC
	CheckAllCamNeighs();
	this->frameCount = this->cameras[0].frameCount;

	//PrintCamNeighsByCalib(5);
	//PrintCamNeighsByPointCloud(5);
	//exit(0);
}

void Rig::setupCamNeighs()
{
	for(uint iNeighPair = 0; iNeighPair < this->params.camNeighPairs.size(); iNeighPair++)
	{
		CamPair currCamPair = this->params.camNeighPairs[iNeighPair];
		int cam1Index = -1, cam2Index = -1;
		Camera *cam1 = GetCamera(currCamPair.cam1ID, cam1Index);
		Camera *cam2 = GetCamera(currCamPair.cam2ID, cam2Index);
		ENSURE(cam1 != NULL);
		ENSURE(cam2 != NULL);

		if(this->params.useGeoCamDists)
		{
			CVector3 cam1_loc = cam1->cameraMats.GetWorldCoordCoP();
			CVector3 cam2_loc = cam2->cameraMats.GetWorldCoordCoP();
			currCamPair.dist = (float) (cam1_loc - cam2_loc).Length();			
		}

		CameraNeigh cam1Neigh;
		cam1Neigh.id = currCamPair.cam2ID;
		cam1Neigh.dist = currCamPair.dist;
		cam1->cameraNeighs.push_back(cam1Neigh);

		CameraNeigh cam2Neigh;
		cam2Neigh.id = currCamPair.cam1ID;
		cam2Neigh.dist = currCamPair.dist;
		cam2->cameraNeighs.push_back(cam2Neigh);
	}

	normalizeCamNeighDists();

	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
	  //check
	  //this->cameras[iCam].PrintCamNeighs();
	}
}

void Rig::normalizeCamNeighDists()
{
	float maxNeighDist = 0.0f;

	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		CameraNeighs &camNeighs = this->cameras[iCam].cameraNeighs;
		//bug
		//ENSURE(camNeighs.size() > 0);
		for(uint iNeigh = 0; iNeigh < camNeighs.size(); iNeigh++)
		{
			SET_IF_GT(maxNeighDist, camNeighs[iNeigh].dist);
		}
	}

	ENSURE(maxNeighDist > 0.0f);

	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		CameraNeighs &camNeighs = this->cameras[iCam].cameraNeighs;
		for(uint iNeigh = 0; iNeigh < camNeighs.size(); iNeigh++)
		{
			camNeighs[iNeigh].dist /= maxNeighDist;
		}
	}
}

void Rig::halfSizeCamMats()
{
	ENSURE(this->params.halfSizeCamMats);

	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		CameraMats &camMats = this->cameras[iCam].cameraMats;
		camMats.ScaleMats(0.5f);
	}
}

void Rig::RecenterCameras(const CameraMats centerCamMats)
{
	ENSURE(this->cameras.size() > 0);
	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		this->cameras[iCam].Recenter(centerCamMats);
	}
}

void Rig::RecenterCameras(string centerCamID)
{
	int centerCamIndex = -1;
	Camera *centerCam = GetCamera(centerCamID, centerCamIndex);
	CameraMats centerCamMats = centerCam->cameraMats;
	RecenterCameras(centerCamMats);
}


Camera* Rig::GetCamera(string camID, int &camIndex)
{
	//check - speed up with hash tables?
	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		if(this->cameras[iCam].id.compare(camID) == 0)
		{
			camIndex = iCam;
			return &this->cameras[iCam];
		}
	}

	camIndex = -1;
	REPORT_FAILURE("Unable to find a camera with id: %s\n", camID.c_str());

	return NULL;
}

Camera* Rig::GetCamera(string camName)
{
	int camIndex = -1;
	return GetCamera(camName, camIndex);
}


void Rig::OutputFrame(int iFrame)
{
	ENSURE(this->frameCount > 0);
	CHECK_RANGE(iFrame, 0, this->frameCount - 1);

	printf("Writing rig frame - %i\n", iFrame);

	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		printf(".");
		fflush(stdout);
		string outFN = getOutFrameName(iFrame, iCam);
		//string inFN = this->cameras[iCam].GetFrameName(iFrame);
		string inFN = this->cameras[iCam].GetFrameDepsVisName(iFrame);

		CByteImage frameImg;
		ImageIO::ReadFile(frameImg, inFN);
		ImageIO::WriteFile(frameImg, outFN);
	}
	printf("\n");
}

void Rig::PrintCamNeighsByCalib(int maxNeighs)
{
	vector< std::pair<double, int> > camNeighs;

	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		CVector3 camCOP = this->cameras[iCam].cameraMats.GetWorldCoordCoP();
		camNeighs.clear();
		
		for(uint jCam = 0; jCam < this->cameras.size(); jCam++)
		{
			if(jCam != iCam)
			{
				CVector3 neighCOP = this->cameras[jCam].cameraMats.GetWorldCoordCoP();
				double dist  = (camCOP - neighCOP).Length();
				camNeighs.push_back(std::make_pair(dist, jCam));
			}
		}		

		std::sort(camNeighs.begin(), camNeighs.end());

		printf("Cam-%s neighbours\n", this->cameras[iCam].id.c_str());
		int neighCount = min(maxNeighs, (int) camNeighs.size());
		for(int iNeigh = 0; iNeigh < neighCount; iNeigh++)
		{
			int neighCamIndex = camNeighs[iNeigh].second;
			printf("\tCam-%s\n", this->cameras[neighCamIndex].id.c_str());
		}		
		printf("\n");
	}
	printf("\n");
}


void Rig::PrintCamNeighsByPointCloud(int maxNeighs)
{
	INSIST(maxNeighs < (int) this->cameras.size());

	ScenePoints scenePoints;
	scenePoints.Read(this->GetScenePtsFN());
	ENSURE(scenePoints.size() > 0);	

	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		if(IsVideoFrame(iCam) == true) continue;
		vector<IntPair> spInCommon(this->cameras.size());
		for(uint jCam = 0; jCam < this->cameras.size(); jCam++)
		{
			spInCommon[jCam] = IntPair(0, jCam);
		}		

		for(ScenePoints::const_iterator scenePoint = scenePoints.begin(); 
			scenePoint != scenePoints.end();
			scenePoint++)
		{
			bool useCurrSP = false;
			for(vector<IntPair>::const_iterator currView = scenePoint->viewFeaList.begin();
				currView != scenePoint->viewFeaList.end();
				currView++)
			{
				if(currView->first == iCam)
				{
					useCurrSP = true;
					break;
				}
			}

			if(useCurrSP == true)
			{
				for(vector<IntPair>::const_iterator currView = scenePoint->viewFeaList.begin();
					currView != scenePoint->viewFeaList.end();
					currView++)
				{
					if(currView->first != iCam)
					{
						if(IsVideoFrame(currView->first) == true)
						{
							spInCommon[currView->first].first++;
						}
					}
				}
			}
		}

		std::sort(spInCommon.begin(), spInCommon.end(), greater<IntPair>( ));

		printf("%s\n", this->cameras[iCam].id.c_str());
		for(int iNeigh = 0; iNeigh < maxNeighs; iNeigh++)
		{
			int neighCamID     = spInCommon[iNeigh].second;
			int pointsInCommon = spInCommon[iNeigh].first;
			printf("\t %s - %i\n", this->cameras[neighCamID].id.c_str(), pointsInCommon);
		}		
	}
}


void Rig::OutputFMatrices()
{
	printf("Writing FMatrices to %s\n", this->params.outDir.c_str());

	for(vector<Camera>::const_iterator currCam = this->cameras.begin(); 
		currCam != this->cameras.end();
		currCam++)
	{
	  const CameraMats &currCamMats = currCam->cameraMats;

		printf("\t%s\n", currCam->id.c_str());
		for(CameraNeighs::const_iterator currCamNeigh = currCam->cameraNeighs.begin();
			currCamNeigh != currCam->cameraNeighs.end();
			currCamNeigh++)
		{
			const Camera *neighCam = GetCamera(currCamNeigh->id);
			const CameraMats &neighCamMats = neighCam->cameraMats;

			CTransform3x3 fmatrix = currCamMats.GenerateFMatrix(neighCamMats, true);

			string fmatrixFN = getFMatrixFN(currCam->id, neighCam->id);
			ofstream outStream(fmatrixFN.c_str());
			ENSURE(outStream.is_open());
			fmatrix.Write(outStream);
			outStream.close();
		}
	}
}

bool Rig::IsVideoFrame(int camIndex)
{
	if(this->videoFrameMap == NULL)
	{
		ENSURE(this->cameras.size() > 0);

		this->videoFrameMap = new bool[this->cameras.size()];
		ENSURE(this->videoFrameMap != NULL);

		for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
		{
			string camID = this->cameras[iCam].id;
			ENSURE(camID.length() >= 3);
			string camID_prefix = camID.substr(0, 3);

			if(camID_prefix.compare("vid") == 0)
				this->videoFrameMap[iCam] = true;
			else
			{
				ENSURE(camID_prefix.compare("IMG") == 0);
				this->videoFrameMap[iCam] = false;
			}
		}
	}

	return this->videoFrameMap[camIndex];
}

bool Rig::IsVideoFrame(string camID)
{
	int camIndex;
	Camera *cam = GetCamera(camID, camIndex);
	ENSURE(cam != NULL);
	return IsVideoFrame(camIndex);
}

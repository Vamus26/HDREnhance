#include "RigCalibrator.h"
#include "ScenePoints.h"
#include "Disk.h"

//check - save points data and use in stereo
void RigCalibrator::loadNoahDrewCalibFile(int minViewsForScenePoint, bool drewMode)
{
	ENSURE(minViewsForScenePoint >= 0);

	printf("Loading Noah calib from - %s...\n", this->params.calibFN.c_str());

	ifstream inStream(params.calibFN.c_str());
	ENSURE(inStream.is_open());

	if(drewMode)
	{
		string fileHeader;
		inStream >> fileHeader;
		ENSURE(fileHeader.compare(RIG_CALIBRATOR_DrewFileHeader) == 0); 
	}

	int cameraCount = 0;
	int pointCount  = 0;

	inStream >> cameraCount; 
	inStream >> pointCount; 
	ENSURE(cameraCount > 0);
	ENSURE(cameraCount == (int) this->rig->cameras.size());
	ENSURE(pointCount > 0);

	for(int iCam = 0; iCam < cameraCount; iCam++)
	{
		Camera &currCam = this->rig->cameras[iCam];
		CShape frameShape = currCam.GetFrameShape(0);

		currCam.cameraMats.ReadCameraNoahDrew(inStream, frameShape.width, frameShape.height, drewMode);
		if(currCam.cameraMats.cameraMatsLoaded)
		{
			if(drewMode)
			{
				currCam.cameraMats.InvertAxisY();
			}

			string camFN = currCam.GetCameraMatsFN();
			currCam.cameraMats.WriteCamera(camFN);
			currCam.viewPoints.clear();
		}
	}

	CVector3 pcLowerBound( DBL_MAX,  DBL_MAX,  DBL_MAX);
	CVector3 pcUpperBound(-DBL_MAX, -DBL_MAX, -DBL_MAX);
	ScenePoints scenePoints;
	scenePoints.reserve(pointCount);

	for(int iPoint = 0; iPoint < pointCount; iPoint++)
	{
		ScenePoint currPoint;
		currPoint.Read(inStream, drewMode);
		bool addPoint = false;

		if(InRange(currPoint.pos[2], this->params.pcLowerBound[2], this->params.pcUpperBound[2]) &&
		   InRange(currPoint.pos[1], this->params.pcLowerBound[1], this->params.pcUpperBound[1]) &&
		   InRange(currPoint.pos[0], this->params.pcLowerBound[0], this->params.pcUpperBound[0]))
		{
			if(this->params.minViewsForScenePoint <= (int) currPoint.viewFeaList.size())
				addPoint = true;
			else		
			{
				int imgCount = 0;
				for(uint iView = 0; iView < currPoint.viewFeaList.size(); iView++)
				{
					int viewCamID = currPoint.viewFeaList[iView].first;
					if(this->rig->IsVideoFrame(viewCamID) == false)
					{
						imgCount++;
					}
				}

				if(imgCount > 2)
				{
					addPoint = true;
				}
			}
		}

		if(addPoint)
		{
			scenePoints.push_back(currPoint);

			SET_IF_LT(pcLowerBound[0], currPoint.pos[0]);
			SET_IF_LT(pcLowerBound[1], currPoint.pos[1]);
			SET_IF_LT(pcLowerBound[2], currPoint.pos[2]);

			SET_IF_GT(pcUpperBound[0], currPoint.pos[0]);
			SET_IF_GT(pcUpperBound[1], currPoint.pos[1]);
			SET_IF_GT(pcUpperBound[2], currPoint.pos[2]);
		}
	}

	//scenePoints = scenePoints.Prune(3);
	scenePoints.GenerateViewPoints(this->rig->cameras);
	string scenePtsFN = this->rig->GetScenePtsFN();
	scenePoints.Write(scenePtsFN);

	double medianNeighFrameDist = this->rig->GetMedianNeighVideoFrameDist();
	PRINT_DOUBLE(medianNeighFrameDist);

	vector<CVector3> camWorldCoords;
	camWorldCoords.reserve(cameraCount);
	int vpCount = 0;
	for(int iCam = 0; iCam < cameraCount; iCam++)
	{
		#if defined(__ST__) || defined(__SUZZALO__) 
			if(this->rig->IsVideoFrame(iCam))
			{
				bool leftNeighDist = false;
				bool righNeighDist = false;

				CVector3 currCamCoord = this->rig->cameras[iCam].cameraMats.GetWorldCoordCoP();
				if(iCam > 0)
				{
					if(this->rig->IsVideoFrame(iCam - 1))
					{
						CVector3 prevCamCoord = this->rig->cameras[iCam - 1].cameraMats.GetWorldCoordCoP();
						double neighCamDist = (prevCamCoord - currCamCoord).Length();

						if(neighCamDist > 10.0 * medianNeighFrameDist)
						{
							leftNeighDist = true;						
						}
					}
				}

				if(iCam < (int) this->rig->cameras.size() - 1)
				{
					if(this->rig->IsVideoFrame(iCam + 1))
					{
						CVector3 nextCamCoord = this->rig->cameras[iCam + 1].cameraMats.GetWorldCoordCoP();
						double neighCamDist = (nextCamCoord - currCamCoord).Length();

						if(neighCamDist > 10.0 * medianNeighFrameDist)
						{
							righNeighDist = true;						
						}
					}
				}

				if(leftNeighDist && righNeighDist)
				{
					PRINT_STRING(this->rig->cameras[iCam].id);
					this->rig->cameras[iCam].cameraMats.cameraMatsLoaded = false;				
					getchar();
				}
			}
		#endif

		Camera &currCam = this->rig->cameras[iCam];
		if(currCam.cameraMats.cameraMatsLoaded)
		{
			currCam.WriteViewPts();
			printf("\t Cam-%s: %i\n", currCam.id.c_str(), (int) currCam.viewPoints.size());
			vpCount += (int) currCam.viewPoints.size();
			camWorldCoords.push_back(currCam.cameraMats.GetWorldCoordCoP());
		}
	}

	printf("\t Total scene points: %i\n", scenePoints.size());
	printf("\t Total view  points: %i\n", vpCount);	

	printf("\t Scene bounds: [%lf, %lf, %lf] - [%lf, %lf, %lf]\n", 
			pcLowerBound[0], pcLowerBound[1], pcLowerBound[2],
			pcUpperBound[0], pcUpperBound[1], pcUpperBound[2]);


	if(this->params.pcUpperBound[0] == DBL_MAX) //params not set by user
	{
		scenePoints.WritePly(Disk::GetFileNameID(scenePtsFN) + ".ply",
							 camWorldCoords, true,
							 pcLowerBound, pcUpperBound, true);
	}
	else
	{
		scenePoints.WritePly(Disk::GetFileNameID(scenePtsFN) + ".ply",
							 camWorldCoords, true,
							 this->params.pcLowerBound, this->params.pcUpperBound, true);
	}	

	inStream.close();
}


void RigCalibrator::loadLarryCalibFile()
{
	printf("Loading Larry calib from - %s...\n", this->params.calibFN.c_str());

	ifstream inStream(params.calibFN.c_str());
	ENSURE(inStream.is_open());

	int cameraCount = (int) this->rig->cameras.size();

	for(int iCam = 0; iCam < cameraCount; iCam++)
	{
		Camera &currCam = this->rig->cameras[iCam];

		printf("\t Cam-%s\n", currCam.id.c_str());

		currCam.cameraMats.ReadCameraLarry(inStream);

		string camFN = currCam.GetCameraMatsFN();
		currCam.cameraMats.WriteCamera(camFN);
	}

	inStream.close();
}


//double medianNeighFrameDist = this->rig->GetMedianNeighVideoFrameDist();
//PRINT_DOUBLE(medianNeighFrameDist);
//
//if(this->rig->IsVideoFrame(iCam))
//{
//	bool leftNeighDist = false;
//	bool righNeighDist = false;
//
//	CVector3 currCamCoord = this->rig->cameras[iCam].cameraMats.GetWorldCoordCoP();
//	if(iCam > 0)
//	{
//		if(this->rig->IsVideoFrame(iCam - 1))
//		{
//			CVector3 prevCamCoord = this->rig->cameras[iCam - 1].cameraMats.GetWorldCoordCoP();
//			double neighCamDist = (prevCamCoord - currCamCoord).Length();
//
//			if(neighCamDist > 100.0 * medianNeighFrameDist)
//			{
//				leftNeighDist = true;						
//			}
//		}
//	}
//
//	if(iCam < (int) this->rig->cameras.size() - 1)
//	{
//		if(this->rig->IsVideoFrame(iCam + 1))
//		{
//			CVector3 nextCamCoord = this->rig->cameras[iCam + 1].cameraMats.GetWorldCoordCoP();
//			double neighCamDist = (nextCamCoord - currCamCoord).Length();
//
//			if(neighCamDist > 100.0 * medianNeighFrameDist)
//			{
//				righNeighDist = true;						
//			}
//		}
//	}
//
//	if(leftNeighDist && righNeighDist)
//	{
//		PRINT_STRING(this->rig->cameras[iCam].id);
//		getchar();
//		continue;
//	}
//}
#include "Stereo.h"
#include "Disk.h"
#include "ScenePoints.h"
#include "PointCloud.h"
#include "PCloudSegmenter.h"
#include "triangulate.h"

void Stereo::Init(StereoParams sParams, Rig::RigParams rParams) 
{
	this->stereoParams = sParams;
	this->rig.Load(rParams);
	this->rig.CheckAllCamMatsLoaded();

	if(this->rig.params.targetCamIndex != -1)
	{
		ENSURE(Disk::DoesFileExist(getScenePlanesFN()));
	}

	loadPlanes();
}

///*
void Stereo::loadPlanes()
{	
	INSIST(this->rig.params.frameSeq.startIndex == this->rig.params.frameSeq.endIndex);

	string scenePlanesFN = getScenePlanesFN();

	if(Disk::DoesFileExist(scenePlanesFN))
	{		
		this->rig.LoadPlanes(this->rig.params.frameSeq.startIndex);
		INSIST(this->rig.cameras.size() > 0);
		this->scenePlaneCount = this->rig.cameras[0].scenePlaneCount;
		printf("Loaded %i scene planes\n", this->scenePlaneCount);	
	}
	else
	{
		Disk::MakeDir(Stereo::GetStereoDirName(this->rig));	

		string scenePtsFN = this->rig.GetScenePtsFN();
		INSIST(Disk::DoesFileExist(scenePtsFN) == true);
		ScenePoints scenePoints;
		scenePoints.Read(scenePtsFN);
		INSIST(scenePoints.size() > 0);
		//scenePoints.PurgeViewList(); //check		

		vector<Plane> scenePlanes;
		findScenePlanes(scenePlanes, scenePoints);
		writeScenePlanesFile(scenePlanes);
		this->scenePlaneCount = (int) scenePlanes.size();

		this->rig.CreatePlanes(this->rig.params.frameSeq.startIndex, 
							   scenePlanes, scenePoints,
							   this->stereoParams.pcLowerBound, this->stereoParams.pcUpperBound,
							   this->stereoParams.pixelDistBetweenFrontoPlanes);		
	}

	//this->rig.VisScenePlanes();
}
//*/


//void Stereo::loadPlanes()
//{	
//	INSIST(this->rig.params.frameSeq.startIndex == this->rig.params.frameSeq.endIndex);
//
//	string scenePlanesFN = getScenePlanesFN();
//
//	vector<Plane> scenePlanes;
//
//	ifstream inStream(scenePlanesFN.c_str());
//	ENSURE(inStream.is_open());
//
//	int intType = -1;
//	inStream >> intType;
//	ENSURE(intType >= 0);
//	scenePlanes.resize(intType);
//
//	for(uint iPlane = 0; iPlane < scenePlanes.size(); iPlane++)
//	{
//		scenePlanes[iPlane].Read(inStream);
//	}
//
//	inStream.close();
//
//	this->scenePlaneCount = (int) scenePlanes.size();
//
//
//	string scenePtsFN = this->rig.GetScenePtsFN();
//	INSIST(Disk::DoesFileExist(scenePtsFN) == true);
//	ScenePoints scenePoints;
//	scenePoints.Read(scenePtsFN);
//	INSIST(scenePoints.size() > 0);
//	scenePoints.PurgeViewList(); //check		
//	
//	this->rig.CreatePlanes(this->rig.params.frameSeq.startIndex, 
//						   scenePlanes, scenePoints,
//						   this->stereoParams.pcLowerBound, this->stereoParams.pcUpperBound,
//						   this->stereoParams.pixelDistBetweenFrontoPlanes);
//}

void Stereo::findScenePlanes(vector<Plane> &scenePlanes, const ScenePoints &scenePoints)
{
	INSIST(scenePlanes.size() == 0);

	if(this->stereoParams.pcsParams.maxPlanesInScene > 0)
	{
		PointCloud pointCloud;		
		scenePoints.GeneratePointCloud(pointCloud);
			
		PCloudSegmenter pcloudSegmenter(pointCloud);
		pcloudSegmenter.SegmentPointCloud(scenePlanes, this->stereoParams.pcsParams);

		INSIST(scenePlanes.size() > 0);

		for(uint iScenePlane = 0; iScenePlane < scenePlanes.size(); iScenePlane++)
		{
			scenePlanes[iScenePlane].inlierCloud.clear();
		}
	}

	if(this->rig.params.userDefinedPlanes.size() > 0)
	{
		extractUserDefinedPlanes(scenePlanes);
	}

	printf("\n\t Found %i scene planes\n", scenePlanes.size());	
}

void Stereo::extractUserDefinedPlanes(vector<Plane> &scenePlanes)
{
	printf("Extracting user defined planes..\n");
	INSIST(this->rig.params.userDefinedPlanes.size() > 0);

	for(uint iUserPlane = 0; iUserPlane < this->rig.params.userDefinedPlanes.size(); iUserPlane++)
	{
		const MV_Correspondances &mvCorrespondances = this->rig.params.userDefinedPlanes[iUserPlane];
		INSIST(mvCorrespondances.size() >= 3);

		PointCloud planePoints;
		for(uint iPlanePoint = 0; iPlanePoint < mvCorrespondances.size(); iPlanePoint++)
		{
			const MV_Correspondance &mvCorrespondance = mvCorrespondances[iPlanePoint];
			int viewPoints = (int) mvCorrespondance.camIDs.size();
			INSIST(viewPoints == (int) mvCorrespondance.points.size());
			INSIST(viewPoints >= 2);

			v2_t *viewPointsNoah = new v2_t[viewPoints];
			double *rotMats      = new double[9 * viewPoints];
			double *tranVecs     = new double[3 * viewPoints];
			
			INSIST(viewPointsNoah != NULL);
			INSIST(rotMats != NULL);
			INSIST(tranVecs != NULL);

			for(int iView = 0; iView < viewPoints; iView++)
			{
				string camID = mvCorrespondance.camIDs[iView];
				const Camera *viewCam = this->rig.GetCamera(camID);
				INSIST(viewCam != NULL);

				CShape viewShape = viewCam->GetFrameShape(0);
				CVector2 viewPoint = mvCorrespondance.points[iView];
				INSIST(viewShape.InBounds(viewPoint[0], viewPoint[1]));
				viewPoint[1] = (viewShape.height - 1) - viewPoint[1];

				CVector3 viewPointCC(viewPoint[0], viewPoint[1], 1.0);
				viewPointCC = viewCam->cameraMats.GetIntrinsicMatInv() * viewPointCC;

				viewPointsNoah[iView].p[0] = viewPointCC[0];
				viewPointsNoah[iView].p[1] = viewPointCC[1];				

				tranVecs[(3 * iView) + 0] = viewCam->cameraMats.transVec[0];
				tranVecs[(3 * iView) + 1] = viewCam->cameraMats.transVec[1];
				tranVecs[(3 * iView) + 2] = viewCam->cameraMats.transVec[2];

				rotMats[ (9 * iView) + 0] = viewCam->cameraMats.rotMat[0][0];
				rotMats[ (9 * iView) + 1] = viewCam->cameraMats.rotMat[0][1];
				rotMats[ (9 * iView) + 2] = viewCam->cameraMats.rotMat[0][2];
				rotMats[ (9 * iView) + 3] = viewCam->cameraMats.rotMat[1][0];
				rotMats[ (9 * iView) + 4] = viewCam->cameraMats.rotMat[1][1];
				rotMats[ (9 * iView) + 5] = viewCam->cameraMats.rotMat[1][2];
				rotMats[ (9 * iView) + 6] = viewCam->cameraMats.rotMat[2][0];
				rotMats[ (9 * iView) + 7] = viewCam->cameraMats.rotMat[2][1];
				rotMats[ (9 * iView) + 8] = viewCam->cameraMats.rotMat[2][2];				
			}

			double error_out;
			v3_t planePointNoah = triangulate_n(viewPoints, 
												viewPointsNoah, rotMats, tranVecs, &error_out);
			INSIST(fabs(error_out) <= 4.0);

			CVector3 planePoint(planePointNoah.p[0], planePointNoah.p[1], planePointNoah.p[2]);

			planePoints.push_back(planePoint);

			delete [] viewPointsNoah;
			delete [] rotMats;
			delete [] tranVecs;			
		}

		Plane userDefinedPlane;
		PlaneFromPoints::SolveForPlane(planePoints, userDefinedPlane);
		userDefinedPlane.col = FeatureRenderer::GetRandomColor();
		scenePlanes.push_back(userDefinedPlane);

		printf("\t Found plane %i inliers.\n", planePoints.size());
	}

	printf("\t Done.\n");
}

void Stereo::writeScenePlanesFile(const vector<Plane> &scenePlanes)
{
	string scenePlanesFN = getScenePlanesFN();
	ofstream outStream(scenePlanesFN.c_str());
	ENSURE(outStream.is_open());

	outStream << "\n";
	outStream << scenePlanes.size() << "\n";
	for(uint iPlane = 0; iPlane < scenePlanes.size(); iPlane++)
	{
		INSIST(scenePlanes[iPlane].inlierCloud.size() == 0);
		scenePlanes[iPlane].Write(outStream);
	}
	outStream << "\n";

	outStream.close();
}


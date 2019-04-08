#include "Camera.h"
#include "Segment.h"

//bug - speed up using binary search
int Camera::GetBestFrontoPlaneIndex(double depth) const
{
	if(InRange(depth, this->minDepth, this->maxDepth) == false)
		return SEGMENT_OccDepPlaneIndex;

	int bestFrontoPlaneIndex    = SEGMENT_OccDepPlaneIndex;
	double bestFrontoPlaneScore = DBL_MAX;
	for(int iFrontoPlane = 0; iFrontoPlane < this->frontoPlaneCount; iFrontoPlane++)
	{
		double frontoPlaneScore = fabs(this->frontoPlanes[iFrontoPlane] - depth);
		if(frontoPlaneScore < bestFrontoPlaneScore)
		{
			bestFrontoPlaneScore = frontoPlaneScore;
			bestFrontoPlaneIndex = iFrontoPlane;
		}
	}
	ENSURE(bestFrontoPlaneIndex != SEGMENT_OccDepPlaneIndex);

	return bestFrontoPlaneIndex;
}

void Camera::CreatePlanes(int iFrame,
						  const vector<Plane> &globalScenePlanes, 
						  const ScenePoints &scenePoints,
						  CVector3 pcLowerBound, CVector3 pcUpperBound,
						  double pixelDistBetweenFrontoPlanes,
						  vector<Camera *> neighCams)
{
	INSIST(this->frontoPlanes.size() == 0);
	INSIST(this->planes.size()       == 0);
	INSIST(this->scenePlanes.size()  == 0);
	INSIST(this->frontoPlaneCount    == 0);
	INSIST(this->planeCount          == 0);
	INSIST(this->scenePlaneCount     == 0);

	createScenePlanes(globalScenePlanes);	

	createFrontoParallelPlanes(scenePoints, pcLowerBound, pcUpperBound, 
							   pixelDistBetweenFrontoPlanes, neighCams);
	INSIST(this->frontoPlanes.size() > 0);

	createPlanes();

	writeFrontoPlanesFile();
	writeScenePlanesFile(iFrame);
}

void Camera::LoadPlanes(int iFrame)
{
	//bug
	//if(this->id.substr(0, 3).compare("vid") == 0)
	//{
	//	this->scenePlanes.clear();
	//	this->frontoPlanes.clear();
	//	this->frontoPlanes.push_back(0.005);
	//	this->frontoPlanes.push_back(0.1);
	//	writeFrontoPlanesFile();
	//	writeScenePlanesFile(iFrame);
	//	this->scenePlanes.clear();
	//	this->frontoPlanes.clear();
	//}


	readScenePlanesFile(iFrame);
	readFrontoPlanesFile();	

	createPlanes();
}

void Camera::createPlanes()
{
	this->planes.insert(this->planes.begin(), this->scenePlanes.begin(), this->scenePlanes.end());
	for(uint iFrontoPlane = 0; iFrontoPlane < this->frontoPlanes.size(); iFrontoPlane++)
	{
		Plane frontoPlane;
		frontoPlane.normal = CVector3(0.0, 0.0, -1.0);
		frontoPlane.D      = this->frontoPlanes[iFrontoPlane];
		frontoPlane.point  = CVector3(0.0, 0.0, this->frontoPlanes[iFrontoPlane]);
		
		double depthCol    = 1.0 - ((double) iFrontoPlane / this->frontoPlanes.size());
		frontoPlane.col    = CVector3(depthCol, depthCol, depthCol);
		this->planes.push_back(frontoPlane);
	}

	this->scenePlaneCount  = (int) this->scenePlanes.size();
	this->frontoPlaneCount = (int) this->frontoPlanes.size();
	this->planeCount       = (int) this->planes.size();
}

void Camera::createScenePlanes(const vector<Plane> &globalScenePlanes)
{
	for(uint iPlane = 0; iPlane < globalScenePlanes.size(); iPlane++)
	{
		Plane newScenePlane = globalScenePlanes[iPlane];
		double normLen = newScenePlane.normal.Length();
		CHECK_RANGE(normLen, 0.99, 1.01);
		ENSURE(fabs(newScenePlane.normal.dot(newScenePlane.point) + newScenePlane.D) <= 0.0001);

		CVector3 normPoint = newScenePlane.point + newScenePlane.normal;
		newScenePlane.point = this->cameraMats.transformCoordSys(newScenePlane.point);
		normPoint = this->cameraMats.transformCoordSys(normPoint);

		newScenePlane.normal = normPoint - newScenePlane.point;
		normLen = newScenePlane.normal.Length();
		CHECK_RANGE(normLen, 0.99, 1.01);

		newScenePlane.D = -newScenePlane.normal.dot(newScenePlane.point);

		for(uint iInlier = 0; iInlier < newScenePlane.inlierCloud.size(); iInlier++)
		{
			newScenePlane.inlierCloud[iInlier].pos = 
				this->cameraMats.transformCoordSys(newScenePlane.inlierCloud[iInlier].pos);
		}

		this->scenePlanes.push_back(newScenePlane);
	}
}

void Camera::readScenePlanesFile(int iFrame)
{
	ENSURE(this->scenePlanes.size() == 0);

	string scenePlanesFN = GetScenePlanesFN(iFrame);
	ifstream inStream(scenePlanesFN.c_str());

	if(inStream.is_open() == false)
	{
		PRINT_STRING(this->id);
	}

	ENSURE(inStream.is_open());

	int intType = -1;
	inStream >> intType;
	ENSURE(intType >= 0);
	this->scenePlanes.resize(intType);

	for(uint iPlane = 0; iPlane < this->scenePlanes.size(); iPlane++)
	{
		this->scenePlanes[iPlane].Read(inStream);
	}

	inStream.close();
}


void Camera::writeScenePlanesFile(int iFrame)
{
	string scenePlanesFN = GetScenePlanesFN(iFrame);
	WriteScenePlanesFile(scenePlanesFN);
}

void Camera::WriteScenePlanesFile(string scenePlanesFN)
{
	ofstream outStream(scenePlanesFN.c_str());
	ENSURE(outStream.is_open());

	outStream << "\n";
	outStream << this->scenePlanes.size() << "\n";
	for(uint iPlane = 0; iPlane < this->scenePlanes.size(); iPlane++)
	{
		INSIST(this->scenePlanes[iPlane].inlierCloud.size() == 0);
		this->scenePlanes[iPlane].Write(outStream);
	}
	outStream << "\n";

	outStream.close();
}


void Camera::readFrontoPlanesFile()
{
	string frontoPlanesFN = GetFrontoPlanesFN();
	Camera::ReadFrontoPlanesFile(this->frontoPlanes, frontoPlanesFN);	

	if(this->frontoPlanes.size() < 2)
	{
		PRINT_STRING(this->id);
	}

	INSIST(this->frontoPlanes.size() >= 2);
	this->minDepth = this->frontoPlanes[0];
	this->maxDepth = this->frontoPlanes[this->frontoPlanes.size() - 1];
	INSIST(this->minDepth <= this->maxDepth);

	this->minDepth -= 0.0001;
	this->maxDepth += 0.0001;
	SET_IF_GT(this->minDepth, 0.0);	
}

void Camera::ReadFrontoPlanesFile(vector<double> &fPlanes, string frontoPlanesFN)
{
	INSIST(fPlanes.size() == 0);

	ifstream inStream;
	inStream.open(frontoPlanesFN.c_str());
	ENSURE(inStream.is_open());

	int planesFound = -1;
	inStream >> planesFound;
	ENSURE(planesFound > 0);
	fPlanes.clear();
	fPlanes.reserve(planesFound);
	for(int iPlane = 0; iPlane < planesFound; iPlane++)
	{
		double currDepth = -1;
		inStream >> currDepth;
		ENSURE(currDepth >= 0.0);
		fPlanes.push_back(currDepth);
	}

	inStream.close();
}

void Camera::writeFrontoPlanesFile()
{
	string frontoPlanesFN = GetFrontoPlanesFN();
	WriteFrontoPlanesFile(frontoPlanesFN);
}

void Camera::WriteFrontoPlanesFile(string frontoPlanesFN)
{
	ofstream outStream;
	outStream.open(frontoPlanesFN.c_str());
	ENSURE(outStream.is_open());

	ENSURE(this->frontoPlanes.size() > 0);
	outStream << this->frontoPlanes.size() << "\n";
	for(uint iPlane = 0; iPlane < this->frontoPlanes.size(); iPlane++)
	{
		outStream << this->frontoPlanes[iPlane] << "\n";
	}

	outStream.close();
}

void Camera::getMinMaxDepthFromBounds(const ScenePoints &scenePoints, CVector3 pcLowerBound, CVector3 pcUpperBound,
									  double &minDepth, double &maxDepth)
{
	INSIST(pcLowerBound[0] < pcUpperBound[0]);
	INSIST(pcLowerBound[1] < pcUpperBound[1]);
	INSIST(pcLowerBound[2] < pcUpperBound[2]);

	minDepth =  DBL_MAX;
	maxDepth = -DBL_MAX;

	if(pcUpperBound[0] == DBL_MAX) //params not set by user
	{
		ScenePoints culledScenePts = scenePoints.GetVisibleScenePoints(this);
		culledScenePts.TransformCoordSys(this->cameraMats);
		culledScenePts.GetBounds(pcLowerBound, pcUpperBound);
		minDepth = pcLowerBound[2];
		maxDepth = pcUpperBound[2];
	}
	else
	{
		for(int iBound = 0; iBound < 8; iBound++)
		{
			CVector3 boundPT;

			switch(iBound)
			{
			case 0:
				boundPT = CVector3(pcLowerBound[0], pcLowerBound[1], pcLowerBound[2]);
				break;
			case 1:
				boundPT = CVector3(pcUpperBound[0], pcLowerBound[1], pcLowerBound[2]);
				break;
			case 2:
				boundPT = CVector3(pcLowerBound[0], pcUpperBound[1], pcLowerBound[2]);
				break;
			case 3:
				boundPT = CVector3(pcUpperBound[0], pcUpperBound[1], pcLowerBound[2]);
				break;
			case 4:
				boundPT = CVector3(pcLowerBound[0], pcLowerBound[1], pcUpperBound[2]);
				break;
			case 5:
				boundPT = CVector3(pcUpperBound[0], pcLowerBound[1], pcUpperBound[2]);
				break;
			case 6:
				boundPT = CVector3(pcLowerBound[0], pcUpperBound[1], pcUpperBound[2]);
				break;
			case 7:
				boundPT = CVector3(pcUpperBound[0], pcUpperBound[1], pcUpperBound[2]);
				break;
			default:
				REPORT_FAILURE("Unknown bound type - %i\n", iBound);
				break;
			}

			CVector3 transformedBoundPT = this->cameraMats.transformCoordSys(boundPT);

			SET_IF_LT(minDepth, transformedBoundPT[2]);
			SET_IF_GT(maxDepth, transformedBoundPT[2]);
		}
	}

	ENSURE(minDepth !=  DBL_MAX);
	ENSURE(maxDepth != -DBL_MAX);
	SET_IF_GT(minDepth, CAMERA_MinDepth);
	SET_IF_LT(minDepth, maxDepth);
}

void Camera::createFrontoParallelPlanes(const ScenePoints &scenePoints,
										CVector3 pcLowerBound, CVector3 pcUpperBound, 
										double pixelDistBetweenFrontoPlanes,
										vector<Camera *> neighCams)
{
	INSIST(pixelDistBetweenFrontoPlanes > 0.0f);
	INSIST(this->frontoPlanes.size() == 0);

	getMinMaxDepthFromBounds(scenePoints, pcLowerBound, pcUpperBound, 
							 this->minDepth, this->maxDepth);
	INSIST(this->minDepth < this->maxDepth);

	CShape leftImgShape = GetFrameShape(0);
	CameraMats leftCamMats = this->cameraMats;
	leftCamMats.Recenter(this->cameraMats);

	this->frontoPlanes.clear();
	string bestNeighID;	
	for(uint iNeigh = 0; iNeigh < neighCams.size(); iNeigh++)
	{
		string neighCamID = neighCams[iNeigh]->id;
		printf("Neigh: %s\n", neighCamID.c_str());
			
		for(int iLoc = 0; iLoc < 5; iLoc++)
		{
			vector<double> frontoPlanesWithNeigh;

			CShape rightImgShape = neighCams[iNeigh]->GetFrameShape(0);
			CameraMats rightCamMats = neighCams[iNeigh]->cameraMats;		
			rightCamMats.Recenter(this->cameraMats);

			CVector3 leftUVZ(0.0, 
							 0.0, 
							 this->minDepth);
			switch(iLoc)
			{
			case 0:
				leftUVZ[0] = leftImgShape.width / 2.0;
				leftUVZ[1] = leftImgShape.height / 2.0;
				break;

			case 1:
				leftUVZ[0] = 0.0;
				leftUVZ[1] = 0.0;
				break;
			case 2:
				leftUVZ[0] = leftImgShape.width - 1;
				leftUVZ[1] = 0.0;
				break;
			case 3:
				leftUVZ[0] = 0.0;
				leftUVZ[1] = leftImgShape.height - 1;
				break;
			case 4:
				leftUVZ[0] = leftImgShape.width - 1;
				leftUVZ[1] = leftImgShape.height - 1;
				break;

			default:
				REPORT_FAILURE("FAILURE");
				break;
			}

			CVector3 worldXYZ;
			CVector2 rightUV;

			leftCamMats.projUVZtoXYZ(leftUVZ, worldXYZ);
			rightCamMats.projXYZtoUV(worldXYZ, rightUV);

			bool searchStagnated = false;
			while((leftUVZ[2] <= this->maxDepth) && (searchStagnated == false))
			{
				printf("."); fflush(stdout);

				CVector2 lastRightUV = rightUV;
				if(rightImgShape.InBounds(lastRightUV[0], lastRightUV[1]) == false)
				{
					//REPORT("Warning: depth plane projected out of bounds in neigh view");				
				}
				frontoPlanesWithNeigh.push_back(leftUVZ[2]);

				double wIncr = 1.0;	
				bool correctIncrFound = false;
				int iterations = 0;
				while((correctIncrFound == false) && (searchStagnated == false))
				{			
					leftUVZ[2] += wIncr;

					leftCamMats.projUVZtoXYZ(leftUVZ, worldXYZ);
					rightCamMats.projXYZtoUV(worldXYZ, rightUV);

					double dist = rightUV.Dist(lastRightUV);
					double distErr = dist - pixelDistBetweenFrontoPlanes;
					
					if(fabs(distErr) < 0.001) 
						correctIncrFound = true;
					else if(distErr > 0.0)
						wIncr = fabs(wIncr) / -2.0;
					else
						wIncr = fabs(wIncr) * 2.0;

					iterations++;
					if(iterations > 40)
					{
						searchStagnated = true;
					}
				}
			}

			INSIST(frontoPlanesWithNeigh.size() > 0);
			if(searchStagnated)
			{
				REPORT("Stagnated search with %s\n", neighCamID.c_str());
			}		

			if(this->frontoPlanes.size() < frontoPlanesWithNeigh.size())
			{
				this->frontoPlanes = frontoPlanesWithNeigh;
				bestNeighID = neighCamID;
			}
			
			printf("\n");
		}
	}
	INSIST(this->frontoPlanes.size() > 0);

	this->minDepth = this->frontoPlanes[0] - 0.0001;
	this->maxDepth = this->frontoPlanes[this->frontoPlanes.size() - 1] + 0.0001;
	SET_IF_GT(this->minDepth, CAMERA_MinDepth);	

	printf("\nFound %i fronto planes for %s using %s.\n\n\n", this->frontoPlanes.size(), this->id.c_str(), bestNeighID.c_str());
}



/*
void Camera::createFrontoParallelPlanes(const ScenePoints &scenePoints,
										CVector3 pcLowerBound, CVector3 pcUpperBound, 
										double pixelDistBetweenFrontoPlanes,
										vector<Camera *> neighCams)
{
	INSIST(pixelDistBetweenFrontoPlanes > 0.0f);
	INSIST(this->frontoPlanes.size() == 0);

	getMinMaxDepthFromBounds(scenePoints, pcLowerBound, pcUpperBound, 
							 this->minDepth, this->maxDepth);
	INSIST(this->minDepth < this->maxDepth);

	CShape leftImgShape = GetFrameShape(0);
	CameraMats leftCamMats = this->cameraMats;
	leftCamMats.Recenter(this->cameraMats);

	this->frontoPlanes.clear();
	string bestNeighID;
	
	for(uint iNeigh = 0; iNeigh < neighCams.size(); iNeigh++)
	{
		string neighCamID = neighCams[iNeigh]->id;
		printf("Neigh: %s\n", neighCamID.c_str());

		
		#ifdef __GROTTO__
			if((this->id.substr(0, 3).compare("vid") == 0) &&
			   (neighCamID.substr(0, 3).compare("vid") == 0))
			{
				pixelDistBetweenFrontoPlanes /= 2.0f;
			}
		#endif

		vector<double> frontoPlanesWithNeigh;

		CShape rightImgShape = neighCams[iNeigh]->GetFrameShape(0);
		CameraMats rightCamMats = neighCams[iNeigh]->cameraMats;		
		rightCamMats.Recenter(this->cameraMats);

		CVector3 leftUVZ(leftImgShape.width  / 2.0, 
						 leftImgShape.height / 2.0, 
						 this->minDepth);
		CVector3 worldXYZ;
		CVector2 rightUV;

		leftCamMats.projUVZtoXYZ(leftUVZ, worldXYZ);
		rightCamMats.projXYZtoUV(worldXYZ, rightUV);

		bool searchStagnated = false;
		while((leftUVZ[2] <= this->maxDepth) && (searchStagnated == false))
		{
			printf("."); fflush(stdout);

			CVector2 lastRightUV = rightUV;
			if(rightImgShape.InBounds(lastRightUV[0], lastRightUV[1]) == false)
			{
				REPORT("Warning: depth plane projected out of bounds in neigh view");				
			}
			frontoPlanesWithNeigh.push_back(leftUVZ[2]);

			double wIncr = 1.0;	
			bool correctIncrFound = false;
			int iterations = 0;
			while((correctIncrFound == false) && (searchStagnated == false))
			{			
				leftUVZ[2] += wIncr;

				leftCamMats.projUVZtoXYZ(leftUVZ, worldXYZ);
				rightCamMats.projXYZtoUV(worldXYZ, rightUV);

				double dist = rightUV.Dist(lastRightUV);
				double distErr = dist - pixelDistBetweenFrontoPlanes;
				
				if(fabs(distErr) < 0.001) 
					correctIncrFound = true;
				else if(distErr > 0.0)
					wIncr = fabs(wIncr) / -2.0;
				else
					wIncr = fabs(wIncr) * 2.0;

				iterations++;
				if(iterations > 40)
				{
					searchStagnated = true;
				}
			}
		}

		INSIST(frontoPlanesWithNeigh.size() > 0);
		if(searchStagnated)
		{
			REPORT("Stagnated search with %s\n", neighCamID.c_str());
		}		

		if(this->frontoPlanes.size() < frontoPlanesWithNeigh.size())
		{
			this->frontoPlanes = frontoPlanesWithNeigh;
			bestNeighID = neighCamID;
		}

		#ifdef __GROTTO__
			if((this->id.substr(0, 3).compare("vid") == 0) &&
			   (neighCamID.substr(0, 3).compare("vid") == 0))
			{
				pixelDistBetweenFrontoPlanes *= 2.0f;
			}
		#endif

		printf("\n");
	}
	INSIST(this->frontoPlanes.size() > 0);

	this->minDepth = this->frontoPlanes[0] - 0.0001;
	this->maxDepth = this->frontoPlanes[this->frontoPlanes.size() - 1] + 0.0001;
	SET_IF_GT(this->minDepth, CAMERA_MinDepth);	

	printf("\nFound %i fronto planes for %s using %s.\n\n\n", this->frontoPlanes.size(), this->id.c_str(), bestNeighID.c_str());	
}

*/
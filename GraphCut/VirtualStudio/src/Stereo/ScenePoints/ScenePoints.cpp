#include "ScenePoints.h"
#include <algorithm>
#include <math.h>
#include "PointCloud.h"

void ScenePoint::Read(ifstream &inStream, bool drewMode)
{
	this->pos.Read(inStream);
	this->col.Read(inStream);	

	this->viewFeaList.clear();
	int viewCount = 0;
	inStream >> viewCount;
	this->viewFeaList.reserve(viewCount);
	for(int iView = 0; iView < viewCount; iView++)
	{
		IntPair viewFea;
		inStream >> viewFea.first;
		inStream >> viewFea.second;
		if(drewMode)
		{
			float dummy;
			inStream >> dummy;
		}
		this->viewFeaList.push_back(viewFea);
	}
}

void ScenePoint::Write(ofstream &outStream)
{
	this->pos.Write(outStream);
	this->col.Write(outStream);

	outStream << this->viewFeaList.size();
	for(uint iViewFea = 0; iViewFea < this->viewFeaList.size(); iViewFea++)
	{
		outStream << " " << this->viewFeaList[iViewFea].first << " " << this->viewFeaList[iViewFea].second;
	}
	outStream << "\n";
}

void ScenePoints::Write(const char *fileName)
{
	ENSURE(this->size() > 0);

	ofstream outStream(fileName);
	ENSURE(outStream.is_open());
	
	outStream << this->size() << "\n";
	for(uint iPoint = 0; iPoint < this->size(); iPoint++)
	{
		this->at(iPoint).Write(outStream);
	}

	outStream.close();
}

void ScenePoints::Read(const char *fileName)
{
	ENSURE(this->size() == 0);

	ifstream inStream(fileName);
	ENSURE(inStream.is_open());

	int pointCount = -1;
	inStream >> pointCount;
	ENSURE(pointCount != -1);
	this->resize(pointCount);
	for(int iPoint = 0; iPoint < pointCount; iPoint++)
	{
		this->at(iPoint).Read(inStream, false);
	}

	inStream.close();
}

void ScenePoints::WritePly(string plyFN,
						   vector<CVector3> cameraLocs,
						   bool drawCameraLocs,
						   CVector3 pcLowerBound,
						   CVector3 pcUpperBound,
						   bool drawBounds,
						   const vector<double> &frontoPlanes,
						   bool drawFrontoPlanes)
{
	ENSURE(this->size() > 0);

	ofstream outStream(plyFN.c_str());
	ENSURE(outStream.is_open());	

	const int axisDensity = 10;

	int totalPointCloud = (int) this->size();
	//int totalPointCloud = 0;

	if(drawBounds)
		totalPointCloud += 6 * SQR(axisDensity + 1);

	if(drawFrontoPlanes)
		totalPointCloud += (int) (frontoPlanes.size() * SQR(axisDensity + 1));

	if(drawCameraLocs)
		totalPointCloud += (int) cameraLocs.size();

	outStream << "ply\n";
	outStream << "format ascii 1.0\n";
	outStream << "element vertex " << totalPointCloud << "\n";
	outStream << "property float x\n";
	outStream << "property float y\n";
	outStream << "property float z\n";
	outStream << "property uchar diffuse_red\n";
	outStream << "property uchar diffuse_green\n";
	outStream << "property uchar diffuse_blue\n";
	outStream << "end_header\n";
	
	for(uint iPoint = 0; iPoint < this->size(); iPoint++)
	{
		int red   = (int) this->at(iPoint).col[0];
		int green = (int) this->at(iPoint).col[1];
		int blue  = (int) this->at(iPoint).col[2];

		outStream << (this->at(iPoint).pos[0]) << " " <<
					 (this->at(iPoint).pos[1]) << " " <<
					 //Scanalyze uses right handed coordinate sys
					 //We are left handed
					 (-this->at(iPoint).pos[2]) << " " <<    
					 red << " " << green << " " << blue << "\n";
	}

	if(drawBounds)
	{
		for(int iFace = 0; iFace < 6; iFace++)
		{
			CVector3 boxLL, boxLR, boxUL, boxUR; 
			int red, green, blue;

			switch(iFace)
			{
			case 0:
				boxLL = CVector3(pcLowerBound[0], pcLowerBound[1], pcLowerBound[2]);
				boxLR = CVector3(pcUpperBound[0], pcLowerBound[1], pcLowerBound[2]);
				boxUL = CVector3(pcLowerBound[0], pcUpperBound[1], pcLowerBound[2]);
				boxUR = CVector3(pcUpperBound[0], pcUpperBound[1], pcLowerBound[2]);
				red = 255; green = 0; blue = 0;
				break;

			case 1:
				boxLL = CVector3(pcLowerBound[0], pcLowerBound[1], pcUpperBound[2]);
				boxLR = CVector3(pcUpperBound[0], pcLowerBound[1], pcUpperBound[2]);
				boxUL = CVector3(pcLowerBound[0], pcUpperBound[1], pcUpperBound[2]);
				boxUR = CVector3(pcUpperBound[0], pcUpperBound[1], pcUpperBound[2]);
				red = 0; green = 255; blue = 0;
				break;

			case 2:
				boxLL = CVector3(pcLowerBound[0], pcLowerBound[1], pcLowerBound[2]);
				boxLR = CVector3(pcLowerBound[0], pcLowerBound[1], pcUpperBound[2]);
				boxUL = CVector3(pcLowerBound[0], pcUpperBound[1], pcLowerBound[2]);
				boxUR = CVector3(pcLowerBound[0], pcUpperBound[1], pcUpperBound[2]);
				red = 0; green = 0; blue = 255;
				break;

			case 3:
				boxLL = CVector3(pcUpperBound[0], pcLowerBound[1], pcLowerBound[2]);
				boxLR = CVector3(pcUpperBound[0], pcLowerBound[1], pcUpperBound[2]);
				boxUL = CVector3(pcUpperBound[0], pcUpperBound[1], pcLowerBound[2]);
				boxUR = CVector3(pcUpperBound[0], pcUpperBound[1], pcUpperBound[2]);
				red = 255; green = 255; blue = 0;
				break;

			case 4:
				boxLL = CVector3(pcLowerBound[0], pcLowerBound[1], pcLowerBound[2]);
				boxLR = CVector3(pcUpperBound[0], pcLowerBound[1], pcLowerBound[2]);
				boxUL = CVector3(pcLowerBound[0], pcLowerBound[1], pcUpperBound[2]);
				boxUR = CVector3(pcUpperBound[0], pcLowerBound[1], pcUpperBound[2]);
				red = 255; green = 0; blue = 255;
				break;

			case 5:
				boxLL = CVector3(pcLowerBound[0], pcUpperBound[1], pcLowerBound[2]);
				boxLR = CVector3(pcUpperBound[0], pcUpperBound[1], pcLowerBound[2]);
				boxUL = CVector3(pcLowerBound[0], pcUpperBound[1], pcUpperBound[2]);
				boxUR = CVector3(pcUpperBound[0], pcUpperBound[1], pcUpperBound[2]);
				red = 0; green = 255; blue = 255;
				break;

			default:
				REPORT_FAILURE("Unknown face type - %i.\n", iFace);
				break;
			}
		
			for(int iAxisPoint = 0; iAxisPoint <= axisDensity; iAxisPoint++)
			{
				CVector3 upperPoint = boxLL + ((boxLR - boxLL) * (iAxisPoint / (float) axisDensity));
				CVector3 lowerPoint = boxUL + ((boxUR - boxUL) * (iAxisPoint / (float) axisDensity));

				for(int jAxisPoint = 0; jAxisPoint <= axisDensity; jAxisPoint++)
				{
					CVector3 planePoint = lowerPoint + ((upperPoint - lowerPoint) * (jAxisPoint / (float) axisDensity));
					outStream << planePoint[0] << " " << 
								 planePoint[1] << " " << 
								-planePoint[2] << " " << 
								 red << " " << green << " " << blue << "\n";
				}
			}
		}		
	}
	
	if(drawFrontoPlanes)
	{
		for(uint iFrontoPlane = 0; iFrontoPlane < frontoPlanes.size(); iFrontoPlane++)
		{
			CVector3 boxLL = CVector3(pcLowerBound[0], pcLowerBound[1], frontoPlanes[iFrontoPlane]);
			CVector3 boxLR = CVector3(pcUpperBound[0], pcLowerBound[1], frontoPlanes[iFrontoPlane]);
			CVector3 boxUL = CVector3(pcLowerBound[0], pcUpperBound[1], frontoPlanes[iFrontoPlane]);
			CVector3 boxUR = CVector3(pcUpperBound[0], pcUpperBound[1], frontoPlanes[iFrontoPlane]);
			int red   = (int) (55 + (200.0f * iFrontoPlane / frontoPlanes.size())); 
			int green = red;
			int blue  = red;
		
			for(int iAxisPoint = 0; iAxisPoint <= axisDensity; iAxisPoint++)
			{
				CVector3 upperPoint = boxLL + ((boxLR - boxLL) * (iAxisPoint / (float) axisDensity));
				CVector3 lowerPoint = boxUL + ((boxUR - boxUL) * (iAxisPoint / (float) axisDensity));

				for(int jAxisPoint = 0; jAxisPoint <= axisDensity; jAxisPoint++)
				{
					CVector3 planePoint = lowerPoint + ((upperPoint - lowerPoint) * (jAxisPoint / (float) axisDensity));
					outStream << planePoint[0] << " " << 
								 planePoint[1] << " " << 
								-planePoint[2] << " " << 
								 red << " " << green << " " << blue << "\n";
				}
			}
		}		
	}


	if(drawCameraLocs)
	{
		for(uint iCam = 0; iCam < cameraLocs.size(); iCam++)
		{
			int red   = 255; 
			int green = 0;
			int blue  = 0;

			outStream << cameraLocs[iCam][0] << " " << 
						 cameraLocs[iCam][1] << " " << 
						-cameraLocs[iCam][2] << " " << 
						 red << " " << green << " " << blue << "\n";
		}
	}

	outStream.close();
}



ScenePoints ScenePoints::Prune(double maxDevFromMedian) const
{
	INSIST(this->size() > 0);
	ENSURE(maxDevFromMedian >= 0);

	ScenePoints prunedScenePts = *this;

	vector<double> colData;
	colData.resize(prunedScenePts.size());	

	for(int iCol = 0; iCol < 3; iCol++)
	{
		for(uint iPoint = 0; iPoint < prunedScenePts.size(); iPoint++)
		{
			colData[iPoint] = prunedScenePts[iPoint].pos[iCol];
		}
		std::sort(colData.begin(), colData.end());

		double median = colData[(colData.size() / 2)];
		double stdDev = 0;
		for(uint iPoint = 0; iPoint < prunedScenePts.size(); iPoint++)
		{
			double diff = prunedScenePts[iPoint].pos[iCol] - median;
			stdDev += (diff * diff);
		}
		stdDev /= prunedScenePts.size();
		stdDev = sqrt(stdDev);

		double errThres = stdDev * maxDevFromMedian;
		
		for(int iPoint = 0; iPoint < (int) prunedScenePts.size(); iPoint++)
		{
			double dist = prunedScenePts[iPoint].pos[iCol] - median;
			if((dist < -errThres) || 
			   (dist >  errThres))
			{
				prunedScenePts.erase(prunedScenePts.begin() + iPoint,
				 					 prunedScenePts.begin() + iPoint + 1);
				iPoint--;
			}
		}
	}

	return prunedScenePts;
}

ScenePoints ScenePoints::Prune(CVector3 pcLowerBound, CVector3 pcUpperBound) const
{
	ScenePoints prunedScenePts;

	for(ScenePoints::const_iterator currPoint = this->begin();
		currPoint != this->end();
		currPoint++)
	{
		CVector3 realWorldPos = currPoint->pos;

		if(InRange(realWorldPos[0], pcLowerBound[0], pcUpperBound[0]) &&
           InRange(realWorldPos[1], pcLowerBound[1], pcUpperBound[1]) &&
		   InRange(realWorldPos[2], pcLowerBound[2], pcUpperBound[2]))
		{
			prunedScenePts.push_back(*currPoint);
		}
	}	

	return prunedScenePts;
}


ScenePoints ScenePoints::Prune(int minViewCount) const
{
	INSIST(this->size() > 0);

	ScenePoints prunedScenePts;

	for(int iPoint = 0; iPoint < (int) this->size(); iPoint++)
	{	
		if((int) this->at(iPoint).viewFeaList.size() >= minViewCount)
		{
			prunedScenePts.push_back(this->at(iPoint));
		}
	}

	return prunedScenePts;
}

ScenePoints ScenePoints::Prune(CVector3 col, float maxDist) const
{
	INSIST(this->size() > 0);

	ScenePoints prunedScenePts;

	for(int iPoint = 0; iPoint < (int) this->size(); iPoint++)
	{	
		if((this->at(iPoint).col - col).Length() <= maxDist)
		{
			prunedScenePts.push_back(this->at(iPoint));
		}
	}

	return prunedScenePts;
}


void ScenePoints::GenerateViewPoints(vector<Camera> &cameras)
{
	INSIST(this->size() > 0);
	int camCount = (int) cameras.size();
	INSIST(camCount > 0);

	for(ScenePoints::const_iterator currPoint = this->begin();
		currPoint != this->end();
		currPoint++)
	{
		int viewCount = (int) currPoint->viewFeaList.size();;
		ENSURE(viewCount > 0);
		for(int iView = 0; iView < viewCount; iView++)
		{
			int camID = currPoint->viewFeaList[iView].first;
			CHECK_RANGE(camID, 0, camCount - 1);
			cameras[camID].viewPoints.push_back(currPoint->pos);
		}
	}	
}


void ScenePoints::GeneratePointCloud(PointCloud &pointCloud) const
{
	INSIST(this->size() > 0);
	INSIST(pointCloud.size() == 0);

	for(uint iPoint = 0; iPoint < this->size(); iPoint++)
	{
		Point currPoint(this->at(iPoint).pos, iPoint);
		pointCloud.push_back(currPoint);
	}
}


void ScenePoints::TransformCoordSys(const CameraMats &cameraMats)
{
	INSIST(this->size() > 0);

	for(uint iPoint = 0; iPoint < this->size(); iPoint++)
	{
		this->at(iPoint).pos = cameraMats.transformCoordSys(this->at(iPoint).pos);
	}
}


void ScenePoints::GetBounds(CVector3 &pcLowerBound, CVector3 &pcUpperBound)
{
	INSIST(this->size() > 0);

	pcLowerBound = CVector3( DBL_MAX,  DBL_MAX,  DBL_MAX);
	pcUpperBound = CVector3(-DBL_MAX, -DBL_MAX, -DBL_MAX);

	for(ScenePoints::const_iterator currPoint = this->begin();
		currPoint != this->end();
		currPoint++)
	{
		SET_IF_LT(pcLowerBound[0], currPoint->pos[0]);
		SET_IF_LT(pcLowerBound[1], currPoint->pos[1]);
		SET_IF_LT(pcLowerBound[2], currPoint->pos[2]);

		SET_IF_GT(pcUpperBound[0], currPoint->pos[0]);
		SET_IF_GT(pcUpperBound[1], currPoint->pos[1]);
		SET_IF_GT(pcUpperBound[2], currPoint->pos[2]);
	}
}


void ScenePoints::PurgeViewList()
{
	for(ScenePoints::iterator currPoint = this->begin();
		currPoint != this->end();
		currPoint++)
	{
		currPoint->viewFeaList.clear();
	}
}


ScenePoints ScenePoints::GetVisibleScenePoints(Camera* const camera) const
{
	ScenePoints culledScenePts;
	CShape viewShape = camera->GetFrameShape(0);

	for(ScenePoints::const_iterator currPoint = this->begin();
		currPoint != this->end();
		currPoint++)
	{
		CVector3 realWorldPos = currPoint->pos;
		CVector3 cameraWorldPos = camera->cameraMats.transformCoordSys(realWorldPos);

		if(cameraWorldPos[2] >= CAMERA_MinDepth)
		{
			CVector2 camUV;
			camera->cameraMats.projXYZtoUV(realWorldPos, camUV);
			if(viewShape.InBounds(camUV[0], camUV[1]))
			{
				culledScenePts.push_back(*currPoint);
			}
		}
	}	

	return culledScenePts;
}
#include "PCloudSegmenter.h"

PCloudSegmenter::PCloudSegmenter(const PointCloud &points) : scenePoints(points)
{
}

void PCloudSegmenter::SegmentPointCloud(vector<Plane> &planes, 
								        PCloudSegmenterParams params)
{
	printf("Segmenting point cloud into planes...\n");
	fflush(stdout);

	//check
	//planes.clear();	

	while((int) this->scenePoints.size() >= params.ransacParams.minInliers)
	{
		if(planes.size() >= params.maxPlanesInScene)
			break;

		vector<uint> planeInliers;		
		PlaneFromPoints planeFromPoints(this->scenePoints);

		PlaneFromPoints::PlaneFromPointsParams pfpParams = params.ransacParams;
		Plane dominantPlane = planeFromPoints.GetDominantPlane(planeInliers, pfpParams);

		if((int) planeInliers.size() < params.ransacParams.minInliers)
			break;

		for(uint iInlier = 0; iInlier < planeInliers.size(); iInlier++)
		{
			if(iInlier > 0)
			{
				ENSURE(planeInliers[iInlier] >= planeInliers[iInlier - 1]);
			}
			int inlierIndex = planeInliers[iInlier] - iInlier;
			this->scenePoints.erase(this->scenePoints.begin() + inlierIndex,
									this->scenePoints.begin() + inlierIndex + 1);
		}

		PointCloud scenePtsCpy = this->scenePoints;
		this->scenePoints.clear();
		float inlierThreshold = params.ransacParams.ransacThres * params.ransacThresMult;
		for(uint iPoint = 0; iPoint < scenePtsCpy.size(); iPoint++)
		{
			if(!PlaneFromPoints::IsInlier(dominantPlane, scenePtsCpy[iPoint].pos, inlierThreshold))
			{
				this->scenePoints.push_back(scenePtsCpy[iPoint]);
			}
		}

		printf("\tFound plane with %i inliers.\n", dominantPlane.inlierCloud.size());
		fflush(stdout);

		planes.push_back(dominantPlane);
	}
		
	if(params.createPlyFile == true)
	{
		writePlyFile(params.plyFN, planes);
		char planeSuffixBuf[1024];
		for(uint iPlane = 0; iPlane < planes.size(); iPlane++)
		{
			sprintf(planeSuffixBuf, "plane-%03i-", iPlane);
			string planeSuffixStr = planeSuffixBuf;
			writePlyFile(planeSuffixStr + params.plyFN, planes, iPlane);
		}
	}

	printf("\tDone.\n");
	fflush(stdout);
}

void PCloudSegmenter::writePlyFile(string fileName, const vector<Plane> &planes, int visPlaneIndex)
{
	ofstream outStream;
	outStream.open(fileName.c_str());
	ENSURE(outStream.is_open());
	writePlyFile(outStream, planes, visPlaneIndex);
	outStream.close();
}

void PCloudSegmenter::writePlyFile(ofstream &outStream, const vector<Plane> &planes, int visPlaneIndex)
{
	INSIST(planes.size() > 0);
	INSIST((visPlaneIndex == -1) ||
		   InRange(visPlaneIndex, 0, (int) planes.size() - 1));

	int pointCount = 0;
	for(uint iPlane = 0; iPlane < planes.size(); iPlane++)
	{
		pointCount += (int) planes[iPlane].inlierCloud.size();
	}

	outStream << "ply\n";
	outStream << "format ascii 1.0\n";
	outStream << "element vertex " << pointCount << "\n";
	outStream << "property float x\n";
	outStream << "property float y\n";
	outStream << "property float z\n";
	outStream << "property uchar diffuse_red\n";
	outStream << "property uchar diffuse_green\n";
	outStream << "property uchar diffuse_blue\n";
	outStream << "end_header\n";

	for(uint iPlane = 0; iPlane < planes.size(); iPlane++)
	{
		const PointCloud &currPlanePts = planes[iPlane].inlierCloud;

		int red = 100, green = 100, blue = 100;
		if((visPlaneIndex == -1) ||
		   (visPlaneIndex == iPlane))
		{
			red   = (int) (planes[iPlane].col[0] * 255.0);
			green = (int) (planes[iPlane].col[1] * 255.0);
			blue  = (int) (planes[iPlane].col[2] * 255.0);
		}

		for(uint iPoint = 0; iPoint < currPlanePts.size(); iPoint++)
		{
			outStream << (currPlanePts[iPoint].pos[0]) << " " <<
						 (currPlanePts[iPoint].pos[1]) << " " <<
						 //left handed to right handed conversion for scanalyze
						 (-currPlanePts[iPoint].pos[2]) << " " <<
						 red << " " << green << " " << blue << "\n";
		}
	}
}

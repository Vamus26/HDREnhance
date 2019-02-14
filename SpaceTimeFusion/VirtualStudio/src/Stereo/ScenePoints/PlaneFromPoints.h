#ifndef __PLANE_FROM_POINTS_H__
#define __PLANE_FROM_POINTS_H__

#include "Definitions.h"
#include "PointCloud.h"
#include "Plane.h"

#pragma warning(disable : 4512)

class PlaneFromPoints
{
	const PointCloud &scenePoints;
	bool *pointsPicked;

public:

	const static int MinPointsForPlane = 3;

	typedef struct _PlaneFromPointsParams
	{
		int   ransacIter;
		float ransacThres;
		int   minInliers;

		static _PlaneFromPointsParams Default()
		{
			_PlaneFromPointsParams params;
			params.ransacIter  = 5000;
			params.ransacThres = 0.0001f;
			params.minInliers  = 16;
			return params;
		}

		void Dump(const string prefix)
		{
			printf("%sPlane From Points Options:\n", prefix.c_str());		
			printf("%s  ransacIter  = %i\n", prefix.c_str(), ransacIter);
			printf("%s  ransacThres = %f\n", prefix.c_str(), ransacThres);
			printf("%s  minInliers  = %u\n", prefix.c_str(), minInliers);
			printf("\n");
		}

	} PlaneFromPointsParams;

	PlaneFromPoints(const PointCloud &points);
	~PlaneFromPoints();

	Plane GetDominantPlane(vector<uint> &planeInliers, 
						  PlaneFromPointsParams params = PlaneFromPointsParams::Default());		

	static int SolveForPlane(const PointCloud &pts, Plane &plane);

	static bool IsInlier(const Plane &plane, const CVector3 pos, double threshold);

private:
	Plane getRandomPlane();

	static int solveForPlane(float *A, int ARows, int ACols, Plane &homography);	

	void getInliers(Plane &plane, double threshold, vector<uint> &inliers);

};

#endif

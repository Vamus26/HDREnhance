#include "PlaneFromPoints.h"
#include <math.h>
#include "mkl_lapack.h"

PlaneFromPoints::PlaneFromPoints(const PointCloud &points) : scenePoints(points)
{
	ENSURE(this->scenePoints.size() >= PlaneFromPoints::MinPointsForPlane);
	this->pointsPicked = new bool[this->scenePoints.size()];
	memset(this->pointsPicked, 0, sizeof(*this->pointsPicked) * this->scenePoints.size());
}

PlaneFromPoints::~PlaneFromPoints()
{
	delete this->pointsPicked;
	this->pointsPicked = NULL;
}


Plane PlaneFromPoints::getRandomPlane()
{	
	Plane randomPlane;

	int planePointIndices[3];

	int pointsFound = 0;
	int iterations = 0;
	while(pointsFound < 3)
	{
		iterations++;
		ENSURE(iterations < 200);

		int pointIndex = (myRand() % (int) this->scenePoints.size());		
		if(this->pointsPicked[pointIndex] == false)
		{	
			planePointIndices[pointsFound] = pointIndex;
			if(pointsFound == 1)
			{
				CVector3 dirc1 = this->scenePoints[planePointIndices[0]].pos - 
							     this->scenePoints[planePointIndices[1]].pos;
				if(dirc1.Length() == 0.0) continue;
			}
			else if(pointsFound == 2)
			{
				CVector3 dirc1 = this->scenePoints[planePointIndices[0]].pos - 
							     this->scenePoints[planePointIndices[1]].pos;
				if(dirc1.Length() == 0.0) continue;

				CVector3 dirc2 = this->scenePoints[planePointIndices[0]].pos - 
							     this->scenePoints[planePointIndices[2]].pos;
				if(dirc2.Length() == 0.0) continue;

				dirc1 = dirc1.Normalize();
				dirc2 = dirc2.Normalize();
				CVector3 normal = dirc1.cross(dirc2);
				if(normal.Length() == 0.0) continue;
				normal = normal.Normalize();
				
				randomPlane.normal = normal;
				randomPlane.point  = this->scenePoints[planePointIndices[0]].pos;
				randomPlane.D      = -normal.dot(randomPlane.point);				
			}

			this->pointsPicked[pointIndex] = true;
			pointsFound++;
		}
	}

	for(int iPointPicked = 0; iPointPicked < 3; iPointPicked++)
	{
		int pointIndex = planePointIndices[iPointPicked];
		this->pointsPicked[pointIndex] = false;
	}

	return randomPlane;
}

Plane PlaneFromPoints::GetDominantPlane(vector<uint> &planeInliers, 
										PlaneFromPointsParams params)
{
	ENSURE(params.minInliers >= PlaneFromPoints::MinPointsForPlane);
	ENSURE(params.minInliers <= (int) this->scenePoints.size());
	planeInliers.clear();
	Plane dominantPlane;
	
	for(int iter = 0; iter < params.ransacIter; iter++)
	{
		Plane predictedPlane = getRandomPlane();
		vector<uint> predictedInliers;
        getInliers(predictedPlane, params.ransacThres, predictedInliers);
		ENSURE(predictedInliers.size() >= PlaneFromPoints::MinPointsForPlane);

		if(predictedInliers.size() > planeInliers.size())
		{
			dominantPlane = predictedPlane;
			planeInliers  = predictedInliers;
		}
	}

	if((int) planeInliers.size() >= params.minInliers)
	{
		int returnVal = SolveForPlane(dominantPlane.inlierCloud, dominantPlane);
		ENSURE(returnVal == 0);		
		getInliers(dominantPlane, params.ransacThres, planeInliers);		
		ENSURE(planeInliers.size() >= PlaneFromPoints::MinPointsForPlane);
	}
	else
	{
		planeInliers.clear();
	}

	return dominantPlane;
}

///*
int PlaneFromPoints::solveForPlane(float *A, int ARows, int ACols, Plane &plane)
{
	float *AT = new float[ARows * ACols];
	ENSURE(AT != NULL);
	uint atIndex = 0;
	for(int atY = 0; atY < ACols; atY++)
	{
		for(int atX = 0; atX < ARows; atX++, atIndex++)
		{
			int aIndex = (atX * ACols) + atY;
			AT[atIndex] = A[aIndex];
		}
	}

	char jobu  = 'N';
	char jobvt = 'O';
	int info;
	int m       = ARows;
	int n       = ACols;
	int lda     = m;
	int ldu     = 1;
	int ldvt    = 1;
	
	float *s    = new float[n];
	int lwork   = (5 * m) * 10;  
	float *work = new float[lwork];	
	ENSURE(s != NULL);
	ENSURE(work != NULL);
	
	sgesvd(&jobu, &jobvt, &m, &n, AT, &lda, s, NULL, &ldu, NULL, &ldvt, work, &lwork, &info);

	if(info == 0)
	{
		uint minEigIndex = ACols - 1;
		plane.normal[0]  = AT[minEigIndex];
		minEigIndex += ARows;
		plane.normal[1]  = AT[minEigIndex];
		minEigIndex += ARows;
		plane.normal[2]  = AT[minEigIndex];

		double normalLen = plane.normal.Length();
		ENSURE((normalLen > 0.99) && (normalLen < 1.01));
		plane.normal = plane.normal.Normalize();
	}

	delete s;
	delete work;
	delete AT;

	return info;
}
//*/

int PlaneFromPoints::SolveForPlane(const PointCloud &points, Plane &plane)
{
	ENSURE(points.size() >= PlaneFromPoints::MinPointsForPlane);
	
	plane.point = CVector3(0.0, 0.0, 0.0);
	for(PointCloud::const_iterator currPoint = points.begin();
		currPoint != points.end();
		currPoint++)
	{
		plane.point = plane.point + currPoint->pos;
	}
	plane.point = plane.point / points.size();

	int ARows = (int) points.size();
	int ACols = 3;
	float *A  = new float[ARows * ACols];
	ENSURE(A != NULL);
	float *ARowEntry = A;
	for(PointCloud::const_iterator currPoint = points.begin();
		currPoint != points.end();
		currPoint++)
	{
		ARowEntry[0] = (float) (currPoint->pos[0] - plane.point[0]);
		ARowEntry[1] = (float) (currPoint->pos[1] - plane.point[1]);
		ARowEntry[2] = (float) (currPoint->pos[2] - plane.point[2]);
		ARowEntry += ACols;	
	}

	int info = PlaneFromPoints::solveForPlane(A, ARows, ACols, plane);
	ENSURE(info == 0);
	plane.D = -plane.normal.dot(plane.point);

	delete A;

	return info;
}

void PlaneFromPoints::getInliers(Plane &plane, double threshold, vector<uint> &inliers)
{	
	inliers.clear();
	plane.inlierCloud.clear();
	int pointIndex = 0;
	for(PointCloud::const_iterator currPoint = this->scenePoints.begin();
		currPoint != this->scenePoints.end();
		currPoint++, pointIndex++)
	{
		if(PlaneFromPoints::IsInlier(plane, currPoint->pos, threshold))		
		{
			inliers.push_back(pointIndex);
			plane.inlierCloud.push_back(*currPoint);
		}
	}
}


bool PlaneFromPoints::IsInlier(const Plane &plane, const CVector3 pos, double threshold)
{
	double dist = fabs(plane.normal.dot(pos) + plane.D);
	if (dist <= threshold)
		return true;
	else
		return false;
}
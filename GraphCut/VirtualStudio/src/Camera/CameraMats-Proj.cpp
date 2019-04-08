#include "CameraMats.h"
#include <float.h>
#include <math.h>

void CameraMats::projXYZtoUV(const CVector3 &xyzCoord, CVector2 &uvCoord) const
{
	ENSURE(this->interMatsComputed);

	CVector4 xyzPT(xyzCoord[0], xyzCoord[1], xyzCoord[2], 1.0);	
	CVector4 projPT = this->projMat * xyzPT;	
	
	ENSURE(projPT[2] != 0.0);
	uvCoord[0] = projPT[0] / projPT[2];
	uvCoord[1] = projPT[1] / projPT[2];
}

void CameraMats::projUVZtoXYZ(const CVector3 &uvzCoord, CVector3 &xyzCoord) const
{
	ENSURE(this->interMatsComputed == true);

	CVector3 imgCoord(uvzCoord[0], uvzCoord[1], 1.0);

	// u = K*R(p+t)
	// p = R'K'u - rt
	// camera cop = -rt
	// ray dirc = p - camera cop = R'K'u
	CVector3 rayDirc = this->rotMatInv * (this->intrinsicMatInv * imgCoord);

	//cameraCoord + t*rayDirc = xyzCoord such that z = w
	//solve for t where z = w
	double t = (uvzCoord[2] - this->cameraCoord[2]) / rayDirc[2];
	ENSURE(t >= 0);

	xyzCoord = this->cameraCoord + (rayDirc * t);	
	ENSURE(fabs(xyzCoord[2] - uvzCoord[2]) <= 0.00001); //ensure w = z
}

CVector3 CameraMats::getRayDirc(const CVector2 &uvCoord) const
{
	ENSURE(this->interMatsComputed == true);
	CVector3 imgCoord(uvCoord[0], uvCoord[1], 1.0);
	CVector3 rayDirc = this->rotMatInv * (this->intrinsicMatInv * imgCoord);

	return rayDirc;
}

void CameraMats::projUVtoXYZ(const CVector2 &uvCoord, const Plane &plane, CVector3 &xyzCoord) const
{
	ENSURE(this->interMatsComputed == true);

	CVector3 imgCoord(uvCoord[0], uvCoord[1], 1.0);
	CVector3 rayDirc = this->rotMatInv * (this->intrinsicMatInv * imgCoord);
	
	//Ax + By + Cz + D = 0
	//p = cameraCoP + t.rayDirc
	//t*normal.rayDirc = -(cameraCoP.normal + D)

	double denom = plane.normal.dot(rayDirc);

	if(denom == 0)
		xyzCoord = CVector3(DBL_MAX, DBL_MAX, DBL_MAX);
	else
	{
		double numerator = -(this->cameraCoord.dot(plane.normal) + plane.D);
		double t = numerator / denom;
		xyzCoord = this->cameraCoord + (rayDirc * t);
	}
}

CVector3 CameraMats::transformCoordSys(const CVector3 &xyzCoord) const
{
	ENSURE(this->interMatsComputed == true);	
	CVector3 newXYZCoord = (this->rotMat * xyzCoord) + this->transVec;
	return newXYZCoord;
}

CVector3 CameraMats::invertCoordSys(CVector3 &xyzCoord) const
{
	ENSURE(this->interMatsComputed == true);	
	CVector3 newXYZCoord = this->rotMatInv * (xyzCoord - this->transVec);
	return newXYZCoord;
}

CTransform3x3 CameraMats::GenerateFMatrix(CameraMats neighCamMats, bool centerOrigin) const
{
	neighCamMats.Recenter(*this);

	CTransform3x3 k1 = this->intrinsicMat;
	CTransform3x3 k2 = neighCamMats.intrinsicMat;

	if(centerOrigin)
	{
		k1[0][2] = k1[1][2] = k2[0][2] = k2[1][2] = 0.0;
	}

	CTransform3x3 transCross = CTransform3x3::Null();
	transCross[0][1] = -neighCamMats.transVec[2];
	transCross[1][0] =  neighCamMats.transVec[2];
	transCross[0][2] =  neighCamMats.transVec[1];
	transCross[2][0] = -neighCamMats.transVec[1];
	transCross[1][2] = -neighCamMats.transVec[0];
	transCross[2][1] =  neighCamMats.transVec[0];

	CTransform3x3 fmatrix;
	fmatrix = neighCamMats.rotMat * k1.Inverse();
	fmatrix = transCross * fmatrix; 
	fmatrix = k2.Inverse() * fmatrix; 

	return fmatrix;
}

//equation obtained from "In defense of eigth point algorithm"
CTransform3x3 CameraMats::CenterFMatrixCoordSys(CTransform3x3 fmatrix, CShape leftShape, CShape rightShape)
{
	CTransform3x3 rightTransform = CTransform3x3::Identity();
	rightTransform[0][2] = -rightShape.width  / 2.0;
	rightTransform[1][2] = -rightShape.height / 2.0;

	CTransform3x3 leftTransform = CTransform3x3::Identity();
	leftTransform[0][2] = -leftShape.width  / 2.0;
	leftTransform[1][2] = -leftShape.height / 2.0;
	
	CTransform3x3 transformedFMatrix = fmatrix * leftTransform.Inverse();
	transformedFMatrix = rightTransform.Inverse().Transpose() * transformedFMatrix;

	return transformedFMatrix;
}

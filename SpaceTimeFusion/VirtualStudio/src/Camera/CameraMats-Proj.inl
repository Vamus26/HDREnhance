#include "Definitions.h"
#include <float.h>
#include <math.h>


inline void CameraMats::projXYZtoUV(const CVector3 &xyzCoord, CVector2 &uvCoord) const
{
	CVector4 xyzPT(xyzCoord[0], xyzCoord[1], xyzCoord[2], 1.0);
	
	ENSURE(this->interMatsComputed);
	CVector4 projPT = this->projMat * xyzPT;	
	
	ENSURE(projPT[2] != 0.0);
	uvCoord[0] = projPT[0] / projPT[2];
	uvCoord[1] = projPT[1] / projPT[2];
}


inline void CameraMats::projUVZtoXYZ(const CVector3 &uvzCoord, CVector3 &xyzCoord) const
{
	ENSURE(this->interMatsComputed == true);

	CVector3 imgCoord(uvzCoord[0], uvzCoord[1], 1.0);

	CVector3 rayDirc = this->rotMatInv * (this->intrinsicMatInv * imgCoord);

	//cameraCoord + t*rayDirc = xyzCoord such that z = w
	//solve for t where z = w
	double t = (uvzCoord[2] - this->cameraCoord[2]) / rayDirc[2];
	ENSURE(t >= 0);

	xyzCoord = this->cameraCoord + (rayDirc * t);
	ENSURE(fabs(xyzCoord[2] - uvzCoord[2]) <= 0.00001); //ensure w = z
}
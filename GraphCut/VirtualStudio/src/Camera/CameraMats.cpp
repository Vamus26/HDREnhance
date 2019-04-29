#include "CameraMats.h"


CameraMats::CameraMats()
{
	this->intrinsicMat     = CTransform3x3::Null();	
	this->rotMat           = CTransform3x3::Null();
	this->projMat          = CTransform4x4::Null();
	this->transVec         = CVector3(0, 0, 0);
	this->k1 = this->k2    = 0.0;	
	this->cameraMatsLoaded = false;

	this->interMatsComputed = false;
	this->intrinsicMatInv   = CTransform3x3::Null();
	this->projMat           = CTransform4x4::Null();
	this->projMatInv        = CTransform4x4::Null();
}

CTransform4x4 CameraMats::GetExtrinsicMat() const
{
	ENSURE(this->cameraMatsLoaded == true);
	CTransform4x4 exMat = CTransform4x4::Identity();
	for(int iRow = 0; iRow < 3; iRow++)
	{
		for(int iCol = 0; iCol < 3; iCol++)
		{
			exMat[iRow][iCol] = this->rotMat[iRow][iCol];
		}
		exMat[iRow][3] = this->transVec[iRow];
	}

	return exMat;
}

CTransform4x4 CameraMats::GetProjMat() const
{
	ENSURE(this->interMatsComputed == true);
	return this->projMat;
}

CTransform4x4 CameraMats::GetProjMatInv() const
{
	ENSURE(this->interMatsComputed == true);
	return this->projMatInv;
}

CTransform3x3 CameraMats::GetIntrinsicMatInv() const
{
	ENSURE(this->interMatsComputed == true);
	return this->intrinsicMatInv;
}

CTransform3x3 CameraMats::GetRotMatInv() const
{
	ENSURE(this->interMatsComputed == true);
	return this->rotMatInv;
}

CVector3 CameraMats::GetWorldCoordCoP() const
{
	ENSURE(this->interMatsComputed == true);
	return this->cameraCoord;
}

void CameraMats::ComputeInterMats()
{
	CTransform4x4 exMat = GetExtrinsicMat();
	CTransform4x4 inMat = CTransform4x4::Identity();
	for(int iRow = 0; iRow < 3; iRow++)
	{
		for(int iCol = 0; iCol < 3; iCol++)
		{		
			inMat[iRow][iCol] = this->intrinsicMat[iRow][iCol];
		}
	}

	this->projMat         = inMat * exMat;
	this->projMatInv      = this->projMat.Inverse();
	this->intrinsicMatInv = this->intrinsicMat.Inverse();
	this->rotMatInv       = this->rotMat.Inverse();
	this->cameraCoord     = (this->rotMatInv * this->transVec) * -1.0;
	this->interMatsComputed = true;
}

void CameraMats::Recenter(const CameraMats refCamMats)
{
	this->interMatsComputed = false;

	CTransform4x4 refExtrinsicMatInv = refCamMats.GetExtrinsicMat().Inverse();
	CTransform4x4 currExtrinsicMat   = GetExtrinsicMat();
	CTransform4x4 newExtrinsicMat    = currExtrinsicMat * refExtrinsicMatInv;
	for(int iRow = 0; iRow < 3; iRow++)
	{
		for(int iCol = 0; iCol < 3; iCol++)
		{
			this->rotMat[iRow][iCol] = newExtrinsicMat[iRow][iCol];
		}
		this->transVec[iRow] = newExtrinsicMat[iRow][3];
	}

	ComputeInterMats();	
}

void CameraMats::ScaleMats(float scale)
{
	ENSURE(scale > 0.0f);

	this->interMatsComputed = false;

	this->intrinsicMat[0][0] *= scale;
	this->intrinsicMat[1][1] *= scale;

	this->intrinsicMat[0][2] *= scale;
	this->intrinsicMat[1][2] *= scale;

	ComputeInterMats();	
}


void CameraMats::CropMats(double originX, double originY, double width, double height)
{
	INSIST(this->interMatsComputed == true);

	this->interMatsComputed = false;
	
	double xSpan = this->intrinsicMat[0][2] * 2.0f;
	double ySpan = this->intrinsicMat[1][2] * 2.0f;
	INSIST(xSpan > 0.0); INSIST(ySpan > 0.0);

	INSIST(originX <= xSpan - 1.0f);
	INSIST(originY <= ySpan - 1.0f);

	INSIST(originX + width  - 1.0f <= xSpan - 1.0f);
	INSIST(originY + height - 1.0f <= ySpan - 1.0f);

	this->intrinsicMat[0][2] = this->intrinsicMat[0][2] - originX;
	this->intrinsicMat[1][2] = this->intrinsicMat[1][2] - originY;

	ComputeInterMats();	
}

CameraMats CameraMats::operator+(const CameraMats &cm) const
{
	CameraMats newCameraMats = cm;
	newCameraMats.interMatsComputed = false;

	newCameraMats.k1 += this->k1;
	newCameraMats.k2 += this->k2;

	newCameraMats.rotMat       = newCameraMats.rotMat       + this->rotMat;
	newCameraMats.intrinsicMat = newCameraMats.intrinsicMat + this->intrinsicMat;
	newCameraMats.transVec     = newCameraMats.transVec     + this->transVec;

	newCameraMats.ComputeInterMats();

	return newCameraMats;
}

CameraMats CameraMats::operator/(double div) const
{
	ENSURE(div != 0.0);

	CameraMats newCameraMats = *this;
	newCameraMats.interMatsComputed = false;

	newCameraMats.k1 /= div;
	newCameraMats.k2 /= div;
						
	newCameraMats.rotMat       = newCameraMats.rotMat       * (1.0 / div);
	newCameraMats.intrinsicMat = newCameraMats.intrinsicMat * (1.0 / div);
	newCameraMats.transVec     = newCameraMats.transVec     * (1.0 / div);

	newCameraMats.ComputeInterMats();

	return newCameraMats;
}


void CameraMats::InvertAxisY()
{
	this->interMatsComputed = false;

	CTransform3x3 yInvert = CTransform3x3::Identity();

	yInvert[1][1] = -1;
	this->rotMat = yInvert * this->rotMat * yInvert;
	this->transVec[1] *= -1.0;

	ComputeInterMats();
}
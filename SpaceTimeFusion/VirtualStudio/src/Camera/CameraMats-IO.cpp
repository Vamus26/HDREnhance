#include "CameraMats.h"

void CameraMats::ReadCameraNoahDrew(ifstream &inStream, int imgWidth, int imgHeight, bool drewMode)
{
    ENSURE((imgWidth > 0) && (imgHeight > 0));

	this->k1 = this->k2 = 0.0;

	this->intrinsicMat = CTransform3x3::Null();
	inStream >> this->intrinsicMat[0][0];

	if(drewMode)
	{
		this->intrinsicMat[0][0] *= max(imgWidth, imgHeight);
	}

	this->intrinsicMat[1][1] = this->intrinsicMat[0][0];
	this->intrinsicMat[2][2] = 1.0;
	this->intrinsicMat[0][2] = imgWidth / 2.0;
	this->intrinsicMat[1][2] = imgHeight / 2.0;

	ReadExtrinsicParams(inStream);
	
	if(this->intrinsicMat[0][0] != 0.0)
	{
		this->cameraMatsLoaded = true;
		CheckRotMat();
		ComputeInterMats();
	}
	else
	{
		this->interMatsComputed = false;
		this->cameraMatsLoaded = false;
	}
}


void CameraMats::ReadCameraLarry(ifstream &inStream)
{
	int camID = -1;
	inStream >> camID;
	ENSURE(camID >= 0);

	this->intrinsicMat = CTransform3x3::Null();
	this->intrinsicMat.Read(inStream);
	ENSURE(!(this->intrinsicMat == CTransform3x3::Null()));

	ReadDistortion(inStream);

	this->rotMat = CTransform3x3::Null();
	this->transVec = CVector3(DBL_MAX, DBL_MAX, DBL_MAX);

	inStream >> this->rotMat[0][0];
	inStream >> this->rotMat[0][1];
	inStream >> this->rotMat[0][2];
	inStream >> this->transVec[0];

	inStream >> this->rotMat[1][0];
	inStream >> this->rotMat[1][1];
	inStream >> this->rotMat[1][2];
	inStream >> this->transVec[1];

	inStream >> this->rotMat[2][0];
	inStream >> this->rotMat[2][1];
	inStream >> this->rotMat[2][2];
	inStream >> this->transVec[2];

	ENSURE(!(this->rotMat == CTransform3x3::Null()));
	ENSURE(this->transVec[0] != DBL_MAX);
	ENSURE(this->transVec[1] != DBL_MAX);
	ENSURE(this->transVec[2] != DBL_MAX);
	
	this->cameraMatsLoaded = true;

	CheckRotMat();

	ComputeInterMats();
}

void CameraMats::ReadCamera(ifstream &inStream)
{
	ReadDistortion(inStream);
	
	this->intrinsicMat == CTransform3x3::Null();
	this->intrinsicMat.Read(inStream);
	ENSURE(!(this->intrinsicMat == CTransform3x3::Null()));

	ReadExtrinsicParams(inStream);

	this->cameraMatsLoaded = true;

	CheckRotMat();

	ComputeInterMats();
}

void CameraMats::ReadCamera(string filename)
{
	ifstream inStream(filename.c_str());
	ENSURE(inStream.is_open());
	ReadCamera(inStream);
	inStream.close();	
}

void CameraMats::ReadExtrinsicParams(ifstream &inStream)
{
	this->rotMat == CTransform3x3::Null();
	this->rotMat.Read(inStream);
	ENSURE(!(this->rotMat == CTransform3x3::Null()));

	CVector3 maxTrans(DBL_MAX, DBL_MAX, DBL_MAX);
	this->transVec = maxTrans;
	this->transVec.Read(inStream);
	ENSURE(this->transVec[0] != maxTrans[0]);
	ENSURE(this->transVec[1] != maxTrans[1]);
	ENSURE(this->transVec[1] != maxTrans[2]);
}

void CameraMats::ReadIntrinsicParamsZhang(ifstream &inStream)
{
	double fc1 = 0.0, fc2 = 0.0, alpha_c, cc1, cc2;

	inStream >> fc1;	
	inStream >> alpha_c;
	inStream >> fc2;
	inStream >> cc1;
	inStream >> cc2;

	ENSURE(fc1 != 0.0);
	ENSURE(fc2 != 0.0);

	ReadDistortion(inStream);

	this->intrinsicMat[0][0] = fc1;
	this->intrinsicMat[0][1] = alpha_c; //bug - is this correct?
	this->intrinsicMat[0][2] = cc1;

	this->intrinsicMat[1][0] = 0.0;
	this->intrinsicMat[1][1] = fc2;
	this->intrinsicMat[1][2] = cc2;

	this->intrinsicMat[2][0] = 0.0;
	this->intrinsicMat[2][1] = 0.0;
	this->intrinsicMat[2][2] = 1.0;

	this->intrinsicMatInv = this->intrinsicMat.Inverse();
}


void CameraMats::ReadDistortion(ifstream &inStream)
{
	this->k1 = this->k2 = DBL_MAX;
	inStream >> this->k1;
	inStream >> this->k2;
	ENSURE(this->k1 != DBL_MAX);
	ENSURE(this->k2 != DBL_MAX);
}

void CameraMats::WriteCamera(ofstream &outStream)
{
	outStream << this->k1 << " " << this->k2 << "\n";
	this->intrinsicMat.Write(outStream);
	this->rotMat.Write(outStream);	
	this->transVec.Write(outStream);
}

void CameraMats::WriteCamera(string fileName)
{
	ofstream outStream(fileName.c_str());
	ENSURE(outStream.is_open());
	WriteCamera(outStream);
	outStream.close();
}


void CameraMats::Dump() const
{
	ENSURE(this->cameraMatsLoaded == true);
	
	printf("Radial distortion - [%ld, %ld]\n", this->k1, this->k2);
	printf("\n");

	printf("Intrinsic Matrix\n");
	this->intrinsicMat.Dump();
	printf("\n");

	printf("Rotation Matrix\n");
	this->rotMat.Dump();
	printf("\n");

	printf("Translation Vector\n");
	this->transVec.Dump();
	printf("\n");

	ENSURE(this->interMatsComputed  == true);
	printf("Projection Matrix\n");
	this->projMat.Dump();
	printf("\n");
}


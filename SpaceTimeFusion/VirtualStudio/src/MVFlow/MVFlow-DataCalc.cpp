#include "MVFlow.h"
#include "ImageIO.h"

void MVFlow::resetUV()
{
	this->u.ClearPixels();
	this->v.ClearPixels();
}

void MVFlow::reset_dU_dV()
{
	this->du.ClearPixels();
	this->dv.ClearPixels();
}

void MVFlow::updateUV()
{
	ImageProcessing::Operate(this->u, this->u, this->du, ImageProcessing::OP_ADD);
	ImageProcessing::Operate(this->v, this->v, this->dv, ImageProcessing::OP_ADD);
	reset_dU_dV();
}

void MVFlow::initCurrImgDXY()
{
	computeGradImgs(this->currImg, this->currImgDX, this->currImgDY);
}

void MVFlow::computeDataIterK()
{
	float mean, dev;
	ImageProcessing::GetMeanAndDev(this->du, 0, mean, dev);
	ENSURE(mean == 0.0f);
	ImageProcessing::GetMeanAndDev(this->dv, 0, mean, dev);
	ENSURE(mean == 0.0f);

	computeWarpImg();
	computeGradImgs(this->warpImg, this->Ix, this->Iy);
	ImageProcessing::Operate(this->Iz, this->warpImg, this->currImg, ImageProcessing::OP_SUB);

	computeGradImgs(this->Ix, this->Ixx, this->Ixy);
	computeGradImgs(this->Iy, this->Ixy, this->Iyy);
	ImageProcessing::Operate(this->Ixz, this->Ix, this->currImgDX, ImageProcessing::OP_SUB);
	ImageProcessing::Operate(this->Iyz, this->Iy, this->currImgDY, ImageProcessing::OP_SUB);

	computeTensors();
}

void MVFlow::computeWarpImg()
{
	CShape gridShape = this->currImg.Shape();
	float xCoordLimit = (float) gridShape.width  - 1;
	float yCoordLimit = (float) gridShape.height - 1;

	uint nodeAddr = 0;
	for(float y = 0.0f; y < gridShape.height; y++)
	for(float x = 0.0f; x < gridShape.width;  x++, nodeAddr++)
	{
		ENSURE(fabs(this->u[nodeAddr]) < xCoordLimit);
		ENSURE(fabs(this->v[nodeAddr]) < yCoordLimit);

		float warpX = x + this->u[nodeAddr];
		float warpY = y + this->v[nodeAddr];
		warpX = reflectCoord(warpX, xCoordLimit);
		warpY = reflectCoord(warpY, yCoordLimit);

		this->warpImg[nodeAddr] = ImageProcessing::BilinearIterpolation(this->nextImg, warpX, warpY, 0, false);
	}
}

void MVFlow::computeGradImgs(const CFloatImage &srcImg, 
							 CFloatImage &gradX,
							 CFloatImage &gradY)
{
	CShape gridShape = srcImg.Shape();
	ENSURE(gridShape == gradX.Shape());
	ENSURE(gridShape == gradY.Shape());

	int xCoordLimit = gridShape.width  - 1;
	int yCoordLimit = gridShape.height - 1;	
	
	const int kernelSize               = 4;
	const int kernelOffset[kernelSize] = {-2, -1, 1, 2};

	//bug
	//float kernelDenomX                = 12.0f * gridShape.width;
	float kernelDenomX              = 12.0f;
	float kernelWeightX[kernelSize] = { 1.0f / kernelDenomX, 
									   -8.0f / kernelDenomX, 
									    8.0f / kernelDenomX,
									   -1.0f / kernelDenomX };

	//bug
	//float kernelDenomY                = 12.0f * gridShape.height;
	float kernelDenomY                = 12.0f;
	float kernelWeightY[kernelSize] = { 1.0f / kernelDenomY, 
									   -8.0f / kernelDenomY, 
									    8.0f / kernelDenomY,
									   -1.0f / kernelDenomY };

	uint nodeAddr = 0;
	for(int y = 0; y < gridShape.height; y++)
	for(int x = 0; x < gridShape.width;  x++, nodeAddr++)
	{
		float gradValX = 0.0f;
		float gradValY = 0.0f;

		for(int iKernel = 0; iKernel < kernelSize; iKernel++)
		{
			int kernelX = x + kernelOffset[iKernel];
			kernelX = reflectCoord(kernelX, xCoordLimit);
			gradValX += srcImg.Pixel(kernelX, y, 0) * kernelWeightX[iKernel];

			int kernelY = y + kernelOffset[iKernel];
			kernelY = reflectCoord(kernelY, yCoordLimit);
			gradValY += srcImg.Pixel(x, kernelY, 0) * kernelWeightY[iKernel];
		}

		gradX[nodeAddr] = gradValX;
		gradY[nodeAddr] = gradValY;
	}
}

void MVFlow::computeTensors()
{
	CShape gridShape = this->currImg.Shape();

	uint nodeAddr = 0;
	for(int y = 0; y < gridShape.height; y++)
	for(int x = 0; x < gridShape.width;  x++, nodeAddr++)
	{
		Vector3<float> deltaI(this->Ix[nodeAddr],
					  	      this->Iy[nodeAddr],
						      this->Iz[nodeAddr]);
		this->tensorS[nodeAddr] = deltaI * deltaI;

		Vector3<float> deltaIx(this->Ixx[nodeAddr],
					  	       this->Ixy[nodeAddr],
						       this->Ixz[nodeAddr]);	
		Vector3<float> deltaIy(this->Ixy[nodeAddr],
					  	       this->Iyy[nodeAddr],
						       this->Iyz[nodeAddr]);
		this->tensorT[nodeAddr] = (deltaIx * deltaIx) + (deltaIy * deltaIy);
	}
}


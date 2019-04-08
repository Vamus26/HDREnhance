#include "MathHelper.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

void MathHelper::NormalizeImageIntegral(CFloatImage &image)
{
	CShape imgShape  = image.Shape();
	ENSURE(imgShape.depth == 1);

	float imageIntegral = 0;

	uint pixelIndex = 0;
	for(int imageY = 0; imageY < imgShape.height; imageY++)
	{
		for(int imageX = 0; imageX < imgShape.width; imageX++)
		{
			for(int imageB = 0; imageB < imgShape.nBands; imageB++, pixelIndex++)
			{	
				imageIntegral += image[pixelIndex];
			}
		}
	}

	ENSURE(imageIntegral != 0);

	pixelIndex = 0;
	for(int imageY = 0; imageY < imgShape.height; imageY++)
	{
		for(int imageX = 0; imageX < imgShape.width; imageX++)
		{
			for(int imageB = 0; imageB < imgShape.nBands; imageB++, pixelIndex++)
			{	
				image[pixelIndex] /= imageIntegral;
			}
		}
	}
}

CFloatImage MathHelper::CreateGuassianKernel2D(int kernelSize, float stdDev)
{
	ENSURE(kernelSize > 0);
	ENSURE(kernelSize % 2 == 1);

	float variance = stdDev * stdDev;

	CShape kernelShape(kernelSize, kernelSize, 1);
	CFloatImage kernel(kernelShape);

	int kernelOffset = (kernelSize / 2);
	kernel.origin[0] = -kernelOffset;
	kernel.origin[1] = -kernelOffset;

	float normalizer = (float) (2 * M_PI * variance);
	for(int kernelY = -kernelOffset, y = 0; kernelY <= kernelOffset; kernelY++, y++)
	{
		for(int kernelX = -kernelOffset, x = 0; kernelX <= kernelOffset; kernelX++, x++)
		{
			float exponent = ((kernelX * kernelX) + (kernelY * kernelY)) / (-2.0f * variance);
			kernel.Pixel(x, y, 0) = exp(exponent) / normalizer;
		}
	}
	MathHelper::NormalizeImageIntegral(kernel);
	return kernel;
}

CFloatImage MathHelper::CreateGuassianKernel1D(Axis axis, int kernelSize, float stdDev)
{
	ENSURE(kernelSize > 0);
	ENSURE(kernelSize % 2 != 0);
	
	float variance = stdDev * stdDev;

	int kernelSizeX = 0;
	int kernelSizeY = 0;

	switch(axis)
	{
	case X_AXIS:
		kernelSizeX = kernelSize;
		kernelSizeY = 1;
		break;
	case Y_AXIS:
		kernelSizeY = kernelSize;
		kernelSizeX = 1;
		break;
	default:
		REPORT_FAILURE("Unknown axis");
		break;
	}

	CShape kernelShape(kernelSizeX, kernelSizeY, 1);
	CFloatImage kernel(kernelShape);

	int kernelOffsetX = (kernelSizeX / 2);
	int kernelOffsetY = (kernelSizeY / 2);
	kernel.origin[0] = -kernelOffsetX;
	kernel.origin[1] = -kernelOffsetY;

	float normalizer = (float) sqrt(2 * M_PI * variance);
	for(int kernelY = -kernelOffsetY, y = 0; kernelY <= kernelOffsetY; kernelY++, y++)
	{
		for(int kernelX = -kernelOffsetX, x = 0; kernelX <= kernelOffsetX; kernelX++, x++)
		{
			float exponent = ((kernelX * kernelX) + (kernelY * kernelY)) / (-2.0f * variance);
			kernel.Pixel(x, y, 0) = exp(exponent) / normalizer;
		}
	}
	MathHelper::NormalizeImageIntegral(kernel);
	return kernel;
}



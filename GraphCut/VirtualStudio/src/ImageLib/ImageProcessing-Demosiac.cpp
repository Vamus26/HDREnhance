#include "ImageProcessing.h"
#include "ImageIO.h"

#define CHANNEL_RED   0
#define CHANNEL_GREEN 1
#define CHANNEL_BLUE  2


float ImageProcessing::getBayerRespose(CFloatImage &bayerImg, 
									   CFloatImage &kernel, 
									   int x, int y)
{
	CShape kernelShape = kernel.Shape();
	CShape bayerShape  = bayerImg.Shape();
	ENSURE(kernelShape.nBands == bayerShape.nBands == 1);

	float sampleSum = 0;
	for(int kernelY = 0; kernelY < kernelShape.height; kernelY++)
	{
		for(int kernelX = 0; kernelX < kernelShape.width; kernelX++)
		{
			int sampleX  = x + (kernelX + kernel.origin[0]);
			int sampleY  = y + (kernelY + kernel.origin[1]);
			if(bayerShape.InBounds(sampleX, sampleY))
			{
				float sample = bayerImg.Pixel(sampleX, sampleY, 0);
				float weight = kernel.Pixel(kernelX, kernelY, 0);

				sampleSum += (sample * weight);
			}
		}
	}

	return sampleSum;
}



void ImageProcessing::setGreenKernel(CFloatImage &gKernel)
{
	CShape kernelShape(5, 5, 1);
	gKernel.ReAllocate(kernelShape);
	gKernel.origin[0] = gKernel.origin[1] = -2;
	gKernel.ClearPixels();
	
	gKernel.Pixel(2, 0, 0) = -1.0f / 8.0f;
	gKernel.Pixel(2, 1, 0) =  2.0f / 8.0f;
	gKernel.Pixel(2, 2, 0) =  4.0f / 8.0f;
	gKernel.Pixel(2, 3, 0) =  2.0f / 8.0f;
	gKernel.Pixel(2, 4, 0) = -1.0f / 8.0f;

	gKernel.Pixel(0, 2, 0) = -1.0f / 8.0f;
	gKernel.Pixel(1, 2, 0) =  2.0f / 8.0f;
	gKernel.Pixel(3, 2, 0) =  2.0f / 8.0f;
	gKernel.Pixel(4, 2, 0) = -1.0f / 8.0f;
}

void ImageProcessing::setRB_Kernel1(CFloatImage &rbKernel1)
{
	CShape kernelShape(5, 5, 1);
	rbKernel1.ReAllocate(kernelShape);
	rbKernel1.origin[0] = rbKernel1.origin[1] = -2;
	rbKernel1.ClearPixels();

	rbKernel1.Pixel(0, 2, 0) = -1.0f / 8.0f;
	rbKernel1.Pixel(1, 2, 0) =  4.0f / 8.0f;
	rbKernel1.Pixel(2, 2, 0) =  5.0f / 8.0f;
	rbKernel1.Pixel(3, 2, 0) =  4.0f / 8.0f;
	rbKernel1.Pixel(4, 2, 0) = -1.0f / 8.0f;

	rbKernel1.Pixel(1, 1, 0) = -1.0f / 8.0f;
	rbKernel1.Pixel(1, 3, 0) = -1.0f / 8.0f;

	rbKernel1.Pixel(3, 1, 0) = -1.0f / 8.0f;
	rbKernel1.Pixel(3, 3, 0) = -1.0f / 8.0f;

	rbKernel1.Pixel(2, 0, 0) =  0.5f / 8.0f;
	rbKernel1.Pixel(2, 4, 0) =  0.5f / 8.0f;
}

void ImageProcessing::setRB_Kernel2(CFloatImage &rbKernel2)
{
	CShape kernelShape(5, 5, 1);
	rbKernel2.ReAllocate(kernelShape);
	rbKernel2.origin[0] = rbKernel2.origin[1] = -2;
	rbKernel2.ClearPixels();

	rbKernel2.Pixel(2, 0, 0) = -1.0f / 8.0f;
	rbKernel2.Pixel(2, 1, 0) =  4.0f / 8.0f;
	rbKernel2.Pixel(2, 2, 0) =  5.0f / 8.0f;
	rbKernel2.Pixel(2, 3, 0) =  4.0f / 8.0f;
	rbKernel2.Pixel(2, 4, 0) = -1.0f / 8.0f;
									
	rbKernel2.Pixel(1, 1, 0) = -1.0f / 8.0f;
	rbKernel2.Pixel(1, 3, 0) = -1.0f / 8.0f;
									
	rbKernel2.Pixel(3, 1, 0) = -1.0f / 8.0f;
	rbKernel2.Pixel(3, 3, 0) = -1.0f / 8.0f;
									
	rbKernel2.Pixel(0, 2, 0) =  0.5f / 8.0f;
	rbKernel2.Pixel(4, 2, 0) =  0.5f / 8.0f;
}

void ImageProcessing::setRB_Kernel3(CFloatImage &rbKernel3)
{
	CShape kernelShape(5, 5, 1);
	rbKernel3.ReAllocate(kernelShape);
	rbKernel3.origin[0] = rbKernel3.origin[1] = -2;
	rbKernel3.ClearPixels();

	rbKernel3.Pixel(2, 0, 0) = -1.5f / 8.0f;
	rbKernel3.Pixel(2, 2, 0) =  6.0f / 8.0f;
	rbKernel3.Pixel(2, 4, 0) = -1.5f / 8.0f;

	rbKernel3.Pixel(1, 1, 0) =  2.0f / 8.0f;
	rbKernel3.Pixel(1, 3, 0) =  2.0f / 8.0f;
									
	rbKernel3.Pixel(3, 1, 0) =  2.0f / 8.0f;
	rbKernel3.Pixel(3, 3, 0) =  2.0f / 8.0f;
									
	rbKernel3.Pixel(0, 2, 0) = -1.5f / 8.0f;
	rbKernel3.Pixel(4, 2, 0) = -1.5f / 8.0f;
}

int ImageProcessing::getBayerChannel(int x, int y)
{
	int bayerChannel = 0;
	if(y % 2)
	{
		if(x % 2)
			bayerChannel = CHANNEL_BLUE;
		else
			bayerChannel = CHANNEL_GREEN;
	}
	else
	{
		if(x % 2)
			bayerChannel = CHANNEL_GREEN;
		else
			bayerChannel = CHANNEL_RED;
	}

	return bayerChannel;
}

CFloatImage ImageProcessing::Demosiac(CFloatImage &bayerImg)
{
	ENSURE(bayerImg.Shape().nBands == 1);

	CShape demosiacImgShape = bayerImg.Shape();
	demosiacImgShape.nBands = 3;
	CFloatImage demosiacImg(demosiacImgShape);
	demosiacImg.ClearPixels();

	CFloatImage gKernel, rbKernel1, rbKernel2, rbKernel3;
	setGreenKernel(gKernel);
	setRB_Kernel1(rbKernel1);
	setRB_Kernel2(rbKernel2);
	setRB_Kernel3(rbKernel3);	
	
	for(int channel = 0; channel < demosiacImgShape.nBands; channel++)
	{
		uint pixelIndex = demosiacImgShape.PixelIndex(0, 0, channel);
		uint bayerIndex = 0;
		for(int y = 0; y < demosiacImgShape.height; y++)
		{
			for(int x = 0; x < demosiacImgShape.width; x++, bayerIndex++, pixelIndex += demosiacImgShape.nBands)
			{
				int bayerChannel = getBayerChannel(x, y);

				int rowNeighbourBC;
				if(x == 0)
					rowNeighbourBC = getBayerChannel(x + 1, y);
				else
					rowNeighbourBC = getBayerChannel(x - 1, y);

				if(channel == bayerChannel)
				{
					demosiacImg[pixelIndex] = bayerImg[bayerIndex];
				}
				else
				{
					CFloatImage *kernel = NULL;
					switch(channel)
					{
					case CHANNEL_GREEN:
						kernel = &gKernel;
						break;

					case CHANNEL_RED:
					case CHANNEL_BLUE:
						if(bayerChannel != CHANNEL_GREEN)
							kernel = &rbKernel3;
						else
						{
							if(channel == rowNeighbourBC)
								kernel = &rbKernel1;
							else
								kernel = &rbKernel2;
						}
						break;

					default:
						REPORT_FAILURE("Unknown channel\n");
						break;
					}

					demosiacImg[pixelIndex] = getBayerRespose(bayerImg, *kernel, x, y);
				}
			}
		}
	}

	return demosiacImg;
}


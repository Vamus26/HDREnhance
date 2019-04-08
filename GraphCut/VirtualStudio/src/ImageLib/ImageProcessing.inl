#ifndef __IMAGE_PROCESSSING_INL__
#define __IMAGE_PROCESSSING_INL__

#include "MathHelper.h"
#include <algorithm>

template <class T>
void ImageProcessing::ComputeMedian(const vector< CImageOf<T> > &srcImgs, const vector<CByteImage> &srcMasks, 
									CImageOf<T> &medianImg, CByteImage &medianMask, int maxSamples)
{
	int srcImageCount = (int) srcImgs.size();
	INSIST(srcImageCount > 0);
	INSIST(srcImageCount == srcMasks.size());

	CShape imgShape = srcImgs[0].Shape();
	CShape maskShape(imgShape.width, imgShape.height, 1);
	medianImg.ReAllocate(imgShape);
	medianMask.ReAllocate(maskShape);
	medianImg.ClearPixels();
	medianMask.ClearPixels();

	for(int iSrc = 0; iSrc < srcImageCount; iSrc++)
	{
		INSIST(srcImgs[iSrc].Shape()  == imgShape);
		INSIST(srcMasks[iSrc].Shape() == maskShape);
	}

	for(int iBand = 0; iBand < imgShape.nBands; iBand++)
	{
		uint pixelAddr = 0;
		uint maskAddr  = 0;
		for(int y = 0; y < imgShape.height; y++)
		for(int x = 0; x < imgShape.width;  x++, maskAddr++, pixelAddr += imgShape.nBands)
		{
			vector<T> locVals;
			locVals.reserve(srcImageCount);

			for(int iSrc = 0; iSrc < srcImageCount; iSrc++)
			{
				if(srcMasks[iSrc][maskAddr] == MASK_VALID)
				{
					if((int) locVals.size() < maxSamples)
					{
						locVals.push_back(srcImgs[iSrc][pixelAddr + iBand]);
					}
				}
			}

			int locValsCount = (int) locVals.size();
			if(locValsCount > 0)
			{
				std::sort(locVals.begin(), locVals.end());
				int medianIndex = locValsCount / 2;
				medianImg[pixelAddr + iBand] = locVals[medianIndex];
				medianMask[maskAddr] = MASK_VALID;
			}
		}
	}
}




template <class T>
CImageOf<T> ImageProcessing::GetMaskedImage(const CImageOf<T> &img, const CByteImage &imgMask)
{
	CShape imgShape = img.Shape();
	INSIST(imgShape.SameIgnoringNBands(imgMask.Shape()));
	INSIST(imgMask.Shape().nBands == 1);

	CImageOf<T> maskedImg(imgShape);
	maskedImg.ClearPixels();

	uint maskAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, maskAddr++, pixelAddr += imgShape.nBands)
	{
		if(imgMask[maskAddr] == MASK_INVALID) continue;

		for(int iBand = 0; iBand < imgShape.nBands; iBand++)
		{
			maskedImg[pixelAddr + iBand] = img[pixelAddr + iBand];
		}
	}

	return maskedImg;
}



template <class T>
CImageOf<T> ImageProcessing::ScaleImageWithoutSampling(const CImageOf<T> &srcImg, float scale)
{
	CShape srcShape = srcImg.Shape();
	CShape targetShape(NEAREST_INT(srcShape.width  * scale),
		               NEAREST_INT(srcShape.height * scale), 
					   srcShape.nBands);
	CImageOf<T> newImg(targetShape);

	uint pixelAddr = 0;
	for(int y = 0; y < targetShape.height; y++)
	for(int x = 0; x < targetShape.width;  x++)
	{
		int srcX = NEAREST_INT(x / scale);
		int srcY = NEAREST_INT(y / scale);
		
		SET_IF_LT(srcX, srcShape.width  - 1);
		SET_IF_LT(srcY, srcShape.height - 1);

		uint srcPixelAddr = srcShape.PixelIndex(srcX, srcY, 0);
		for(int channel = 0; channel < targetShape.nBands; channel++, srcPixelAddr++, pixelAddr++)
		{
			newImg[pixelAddr] = srcImg[srcPixelAddr];
		}
	}

	return newImg;
}

template <class T>
CImageOf<T> ImageProcessing::CropImage(const CImageOf<T> &srcImg, int srcX, int srcY, int targetWidth, int targetHeight)
{
	CShape srcShape = srcImg.Shape();
	ENSURE(srcShape.InBounds(srcX, srcY));
	ENSURE(srcShape.InBounds(srcX + targetWidth - 1, srcY + targetHeight - 1));

	CShape targetShape(targetWidth, targetHeight, srcShape.nBands);
	CImageOf<T> targetImg(targetShape);
	uint targetPixelAddr = 0;
	for(int y = 0; y < targetShape.height; y++)
	{
		uint srcPixelAddr = srcShape.PixelIndex(srcX, srcY + y, 0);
		for(int x = 0; x < targetShape.width; x++)
		for(int channel = 0; channel < targetShape.nBands; channel++, srcPixelAddr++, targetPixelAddr++)
		{
			targetImg[targetPixelAddr] = srcImg[srcPixelAddr];
		}
	}

	return targetImg;
}

template <class T>
CImageOf<T> ImageProcessing::CropImageUsingReflection(const CImageOf<T> &srcImg, int srcX, int srcY, int targetWidth, int targetHeight)
{
	CShape srcShape = srcImg.Shape();

	CShape targetShape(targetWidth, targetHeight, srcShape.nBands);
	CImageOf<T> targetImg(targetShape);
	uint targetPixelAddr = 0;
	for(int y = 0; y < targetShape.height; y++)
	for(int x = 0; x < targetShape.width; x++)
	{
		int xx = srcX + x;
		int yy = srcY + y;
		if(xx < 0) xx = -xx;
		if(yy < 0) yy = -yy;
		if(xx > srcShape.width - 1) xx = (srcShape.width - 1) - (xx - (srcShape.width - 1));
		if(yy > srcShape.height - 1) yy = (srcShape.height - 1) - (yy - (srcShape.height - 1));
		ENSURE(srcShape.InBounds(xx, yy));

		uint srcPixelAddr = srcShape.PixelIndex(xx, yy, 0);
		for(int channel = 0; channel < targetShape.nBands; channel++, srcPixelAddr++, targetPixelAddr++)
		{
			targetImg[targetPixelAddr] = srcImg[srcPixelAddr];
		}
	}

	return targetImg;
}

template <class T>
CImageOf<T> ImageProcessing::CropImage(const CImageOf<T> &srcImg, const CByteImage &srcMask, bool &cropViolatesMask, 
									   int srcX, int srcY, int cropWidth, int cropHeight)
{
	CShape srcShape     = srcImg.Shape();
	CShape srcMaskShape = srcMask.Shape();
	ENSURE(srcShape.InBounds(srcX, srcY));
	ENSURE(srcShape.InBounds(srcX + cropWidth - 1, srcY + cropHeight - 1));
	ENSURE(srcShape.SameIgnoringNBands(srcMaskShape));
	ENSURE(srcMaskShape.nBands == 1);

	cropViolatesMask = false;

	CShape cropShape(cropWidth, cropHeight, srcShape.nBands);
	CImageOf<T> cropImg(cropShape);
	uint cropPixelAddr = 0;
	for(int y = 0; y < cropHeight; y++)
	{
		uint srcPixelAddr = srcShape.PixelIndex(srcX, srcY + y, 0);
		uint srcMaskAddr  = srcMaskShape.NodeIndex(srcX, srcY + y);
		for(int x = 0; x < cropWidth; x++, srcMaskAddr++)
		{
			if(srcMask[srcMaskAddr] == MASK_INVALID)
			{
				cropViolatesMask = true;
				return cropImg;
			}

			for(int channel = 0; channel < cropShape.nBands; channel++, cropPixelAddr++, srcPixelAddr++)
			{
				cropImg[cropPixelAddr] = srcImg[srcPixelAddr];
			}
		}
	}

	return cropImg;
}


template <class T>
CImageOf<T> ImageProcessing::FlipVertical(CImageOf<T>  &img)
{
	CShape imgShape = img.Shape();
	CImageOf<T> flipImg(imgShape);

	uint flipPixelIndex = 0;
	for(int y = 0; y < imgShape.height; y++)
	{
		uint pixelIndex = img.PixelIndex(0, imgShape.height - 1 - y, 0);
		for(int x = 0; x < imgShape.width; x++)
		{
			for(int channel = 0; channel < imgShape.nBands; channel++, pixelIndex++, flipPixelIndex++)
			{
				flipImg[flipPixelIndex] = img[pixelIndex];
			}
		}
	}

	return flipImg;
}


template <class T>
CImageOf<T> ImageProcessing::ErodeImage(const CImageOf<T> &mask, int erosionWidth, T erodeVal)
{
	CShape maskShape = mask.Shape();
	ENSURE(maskShape.depth  == 1);
	ENSURE(maskShape.nBands == 1);
	ENSURE(erosionWidth % 2 == 1);

	CImageOf<T> erodedMask(maskShape);
	int halfWidth = erosionWidth / 2;     
	uint nodeAddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	{
		for(int x = 0; x < maskShape.width; x++, nodeAddr++)
		{
			bool erode = false;
			for(int yy = y - halfWidth; yy <= y + halfWidth; yy++)
			{
				for(int xx = x - halfWidth; xx <= x + halfWidth; xx++)
				{
					if(maskShape.InBounds(xx, yy, 0) == false)
					{
						continue;
					}
					if(mask.Pixel(xx, yy, 0) == erodeVal)
					{
						erode = true;
						break;
					}
				}
			}
			if(erode == true)
			{
				erodedMask[nodeAddr] = erodeVal;
			}
			else
			{
				erodedMask[nodeAddr] = mask[nodeAddr];
			}
		}
	}

	return erodedMask;
}


template <class T>
float ImageProcessing::MitchellInterlopation(CImageOf<T> &img, float x, float y, int band, const CByteImage &mask, bool useMask)
{
	CShape imgShape = img.Shape();
	if(useMask == true)
	{
		CShape maskShape = mask.Shape();
		ENSURE(maskShape.nBands == 1);
		ENSURE(imgShape.SameIgnoringNBands(maskShape));
	}

	const static int radius = 2;
	int minX = (int) (x - radius);
	int maxX = (int) (x + radius);
	int minY = (int) (y - radius);
	int maxY = (int) (y + radius);

	double weightSum = 0;
	double signalSum = 0;
	for(int yy = minY; yy <= maxY; yy++)
	for(int xx = minX; xx <= maxX; xx++)
	{
		if(imgShape.InBounds(xx, yy) == false)
		{
			continue;
		}
		if((useMask == true) && (mask.Pixel(xx, yy, 0) == MASK_INVALID))
		{
			continue;
		}

		double weight;
		weight     = MathHelper::MitchellKernelVal(x - xx) * MathHelper::MitchellKernelVal(y - yy);
		weightSum += weight;
		signalSum += (img.Pixel(xx, yy, band) * weight);		
	}

	if(useMask == false)
	{
		ENSURE(weightSum > 0);
	}

	float signal = (float) (signalSum / weightSum);
	return signal;
}

template <class T>
void ImageProcessing::MitchellInterlopation(CImageOf<T>  &dstImg, uint dstPixelAddr, CImageOf<T>  &srcImg, float x, float y)
{
	CShape srcShape = srcImg.Shape();
	ENSURE(srcShape.nBands == dstImg.Shape().nBands);

	const static int radius = 2;
	int minX = (int) (x - radius);
	int maxX = (int) (x + radius);
	int minY = (int) (y - radius);
	int maxY = (int) (y + radius);

	double weightSum = 0;

	if(srcShape.nBands == 3)
	{
		double signalSum[3] = {0.0};
		for(int yy = minY; yy <= maxY; yy++)
		for(int xx = minX; xx <= maxX; xx++)
		{
			if(srcShape.InBounds(xx, yy) == false) continue;

			double weight = MathHelper::MitchellKernelVal(x - xx) * MathHelper::MitchellKernelVal(y - yy);
			weightSum += weight;

			uint srcPixelAddr = srcShape.PixelIndex(xx, yy, 0);
			signalSum[0] += srcImg[srcPixelAddr + 0] * weight;
			signalSum[1] += srcImg[srcPixelAddr + 1] * weight;
			signalSum[2] += srcImg[srcPixelAddr + 2] * weight;
		}
		INSIST(weightSum > 0);
		dstImg[dstPixelAddr + 0] = (T) (signalSum[0] / weightSum);
		dstImg[dstPixelAddr + 1] = (T) (signalSum[1] / weightSum);
		dstImg[dstPixelAddr + 2] = (T) (signalSum[2] / weightSum);
	}
	else
	{
		INSIST(srcShape.nBands == 1);

		double signalSum = 0.0;
		for(int yy = minY; yy <= maxY; yy++)
		for(int xx = minX; xx <= maxX; xx++)
		{
			if(srcShape.InBounds(xx, yy) == false) continue;

			double weight = MathHelper::MitchellKernelVal(x - xx) * MathHelper::MitchellKernelVal(y - yy);
			weightSum += weight;

			signalSum += srcImg.Pixel(xx, yy, 0) * weight;
		}
		INSIST(weightSum > 0);
		dstImg[dstPixelAddr] = (T) (signalSum / weightSum);
	}
}

template <class T>
void ImageProcessing::Operate(CImageOf<T> &dst, const CImageOf<T> &image1, const CImageOf<T> &image2, ImageOperation op, int band, const CByteImage &mask, bool useMask)
{
	CShape img1Shape = image1.Shape();
	CShape img2Shape = image2.Shape();
	CShape dstShape  = dst.Shape();
	INSIST(img1Shape.SameIgnoringNBands(img2Shape));

	INSIST(img1Shape.SameIgnoringNBands(dstShape));
	if(useMask == true)
	{
		INSIST(img1Shape.SameIgnoringNBands(mask.Shape()));
		INSIST(mask.Shape().nBands == 1);
	}

	int startChannel, endChannel;
	if(band == -1)
	{
		startChannel = 0; endChannel = img1Shape.nBands - 1;
		INSIST(img1Shape.nBands == img2Shape.nBands);
		INSIST(img1Shape.nBands == dstShape.nBands);
	}
	else
	{
		startChannel = endChannel = band;
		INSIST_RANGE(band, 0, img1Shape.nBands - 1);
		INSIST_RANGE(band, 0, img2Shape.nBands - 1);
		INSIST_RANGE(band, 0, dstShape.nBands - 1);
	}
		
	uint pixelIndex1   = 0;
	uint pixelIndex2   = 0;
	uint dstPixelIndex = 0;
	uint maskIndex = 0;
	for(int imageY = 0; imageY < img1Shape.height; imageY++)
	for(int imageX = 0; imageX < img1Shape.width; imageX++, 
		maskIndex++, pixelIndex1 += img1Shape.nBands, pixelIndex2 += img2Shape.nBands, dstPixelIndex += dstShape.nBands)
	{
		if((useMask == true) && (mask[maskIndex] == MASK_INVALID))
		{
			continue;
		}
		
		for(int imageB = startChannel; imageB <= endChannel; imageB++)
		{
			T newVal = 0;
			T val1   = image1[pixelIndex1 + imageB];
			T val2   = image2[pixelIndex2 + imageB];

			switch(op)
			{
			case ImageProcessing::OP_ADD:
				newVal = val1 + val2;
				break;

			case ImageProcessing::OP_POW:
				newVal = pow(val1, val2);
				break;

			case ImageProcessing::OP_SUB:
				newVal = val1 - val2;
				break;

			case ImageProcessing::OP_MUL:
				newVal = val1 * val2;
				break;

			case ImageProcessing::OP_DIV:
				newVal = val1 / val2;
				if(val2 == 0)
				{	
					REPORT_FAILURE("Division by zero\n");
				}
				break;

			default:
				REPORT_FAILURE("Unknown image operation");
				break;
			}
			
			dst[dstPixelIndex + imageB] = newVal;			
		}
	}
}

template <class T>
void ImageProcessing::Operate(CImageOf<T> &dst, const CImageOf<T> &image1, T val, ImageOperation op, int band, const CByteImage &mask, bool useMask)
{
	CShape imgShape = image1.Shape();
	ENSURE(imgShape == dst.Shape());
	if(useMask == true)
	{
		ENSURE(imgShape.SameIgnoringNBands(mask.Shape()));
		ENSURE(mask.Shape().nBands == 1);
	}

	if((op == ImageProcessing::OP_DIV) && (val == 0))
	{	
		REPORT_FAILURE("Division by zero\n");
	}

	int startChannel, endChannel;
	if(band == -1)
	{
		startChannel = 0; endChannel = imgShape.nBands - 1;
	}
	else
	{
		startChannel = endChannel = band;
		CHECK_RANGE(band, 0, imgShape.nBands - 1);
	}
		
	uint pixelIndex = 0;
	uint maskIndex  = 0;
	for(int imageY = 0; imageY < imgShape.height; imageY++)
	{
		for(int imageX = 0; imageX < imgShape.width; imageX++, maskIndex++, pixelIndex += imgShape.nBands)
		{
			if((useMask == true) && (mask[maskIndex] == MASK_INVALID))
			{
				continue;
			}			

			for(int imageB = startChannel; imageB <= endChannel; imageB++)
			{
				T newVal = 0;
				T val1   = image1[pixelIndex + imageB];
				T val2   = val;

				switch(op)
				{
				case ImageProcessing::OP_ADD:
					newVal = val1 + val2;
					break;

				case ImageProcessing::OP_POW:
					newVal = (T) pow((double) val1, (double) val2);
					break;

				case ImageProcessing::OP_SUB:
					newVal = val1 - val2;
					break;

				case ImageProcessing::OP_MUL:
					newVal = val1 * val2;
					break;

				case ImageProcessing::OP_DIV:
					newVal = val1 / val2;					
					break;

				case ImageProcessing::OP_ABS:
					newVal = ((val1 < 0) ? (-val1) : val1);
					break;

				case ImageProcessing::OP_SET:
					newVal = val2;
					break;					

				default:
					REPORT_FAILURE("Unknown image operation");
					break;
				}
				
				dst[pixelIndex + imageB] = newVal;			
			}
		}
	}
}


template <class T>
void ImageProcessing::CopyChannel(CImageOf<T> &dst, CImageOf<T> &src, 
								  int dstChannel, int srcChannel)
{
	CShape dstShape = dst.Shape();
	CShape srcShape = src.Shape();
	ENSURE(dstShape.SameIgnoringNBands(srcShape));
	ENSURE(dstChannel >= 0);
	ENSURE(dstChannel < dstShape.nBands);
	ENSURE(srcChannel >= 0);
	ENSURE(srcChannel < srcShape.nBands);

	uint dstAddr = 0;
	uint srcAddr = 0;
	for(int y = 0; y < dstShape.height; y++)
	{
		for(int x = 0; x < dstShape.width; x++, dstAddr += dstShape.nBands, srcAddr += srcShape.nBands)
		{
			dst[dstAddr + dstChannel] = src[srcAddr + srcChannel];
		}
	}
}

#endif



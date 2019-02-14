#include "Definitions.h"
#include "ImageProcessing.h"
#include "MathHelper.h"
#include "ColorHelper.h"
#include "ImageIO.h"


CFloatImage ImageProcessing::GetCompositeImage(const CFloatImage &orgImg, 
											   const CFloatImage &imgToComposite, 
											   const CFloatImage &compositeMask)
{
	CShape imgShape = orgImg.Shape();
	CFloatImage compositeImg(imgShape);

	INSIST(imgShape == imgToComposite.Shape());
	INSIST(imgShape == compositeMask.Shape());

	uint pixelAddr = 0;
	uint nodeAddr  = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++)
	for(int iBand = 0; iBand < imgShape.nBands; iBand++, pixelAddr++)
	{
		INSIST_RANGE(compositeMask[pixelAddr], 0.0f, 1.0f);
		compositeImg[pixelAddr] = (orgImg[pixelAddr]         * (1.0f - compositeMask[pixelAddr])) +
								  (imgToComposite[pixelAddr] * compositeMask[pixelAddr]); 
	}

	return compositeImg;
}

CByteImage ImageProcessing::GetInvertedMask(const CByteImage &mask)
{
	CShape maskShape = mask.Shape();
	CByteImage invertedMask(maskShape);

	uint maskAddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	for(int x = 0; x < maskShape.width;  x++, maskAddr++)
	{
		if(mask[maskAddr] == MASK_VALID)
			invertedMask[maskAddr] = MASK_INVALID;
		else
		{
			//INSIST(mask[maskAddr] == MASK_INVALID)
			invertedMask[maskAddr] = MASK_VALID;
		}
	}

	return invertedMask;
}

void ImageProcessing::GetEdgeImgs(const CFloatImage &image, CFloatImage &horzEdges, CFloatImage &vertEdges)
{
	CShape imgShape = image.Shape();
	horzEdges.ReAllocate(imgShape);
	vertEdges.ReAllocate(imgShape);

	uint pixelAddr = 0;
	int rowSize = imgShape.width * imgShape.nBands;
	for(int y = 0; y < imgShape.height; y++)
	{		
		for(int x = 0; x < imgShape.width; x++)
		{
			bool calcHorzEdge = false;
			bool calcVertEdge = false;

		    if(x + 1 < imgShape.width)
				calcHorzEdge = true;
			if(y + 1 < imgShape.height)
				calcVertEdge = true;

			for(int iBand = 0; iBand < imgShape.nBands; iBand++, pixelAddr++)
			{
				if(calcHorzEdge)
					horzEdges[pixelAddr] = image[pixelAddr + imgShape.nBands] - image[pixelAddr];
				else
					horzEdges[pixelAddr] = 0.0f;

				if(calcVertEdge)
					vertEdges[pixelAddr] = image[pixelAddr + rowSize] - image[pixelAddr];
				else
					vertEdges[pixelAddr] = 0.0f;
			}
		}
	}
}

CByteImage ImageProcessing::FloatToByteImg(const CFloatImage &floatImg)
{
	CShape imgShape = floatImg.Shape();
	int dataCount = imgShape.width * imgShape.height * imgShape.nBands;
	CByteImage byteImage(imgShape);
	for(int iData = 0; iData < dataCount; iData++)
	{
		float currDataVal = (floatImg[iData] * 255.0f);		
		if(currDataVal > 255.0f) currDataVal = 255.0f;
		if(currDataVal < 0.0f) currDataVal = 0.0f;
		byteImage[iData] = (uchar) currDataVal;
	}

	return byteImage;
}

CFloatImage ImageProcessing::ByteToFloatImg(const CByteImage &byteImg)
{
	CShape imgShape = byteImg.Shape();
	CFloatImage floatImg(imgShape);
	int dataCount = imgShape.width * imgShape.height * imgShape.nBands;
	for(int iData = 0; iData < dataCount; iData++)
	{
		floatImg[iData] = (byteImg[iData] / 255.0f);
	}

	return floatImg;
}


float dist(float xDisp, float yDisp)
{
	return sqrt((xDisp * xDisp) + (yDisp * yDisp));
}

CFloatImage ImageProcessing::GetDistImage(const CByteImage &mask)
{
	CShape maskShape = mask.Shape();
	ENSURE(maskShape.nBands == 1);
	CFloatImage distImg(maskShape);
	CFloatImage distImgVec(maskShape.width, maskShape.height, 2);

	uint nodeAddr = 0;
	uint vecAdddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	{		
		for(int x = 0; x < maskShape.width; x++, nodeAddr++, vecAdddr+= 2)
		{
			distImg[nodeAddr] = (mask[nodeAddr] == MASK_INVALID) ? FLT_MAX : 0;
			distImgVec[vecAdddr + 0] = distImgVec[vecAdddr + 1] = distImg[nodeAddr];
		}
	}

	float newDist;
	for(int y = 1; y < maskShape.height; y++)
	{
		for(int x = 0; x < maskShape.width; x++)
		{
			newDist = dist(distImgVec.Pixel(x, y - 1, 0), 
						   distImgVec.Pixel(x, y - 1, 1) + 1);
			if(newDist < distImg.Pixel(x, y, 0))
			{
				distImg.Pixel(x, y, 0)    = newDist;
				distImgVec.Pixel(x, y, 0) = distImgVec.Pixel(x, y - 1, 0);
				distImgVec.Pixel(x, y, 1) = distImgVec.Pixel(x, y - 1, 1) + 1;
			}
		}

		for(int x = 1; x < maskShape.width; x++)
		{
			newDist = dist(distImgVec.Pixel(x - 1, y, 0) + 1, 
						   distImgVec.Pixel(x - 1, y, 1));
			if(newDist < distImg.Pixel(x, y, 0))
			{
				distImg.Pixel(x, y, 0)    = newDist;
				distImgVec.Pixel(x, y, 0) = distImgVec.Pixel(x - 1, y, 0) + 1;
				distImgVec.Pixel(x, y, 1) = distImgVec.Pixel(x - 1, y, 1);
			}
		}

		for(int x = maskShape.width - 2; x >= 0; x--)
		{
			newDist = dist(distImgVec.Pixel(x + 1, y, 0) + 1, 
						   distImgVec.Pixel(x + 1, y, 1));
			if(newDist < distImg.Pixel(x, y, 0))
			{
				distImg.Pixel(x, y, 0)    = newDist;
				distImgVec.Pixel(x, y, 0) = distImgVec.Pixel(x + 1, y, 0) + 1;
				distImgVec.Pixel(x, y, 1) = distImgVec.Pixel(x + 1, y, 1);
			}
		}
	}


	for(int y = maskShape.height - 2; y >= 0; y--)
	{
		for(int x = 0; x < maskShape.width; x++)
		{
			newDist = dist(distImgVec.Pixel(x, y + 1, 0), 
						   distImgVec.Pixel(x, y + 1, 1) + 1);
			if(newDist < distImg.Pixel(x, y, 0))
			{
				distImg.Pixel(x, y, 0)    = newDist;
				distImgVec.Pixel(x, y, 0) = distImgVec.Pixel(x, y + 1, 0);
				distImgVec.Pixel(x, y, 1) = distImgVec.Pixel(x, y + 1, 1) + 1;
			}
		}

		for(int x = 1; x < maskShape.width; x++)
		{
			newDist = dist(distImgVec.Pixel(x - 1, y, 0) + 1, 
						   distImgVec.Pixel(x - 1, y, 1));
			if(newDist < distImg.Pixel(x, y, 0))
			{
				distImg.Pixel(x, y, 0)    = newDist;
				distImgVec.Pixel(x, y, 0) = distImgVec.Pixel(x - 1, y, 0) + 1;
				distImgVec.Pixel(x, y, 1) = distImgVec.Pixel(x - 1, y, 1);
			}
		}

		for(int x = maskShape.width - 2; x >= 0; x--)
		{
			newDist = dist(distImgVec.Pixel(x + 1, y, 0) + 1, 
						   distImgVec.Pixel(x + 1, y, 1));
			if(newDist < distImg.Pixel(x, y, 0))
			{
				distImg.Pixel(x, y, 0)    = newDist;
				distImgVec.Pixel(x, y, 0) = distImgVec.Pixel(x + 1, y, 0) + 1;
				distImgVec.Pixel(x, y, 1) = distImgVec.Pixel(x + 1, y, 1);
			}
		}
	}

	return distImg;
}

void ImageProcessing::ScaleImage(CFloatImage &dst, CFloatImage &src, float scale, bool blurBeforeScaling, float gaussianStdDev)
{
	ENSURE(scale > 0.0f);
	
	CShape imgShape = src.Shape();
	CShape dstShape = imgShape;
	dstShape.width  = NEAREST_INT(dstShape.width  * scale);
	dstShape.height = NEAREST_INT(dstShape.height * scale);
	dst.ReAllocate(dstShape);
	
	CShape dstMaskShape = dstShape;
	dstMaskShape.nBands = 1;
	CByteImage dummyMask(dstMaskShape);

	CTransform3x3 scaleMat;
	scaleMat[0][0] = 1.0f / scale;
	scaleMat[1][1] = scaleMat[0][0];

	if((blurBeforeScaling) &&
	   (scale < 1.0f))
	{
		CFloatImage smoothSrc = ImageProcessing::GaussianBlur(src, 5, gaussianStdDev);
		ImageProcessing::TransformImage(dst, dummyMask, scaleMat, smoothSrc);
	}
	else
	{
		ImageProcessing::TransformImage(dst, dummyMask, scaleMat, src);
	}
}


double ImageProcessing::L2Diff(CFloatImage &image1, 
							   CFloatImage &image2, 
							   const CByteImage &mask, bool useMask)
{
	CShape imgShape = image1.Shape();
	ENSURE(imgShape == image2.Shape());
	ENSURE(imgShape.depth == 1);
	if(useMask == true)
	{
		CShape maskShape = mask.Shape();
		ENSURE(maskShape.nBands == 1);
		ENSURE(imgShape.SameIgnoringNBands(maskShape));		
	}

	double l2diff  = 0;
	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int imageY = 0; imageY < imgShape.height; imageY++)
	{		
		for(int imageX = 0; imageX < imgShape.width; imageX++, pixelAddr += imgShape.nBands, nodeAddr++)
		{
			if((useMask == true) && (mask[nodeAddr] == MASK_INVALID)) continue;

			for(int imageB = 0; imageB < imgShape.nBands; imageB++)
			{
				double diff = image1[pixelAddr + imageB] - image2[pixelAddr + imageB];
				l2diff += diff * diff;
			}
		}
	}

	return l2diff;
}

void ImageProcessing::Convolution(CFloatImage &dst, 
								  CFloatImage &src, 
								  CFloatImage &kernel, 
								  const CByteImage &mask, bool useMask)
{
	CShape kernelShape = kernel.Shape();
	CShape srcShape    = src.Shape();
	ENSURE(srcShape == dst.Shape());
	ENSURE(srcShape.depth == 1);
	ENSURE(kernelShape.depth == 1);
	ENSURE(kernelShape.nBands == 1);

	if(useMask == true)
	{
		ENSURE(srcShape.SameIgnoringNBands(mask.Shape()));
		ENSURE(mask.Shape().nBands == 1);
	}

	uint nodeAddr = 0;
	uint pixelAddr = 0;
	for(int imageY = 0; imageY < srcShape.height; imageY++)
	{		
		for(int imageX = 0; imageX < srcShape.width; imageX++, nodeAddr++)
		{		
			for(int imageB = 0; imageB < srcShape.nBands; imageB++, pixelAddr++)
			{
				if((useMask == true) && (mask[nodeAddr] == MASK_INVALID))
				{
					dst[pixelAddr] = 0;
					continue;
				}

				float sampleSum = 0;
				for(int kernelY = 0; kernelY < kernelShape.height; kernelY++)
				{
					for(int kernelX = 0; kernelX < kernelShape.width; kernelX++)
					{
						int sampleX  = imageX + (kernelX + kernel.origin[0]);
						int sampleY  = imageY + (kernelY + kernel.origin[1]);

						if(sampleX < 0)	sampleX = -sampleX;
						if(sampleY < 0)	sampleY = -sampleY;
						if(sampleX > (srcShape.width  - 1))  sampleX = (srcShape.width  - 1) - (sampleX - (srcShape.width  - 1));
						if(sampleY > (srcShape.height - 1))  sampleY = (srcShape.height - 1) - (sampleY - (srcShape.height - 1));

						if((useMask == true) && (mask.Pixel(sampleX, sampleY, 0) == MASK_INVALID))
						{
							sampleX = imageX;
							sampleY = imageY;
						}

						float sample = src.Pixel(sampleX, sampleY, imageB);
						float weight = kernel.Pixel(kernelX, kernelY, 0);

						sampleSum += (sample * weight);
					}
				}
				dst[pixelAddr] = sampleSum;
			}
		}
	}
}

void ImageProcessing::GetImageSobel(CFloatImage &dst, CFloatImage &img, CFloatImage &kernel, const CByteImage &mask, bool useMask)
{
	CShape imgShape = img.Shape();
	ENSURE(dst.Shape().SameIgnoringNBands(imgShape));
	ENSURE(imgShape.depth == 1);
	ENSURE(dst.Shape().nBands == 1);
	
	CFloatImage edgeImg(imgShape);
	ImageProcessing::Convolution(edgeImg, img, kernel, mask, useMask);
	
	uint maskAddr = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	{
		for(int x = 0; x < imgShape.width; x++, maskAddr++)
		{
			float sum = 0;
			for(int iBand = 0; iBand < imgShape.nBands; iBand++, pixelAddr++)
			{
				sum += (edgeImg[pixelAddr] * edgeImg[pixelAddr]);
			}
			dst[maskAddr] = sqrt(sum);
		}
	}
}

void ImageProcessing::GetImagesSobel(CFloatImage &hEdges, CFloatImage &vEdges, 
									 CFloatImage &img, const CByteImage &mask, bool useMask, int erosionWidth)
{	
	CByteImage maskCopy;
	if((useMask == true) && (erosionWidth > 0))
	{
		maskCopy = ErodeImage(mask, erosionWidth, MASK_INVALID);
	}
	else
	{
		maskCopy = mask;
	}

	CFloatImage sobelHE = getSobelHE();
	CFloatImage sobelVE = getSobelVE();
	ImageProcessing::GetImageSobel(hEdges, img, sobelHE, maskCopy, useMask);
	ImageProcessing::GetImageSobel(vEdges, img, sobelVE, maskCopy, useMask);
}


void ImageProcessing::GetImagesSobel(CFloatImage &hEdges, CFloatImage &vEdges, 
									 CFloatImage &pdEdges, CFloatImage &ndEdges, 
									 CFloatImage &img, const CByteImage &mask, bool useMask, int erosionWidth)
{	
	CByteImage maskCopy;
	if((useMask == true) && (erosionWidth > 0))
	{
		maskCopy = ErodeImage(mask, erosionWidth, MASK_INVALID);
	}
	else
	{
		maskCopy = mask;
	}

	CFloatImage sobelHE    = getSobelHE();
	CFloatImage sobelVE    = getSobelVE();
	CFloatImage sobelDiagP = getSobelDiagP();
	CFloatImage sobelDiagN = getSobelDiagN();

	ImageProcessing::GetImageSobel(hEdges,  img, sobelHE,    maskCopy, useMask);
	ImageProcessing::GetImageSobel(vEdges,  img, sobelVE,    maskCopy, useMask);
	ImageProcessing::GetImageSobel(pdEdges, img, sobelDiagP, maskCopy, useMask);
	ImageProcessing::GetImageSobel(ndEdges, img, sobelDiagN, maskCopy, useMask);
}

uint ImageProcessing::TransformImage(CFloatImage &dst, CByteImage &dstMask,
									 CTransform3x3 homography, CFloatImage &source, 
									 const CByteImage &sourceMask, bool useSourceMask)
{	CShape srcShape = source.Shape();
	CShape dstShape = dst.Shape();
	ENSURE(srcShape.depth == 1);
	ENSURE(dstShape.depth == 1);
	ENSURE(dstShape.SameIgnoringNBands(dstMask.Shape()));
	ENSURE(dstMask.Shape().nBands == 1);
	if(useSourceMask == true)
	{
		CShape maskShape = sourceMask.Shape();
		ENSURE(srcShape.SameIgnoringNBands(maskShape));
		ENSURE(maskShape.nBands == 1);
	}

	CByteImage sampleMask(srcShape.width, srcShape.height, 1);
	sampleMask.ClearPixels();
	uint pixelsSampled = 0;

	dst.ClearPixels();
	dstMask.ClearPixels();
	uint dstMaskAddr  = 0;
	uint dstPixelAddr = 0;
	for(int y = 0; y < dstShape.height; y++)
	for(int x = 0; x < dstShape.width; x++, dstPixelAddr += dstShape.nBands, dstMaskAddr++)
	{
		CVector3 dstPT;
		dstPT[0] = x; dstPT[1] = y; dstPT[2] = 1;

		CVector3 sourcePT = homography * dstPT;
		float srcX = (float) (sourcePT[0] / sourcePT[2]);
		float srcY = (float) (sourcePT[1] / sourcePT[2]);

		int roundedSrcX = NEAREST_INT(srcX); 
		int roundedSrcY = NEAREST_INT(srcY);
		if(roundedSrcX == srcShape.width) roundedSrcX--;
		if(roundedSrcY == srcShape.height) roundedSrcY--;
		if(srcShape.InBounds(roundedSrcX, roundedSrcY) == false) continue;

		if((useSourceMask == true) &&
		   (sourceMask.Pixel(roundedSrcX, roundedSrcY, 0) == MASK_INVALID))
		{
			continue;
		}

		//ImageProcessing::MitchellInterlopation(dst, dstPixelAddr, source, srcX, srcY);
		ImageProcessing::BilinearIterpolation(dst, dstPixelAddr, source, srcX, srcY);
		dstMask[dstMaskAddr] = MASK_VALID;

		uchar *sampleMaskLookup = (uchar *) sampleMask.PixelAddress(roundedSrcX, roundedSrcY, 0);
		if(*sampleMaskLookup == MASK_INVALID)
		{
			pixelsSampled++;
			*sampleMaskLookup = MASK_VALID;
		}
	}
	
	return pixelsSampled;
}

float ImageProcessing::BilinearIterpolation(const CFloatImage &img, float x, float y, int band, bool reflectCoords)
{
	float interpolatedSig = -1;

	CShape imgShape = img.Shape();
	ENSURE(imgShape.depth == 1);

	if(reflectCoords)
	{
		if(x < 0)	x = -x;
		if(y < 0)	y = -y;
		if(x > (imgShape.width  - 1))  x = imgShape.width  + imgShape.width  - 2 - x; //x = (imgShape.width  - 1)  - (x - (imgShape.width  - 1));
		if(y > (imgShape.height - 1))  y = imgShape.height + imgShape.height - 2 - y; //y = (imgShape.height  - 1) - (y - (imgShape.height  - 1));
	}

	int floorX = (int) x;
	int floorY = (int) y;
	int roofX  = (int) (x + 1);
	int roofY  = (int) (y + 1);

	float xRoofFrac  = x - floorX;
	float yRoofFrac  = y - floorY;
	float xFloorFrac = 1.0f - xRoofFrac;
	float yFloorFrac = 1.0f - yRoofFrac;

	uint floorXY_pixelAddr = imgShape.PixelIndex(floorX, floorY, band);
	uint roofXY_pixelAddr  = imgShape.PixelIndex(roofX, roofY, band);

	if(xRoofFrac > FLT_EPSILON)
	{
		float xInterpolatedFloor;
		float floorSignal;
		float roofSignal;

		floorSignal        = img[floorXY_pixelAddr];                   //img.Pixel(floorX, floorY, band);
		roofSignal         = img[floorXY_pixelAddr + imgShape.nBands]; //img.Pixel(roofX,  floorY, band);
		xInterpolatedFloor = (floorSignal * xFloorFrac) + (roofSignal * xRoofFrac);

		if(yRoofFrac > FLT_EPSILON)
		{
			float xInterpolatedRoof;

			floorSignal       = img[roofXY_pixelAddr - imgShape.nBands]; //img.Pixel(floorX, roofY, band);
			roofSignal        = img[roofXY_pixelAddr];                   //img.Pixel(roofX,  roofY, band);
			xInterpolatedRoof = (floorSignal * xFloorFrac) + (roofSignal * xRoofFrac);
			interpolatedSig   = (xInterpolatedFloor * yFloorFrac) + (xInterpolatedRoof * yRoofFrac);
		}
		else
		{
			interpolatedSig = xInterpolatedFloor;
		}
	}
	else
	{
		if(yRoofFrac > FLT_EPSILON)
		{
			float floorSignal;
			float roofSignal;

			floorSignal     = img[floorXY_pixelAddr];                   //img.Pixel(floorX, floorY, band);
			roofSignal      = img[roofXY_pixelAddr - imgShape.nBands];  //img.Pixel(floorX, roofY,  band); //floorX == roofX - 1
			interpolatedSig = (floorSignal * yFloorFrac) + (roofSignal * yRoofFrac);
		}
		else
		{
			interpolatedSig = img[floorXY_pixelAddr]; //img.Pixel(floorX, floorY, band);
		}
	}

	return interpolatedSig;
}

void ImageProcessing::BilinearIterpolation(CFloatImage &dstImg, uint dstPixelAddr, const CFloatImage &srcImg, float x, float y, bool reflectCoords)
{
	CShape srcShape = srcImg.Shape();
	ENSURE(srcShape.nBands == dstImg.Shape().nBands);

	if(srcShape.nBands == 1)
	{
		dstImg[dstPixelAddr] = ImageProcessing::BilinearIterpolation(srcImg, x, y, 0, reflectCoords);
		return;
	}
	INSIST(srcShape.nBands == 3);

	if(reflectCoords)
	{
		if(x < 0) x = -x;
		if(y < 0) y = -y;
		if(x > (srcShape.width  - 1)) x = srcShape.width  + srcShape.width  - 2 - x; //x = (imgShape.width  - 1)  - (x - (imgShape.width  - 1));
		if(y > (srcShape.height - 1)) y = srcShape.height + srcShape.height - 2 - y; //y = (imgShape.height  - 1) - (y - (imgShape.height  - 1));
	}

	int floorX = (int) x;
	int floorY = (int) y;
	int roofX  = (int) (x + 1);
	int roofY  = (int) (y + 1);

	float xRoofFrac  = x - floorX;
	float yRoofFrac  = y - floorY;
	float xFloorFrac = 1.0f - xRoofFrac;
	float yFloorFrac = 1.0f - yRoofFrac;

	uint floorXY_pixelAddr = srcShape.PixelIndex(floorX, floorY, 0);
	uint roofXY_pixelAddr  = srcShape.PixelIndex(roofX, roofY, 0);

	if(xRoofFrac > FLT_EPSILON)
	{
		float xInterpolatedFloor[3];
		float floorSignal[3];
		float roofSignal[3];

		floorSignal[0]        = srcImg[floorXY_pixelAddr + 0];                   //img.Pixel(floorX, floorY, band);
		floorSignal[1]        = srcImg[floorXY_pixelAddr + 1];                   //img.Pixel(floorX, floorY, band);
		floorSignal[2]        = srcImg[floorXY_pixelAddr + 2];                   //img.Pixel(floorX, floorY, band);

		roofSignal[0]         = srcImg[floorXY_pixelAddr + srcShape.nBands + 0]; //img.Pixel(roofX,  floorY, band);
		roofSignal[1]         = srcImg[floorXY_pixelAddr + srcShape.nBands + 1]; //img.Pixel(roofX,  floorY, band);
		roofSignal[2]         = srcImg[floorXY_pixelAddr + srcShape.nBands + 2]; //img.Pixel(roofX,  floorY, band);

		xInterpolatedFloor[0] = (floorSignal[0] * xFloorFrac) + (roofSignal[0] * xRoofFrac);
		xInterpolatedFloor[1] = (floorSignal[1] * xFloorFrac) + (roofSignal[1] * xRoofFrac);
		xInterpolatedFloor[2] = (floorSignal[2] * xFloorFrac) + (roofSignal[2] * xRoofFrac);

		if(yRoofFrac > FLT_EPSILON)
		{
			float xInterpolatedRoof[3];

			floorSignal[0]            = srcImg[roofXY_pixelAddr - srcShape.nBands + 0]; //img.Pixel(floorX, roofY, band);
			floorSignal[1]            = srcImg[roofXY_pixelAddr - srcShape.nBands + 1]; //img.Pixel(floorX, roofY, band);
			floorSignal[2]            = srcImg[roofXY_pixelAddr - srcShape.nBands + 2]; //img.Pixel(floorX, roofY, band);
								
			roofSignal[0]             = srcImg[roofXY_pixelAddr + 0];                   //img.Pixel(roofX,  roofY, band);
			roofSignal[1]             = srcImg[roofXY_pixelAddr + 1];                   //img.Pixel(roofX,  roofY, band);
			roofSignal[2]             = srcImg[roofXY_pixelAddr + 2];                   //img.Pixel(roofX,  roofY, band);
								
			xInterpolatedRoof[0]      = (floorSignal[0] * xFloorFrac) + (roofSignal[0] * xRoofFrac);
			xInterpolatedRoof[1]      = (floorSignal[1] * xFloorFrac) + (roofSignal[1] * xRoofFrac);
			xInterpolatedRoof[2]      = (floorSignal[2] * xFloorFrac) + (roofSignal[2] * xRoofFrac);

			dstImg[dstPixelAddr + 0]  = (xInterpolatedFloor[0] * yFloorFrac) + (xInterpolatedRoof[0] * yRoofFrac);
			dstImg[dstPixelAddr + 1]  = (xInterpolatedFloor[1] * yFloorFrac) + (xInterpolatedRoof[1] * yRoofFrac);
			dstImg[dstPixelAddr + 2]  = (xInterpolatedFloor[2] * yFloorFrac) + (xInterpolatedRoof[2] * yRoofFrac);
		}
		else
		{
			dstImg[dstPixelAddr + 0]  = xInterpolatedFloor[0];
			dstImg[dstPixelAddr + 1]  = xInterpolatedFloor[1];
			dstImg[dstPixelAddr + 2]  = xInterpolatedFloor[2];
		}
	}
	else
	{
		if(yRoofFrac > FLT_EPSILON)
		{
			float floorSignal[3];
			float roofSignal[3];

			floorSignal[0]           = srcImg[floorXY_pixelAddr + 0];                   //img.Pixel(floorX, floorY, band);
			floorSignal[1]           = srcImg[floorXY_pixelAddr + 1];                   //img.Pixel(floorX, floorY, band);
			floorSignal[2]           = srcImg[floorXY_pixelAddr + 2];                   //img.Pixel(floorX, floorY, band);
							
			roofSignal[0]            = srcImg[roofXY_pixelAddr - srcShape.nBands + 0];  //img.Pixel(floorX, roofY,  band); //floorX == roofX - 1
			roofSignal[1]            = srcImg[roofXY_pixelAddr - srcShape.nBands + 1];  //img.Pixel(floorX, roofY,  band); //floorX == roofX - 1
			roofSignal[2]            = srcImg[roofXY_pixelAddr - srcShape.nBands + 2];  //img.Pixel(floorX, roofY,  band); //floorX == roofX - 1

			dstImg[dstPixelAddr + 0] = (floorSignal[0] * yFloorFrac) + (roofSignal[0] * yRoofFrac);
			dstImg[dstPixelAddr + 1] = (floorSignal[1] * yFloorFrac) + (roofSignal[1] * yRoofFrac);
			dstImg[dstPixelAddr + 2] = (floorSignal[2] * yFloorFrac) + (roofSignal[2] * yRoofFrac);
		}
		else
		{
			dstImg[dstPixelAddr + 0] = srcImg[floorXY_pixelAddr + 0]; //img.Pixel(floorX, floorY, band);
			dstImg[dstPixelAddr + 1] = srcImg[floorXY_pixelAddr + 1]; //img.Pixel(floorX, floorY, band);
			dstImg[dstPixelAddr + 2] = srcImg[floorXY_pixelAddr + 2]; //img.Pixel(floorX, floorY, band);
		}
	}
}


void ImageProcessing::Rescale(CFloatImage &image, float newMin, float newMax)
{
	CShape imgShape = image.Shape();
	float oldMin    =  FLT_MAX;
	float oldMax    = -FLT_MAX;

	uint pixelAddr = 0;
	for(int imageY = 0; imageY < imgShape.height; imageY++)
	{		
		for(int imageX = 0; imageX < imgShape.width; imageX++)
		{
			for(int imageB = 0; imageB < imgShape.nBands; imageB++, pixelAddr++)
			{
				float val = image[pixelAddr];
				if(oldMin > val) oldMin = val;
				if(oldMax < val) oldMax = val;
			}
		}
	}

	if((oldMin == newMin) &&
	   (oldMax == newMax))
	{
		return;
	}	

	float oldRange = oldMax - oldMin;
	float newRange = newMax - newMin;
	if(oldRange == 0)
	{
		oldRange = 1;
		newRange = 1;
	}

	pixelAddr = 0;
	float rangeRatio = newRange / oldRange;
	for(int imageY = 0; imageY < imgShape.height; imageY++)
	{		
		for(int imageX = 0; imageX < imgShape.width; imageX++)
		{
			for(int imageB = 0; imageB < imgShape.nBands; imageB++, pixelAddr++)
			{
				float oldVal     = image[pixelAddr];
				float newVal     = ((oldVal - oldMin) * rangeRatio) + newMin;
				image[pixelAddr] = newVal;
			}
		}
	}
}

float ImageProcessing::GetMean(CFloatImage &src, int band, const CByteImage &mask, bool useMask)
{
	float mean = 0.0f;	

	CShape imageShape = src.Shape();
	ENSURE(imageShape.depth == 1);
	if(useMask == true)
	{
		ENSURE(imageShape.SameIgnoringNBands(mask.Shape())); 
		ENSURE(mask.Shape().nBands == 1);
	}

	int samples       = 0; 
	uint pixelAddr    = 0;
	uint nodeAddr     = 0;
	for(int imageY = 0; imageY < imageShape.height; imageY++)
	for(int imageX = 0; imageX < imageShape.width; imageX++, pixelAddr += imageShape.nBands, nodeAddr++)
	{
		if((useMask == true) && (mask[nodeAddr] == MASK_INVALID))
		{
			continue;
		}
		float val = src[pixelAddr + band];
		mean += val;
		samples++;
	}
	ENSURE(samples > 0);
	mean /= samples;

	return mean;
}

void ImageProcessing::GetMeanAndDev(CFloatImage &src, int band, float &mean, float &stdDev, const CByteImage &mask, bool useMask)
{	
	mean = 0;	
	stdDev = 0;

	CShape imageShape = src.Shape();
	ENSURE(imageShape.depth == 1);
	if(useMask == true)
	{
		ENSURE(imageShape.SameIgnoringNBands(mask.Shape())); 
		ENSURE(mask.Shape().nBands == 1);
	}

	int samples       = 0; 
	uint pixelAddr    = 0;
	uint nodeAddr     = 0;
	for(int imageY = 0; imageY < imageShape.height; imageY++)
	{
		for(int imageX = 0; imageX < imageShape.width; imageX++, pixelAddr += imageShape.nBands, nodeAddr++)
		{
			if((useMask == true) && (mask[nodeAddr] == MASK_INVALID))
			{
				continue;
			}
			float val = src[pixelAddr + band];
			mean += val;
			samples++;
		}
	}
	ENSURE(samples > 0);
	mean /= samples;

	pixelAddr = 0;
	nodeAddr  = 0;
	for(int imageY = 0; imageY < imageShape.height; imageY++)
	{
		for(int imageX = 0; imageX < imageShape.width; imageX++, pixelAddr += imageShape.nBands, nodeAddr++)
		{
			if((useMask == true) && (mask[nodeAddr] == MASK_INVALID))
			{
				continue;
			}
			float val = (src[pixelAddr + band] - mean);
			stdDev += (val * val);
		}
	}
	stdDev = sqrt(stdDev / samples);
}

void ImageProcessing::RemapMeanAndDev(CFloatImage &src, int band, float newMean, float newStdDev, const CByteImage &mask, bool useMask)
{
	ENSURE(newStdDev != 0);	
	CShape imageShape = src.Shape();
	ENSURE(imageShape.depth == 1);
	if(useMask == true)
	{
		ENSURE(imageShape.SameIgnoringNBands(mask.Shape())); 
		ENSURE(mask.Shape().nBands == 1);
	}

	float oldMean   = 0;	
	float oldStdDev = 0;
	ImageProcessing::GetMeanAndDev(src, band, oldMean, oldStdDev, mask, useMask);

	if(oldStdDev == 0)
	{
		oldStdDev = newStdDev;
	}
	if((oldMean == newMean) && (oldStdDev == newStdDev))
	{
		return;
	}
	
	float stdDevRatio = newStdDev / oldStdDev;
	uint pixelAddr    = 0;
	uint nodeAddr     = 0;
	for(int imageY = 0; imageY < imageShape.height; imageY++)
	for(int imageX = 0; imageX < imageShape.width; imageX++, pixelAddr += imageShape.nBands, nodeAddr++)
	{
		if((useMask == true) && (mask[nodeAddr] == MASK_INVALID))
		{
			continue;
		}
		float oldPixelVal = src[pixelAddr + band];
		src[pixelAddr + band] = (stdDevRatio * (oldPixelVal - oldMean)) + newMean;
	}
}

CByteImage ImageProcessing::getNullMask(const CFloatImage &img, const CByteImage &imgMask, bool useMask)
{
	CShape imgShape = img.Shape();
	if(useMask == true)
	{
		ENSURE(imgShape.SameIgnoringNBands(imgMask.Shape()));
	}
	CByteImage mask(imgShape.width, imgShape.height, 1);
	mask.ClearPixels();

	uint pixelAddr = 0;
	uint nodeAddr  = 0;
	for(int imageY = 0; imageY < imgShape.height; imageY++)
	{
		for(int imageX = 0; imageX < imgShape.width; imageX++, nodeAddr++, pixelAddr += imgShape.nBands)
		{   
			if(useMask == true)
			{
				if(imgMask[nodeAddr] == MASK_INVALID) continue;
			}
			bool nullPixel = true;
			for(int iBand = 0; iBand < imgShape.nBands; iBand++)
			{
				if(img[pixelAddr + iBand] != 0)
				{
					nullPixel = false;
					break;
				}
			}
			if(nullPixel == false)
			{
				mask[nodeAddr] = MASK_VALID;
			}
		}
	}

	return mask;
}

CFloatImage ImageProcessing::TransferColor(const CFloatImage &target, const CFloatImage &src, 									   
										   CByteImage targetMask, bool useTargetMask,
										   CByteImage srcMask,    bool useSrcMask)
{
	ENSURE(target.Shape().nBands == 3);
	ENSURE(src.Shape().nBands == 3);

	srcMask    = getNullMask(src, srcMask, useSrcMask);
	targetMask = getNullMask(target, targetMask, useTargetMask);

	CFloatImage srcLAB(src.Shape());
	ColorHelper::RGBtoLAB(srcLAB, src);

	CFloatImage targetLAB(target.Shape());
	ColorHelper::RGBtoLAB(targetLAB, target);

	for(int iBand = 0; iBand < 3; iBand++)
	{
		float mean = 0;
		float std = 0;
		ImageProcessing::GetMeanAndDev(srcLAB, iBand, mean, std, srcMask, true);
		ImageProcessing::RemapMeanAndDev(targetLAB, iBand, mean, std, targetMask, true);
	}

	CFloatImage result(targetLAB.Shape());
	ColorHelper::LABtoRGB(result, targetLAB);

	return result;
}

CFloatImage ImageProcessing::RemoveHue(CFloatImage &src)
{
	ENSURE(src.Shape().nBands == 3);

	CFloatImage srcLAB(src.Shape());
	ColorHelper::RGBtoLAB(srcLAB, src);
	for(int iBand = 1; iBand < 3; iBand++)
	{
		float mean = 0;
		float std = 0;
		ImageProcessing::GetMeanAndDev(srcLAB, iBand, mean, std);
		ImageProcessing::RemapMeanAndDev(srcLAB, iBand, 0, std);
	}
	CFloatImage result(src.Shape());
	ColorHelper::LABtoRGB(result, srcLAB);

	return result;
}

CFloatImage ImageProcessing::GetFilteredImage(CFloatImage &image,     CByteImage &imageMask,  bool useImgMask, 
											  CFloatImage &sourceImg, CByteImage &sourceMask, bool useSrcMask,
											  ImageFilter filterType)
{
	CFloatImage filteredImage;
	switch(filterType)
	{
		case IF_LAB_COLOR_TRANSFER:
			filteredImage = ImageProcessing::TransferColor(image, sourceImg, imageMask, useImgMask, sourceMask, useSrcMask);
			break;
		case IF_NO_OP:
			filteredImage = image.Clone();
			break;
		default:
			REPORT_FAILURE("Unknown registration filter\n");
			break;
	};

	return filteredImage;
}


CFloatImage ImageProcessing::GetLabelsImage(CImageOf<short> &labels)
{
	CShape maskShape = labels.Shape();
	CFloatImage labelsImg(maskShape);
	ENSURE(maskShape.nBands == 1);
	ENSURE(maskShape.depth == 1);

    uint nodeAddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	{
		for(int x = 0; x < maskShape.width; x++, nodeAddr++)
		{
			labelsImg[nodeAddr] = labels[nodeAddr];
		}
	}

	ImageProcessing::Rescale(labelsImg, 0, 1);
	return labelsImg;	
}

CFloatImage ImageProcessing::GetSourcesImage(CImageOf<short> &labels, int sourceCount, 
									 	     const vector<CVector3> &srcColors, bool useSrcColors)
{
	ENSURE(sourceCount >= 1);

	CShape maskShape = labels.Shape();
	ENSURE(maskShape.nBands == 1);
	ENSURE(maskShape.depth == 1);

	CFloatImage labelsImg(maskShape.width, maskShape.height, 3);

	if(useSrcColors)
	{
		INSIST(srcColors.size() == sourceCount);		
	}
	
    uint pixelAddr = 0;
	uint nodeAddr  = 0;
	for(int y = 0; y < maskShape.height; y++)
	for(int x = 0; x < maskShape.width; x++, pixelAddr += 3, nodeAddr++)
	{
		int iLabel = labels[nodeAddr];
		ENSURE(iLabel >= -1);
		ENSURE(iLabel < sourceCount);		

		if((useSrcColors == false) ||
		   (iLabel == -1))
		{
			for(int iBand = 0; iBand < 3; iBand++)
			{
				labelsImg[pixelAddr + iBand] = (labels[nodeAddr] + 1.0f) / sourceCount;
			}
		}
		else
		{
			for(int iBand = 0; iBand < 3; iBand++)
			{
				labelsImg[pixelAddr + iBand] = (float) srcColors[iLabel][iBand];
			}
		}
	}

	return labelsImg;
}



CFloatImage ImageProcessing::GaussianBlur(CFloatImage &src, int kernelSize, float stdDev, const CByteImage &mask , bool useMask)
{
	if(useMask == true)
	{
		ENSURE(src.Shape().SameIgnoringNBands(mask.Shape()));
		ENSURE(mask.Shape().nBands == 1);
	}

	CFloatImage gKernelX = MathHelper::CreateGuassianKernel1D(MathHelper::X_AXIS, kernelSize, stdDev);
	CFloatImage gKernelY = MathHelper::CreateGuassianKernel1D(MathHelper::Y_AXIS, kernelSize, stdDev);

	CFloatImage halfBlurImage(src.Shape());	
	ImageProcessing::Convolution(halfBlurImage, src, gKernelX, mask, useMask);
	CFloatImage blurImage(src.Shape());
	ImageProcessing::Convolution(blurImage, halfBlurImage, gKernelY, mask, useMask);

	return blurImage;
}

void ImageProcessing::PrintImageStats(CFloatImage &src, const CByteImage &mask, bool useMask)
{
	CShape imgShape = src.Shape();
	if(useMask == true)
	{
		ENSURE(imgShape.SameIgnoringNBands(mask.Shape()));
		ENSURE(mask.Shape().nBands == 1);
	}		

	PRINT_SHAPE(imgShape);

	float min =  FLT_MAX;
	float max = -FLT_MAX;
	uint pixelAddr = 0;
	uint maskAddr = 0;
	for(int imageY = 0; imageY < imgShape.height; imageY++)
	{		
		for(int imageX = 0; imageX < imgShape.width; imageX++, pixelAddr += imgShape.nBands, maskAddr++)
		{
			if((useMask == true) && (mask[maskAddr] == MASK_INVALID))
			{
				continue;
			}

			for(int imageB = 0; imageB < imgShape.nBands; imageB++)
			{
				float val = src[pixelAddr + imageB];
				if(min > val) min = val;
				if(max < val) max = val;
			}
		}
	}

	PRINT_FLOAT(min);
	PRINT_FLOAT(max);

	for(int imageB = 0; imageB < imgShape.nBands; imageB++, pixelAddr++)
	{
		printf("Channel: %i\n", imageB);
		float mean   = 0;
		float stdDev = 0;
		GetMeanAndDev(src, imageB, mean, stdDev, mask, useMask);
		printf("\t %f %f\n", mean, stdDev);
	}
}

CFloatImage ImageProcessing::CrossBilateralFilter(CFloatImage &noFlash, 
													 CFloatImage &flash, 
													 CFloatImage &detailImg, 
													 float spaceD, float sigD, int halfWidth)
{
	CShape imgShape = noFlash.Shape();
	ENSURE(imgShape == flash.Shape());
	CFloatImage resultImg(imgShape);

	float spaceVar = -2.0f * spaceD * spaceD;
	float sigVar   = -2.0f * sigD * sigD;

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	{
		for(int x = 0; x < imgShape.width; x++, nodeAddr++)
		{
			for(int channel = 0; channel < imgShape.nBands; channel++, pixelAddr++)
			{
				float flashChannelVal = flash[pixelAddr];

				float weightSum = 0;
				float sigSum = 0;
				for(int yy = y - halfWidth; yy <= y + halfWidth; yy++)
				{
					for(int xx = x - halfWidth; xx <= x + halfWidth; xx++)
					{
						if(imgShape.InBounds(xx, yy) == false) continue;

						float neighbourFlashChannelVal = flash.Pixel(xx, yy, channel);
						float neighbourChannelVal = noFlash.Pixel(xx, yy, channel);

						float numerator = (float) ((xx - x) * (xx - x)) + ((yy - y) * (yy - y));
						float spaceWeight = exp(numerator / spaceVar);

						numerator = (neighbourFlashChannelVal - flashChannelVal) * 
								    (neighbourFlashChannelVal - flashChannelVal);
						float sigWeight = exp(numerator / sigVar);

						float currWeight = spaceWeight * sigWeight;
						//float currWeight = spaceWeight;
						weightSum += currWeight;
						sigSum += currWeight * neighbourChannelVal;
					}
				}
				resultImg[pixelAddr] = (sigSum / weightSum) * detailImg[pixelAddr];
			}			
		}		

		int currProgress = (int) ((y / (float)imgShape.height) * 10.0f);
		int prevProgress = (int) (((y - 1) / (float)imgShape.height) * 10.0f);
		if(prevProgress != currProgress)
		{
			printf(".");
			fflush(stdout);
		}		
	}

	printf("\n");

	return resultImg;
}


CFloatImage ImageProcessing::BilateralFilter(CFloatImage &img, 
											 float spaceD, float sigD, int halfWidth)
{
	CShape imgShape = img.Shape();
	CFloatImage resultImg(imgShape);


	float spaceVar = -2.0f * spaceD * spaceD;
	float sigVar   = -2.0f * sigD * sigD;

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	{
		for(int x = 0; x < imgShape.width; x++, nodeAddr++)
		{
			for(int channel = 0; channel < imgShape.nBands; channel++, pixelAddr++)
			{
				float currChannelVal = img[pixelAddr];

				float weightSum = 0;
				float sigSum = 0;

				for(int yy = y - halfWidth; yy <= y + halfWidth; yy++)
				{
					for(int xx = x - halfWidth; xx <= x + halfWidth; xx++)
					{
						if(imgShape.InBounds(xx, yy) == false) continue;

						float neighbourChannelVal = img.Pixel(xx, yy, channel);

						float numerator = (float) ((xx - x) * (xx - x)) + ((yy - y) * (yy - y));
						float spaceWeight = exp(numerator / spaceVar);						
						numerator = (neighbourChannelVal - currChannelVal) * (neighbourChannelVal - currChannelVal);
						float sigWeight = exp(numerator / sigVar);

						float currWeight = spaceWeight * sigWeight;
						//float currWeight = spaceWeight;
						weightSum += currWeight;
						sigSum += currWeight * neighbourChannelVal;
					}
				}
				resultImg[pixelAddr] = sigSum / weightSum;
			}
		}
		int currProgress = (int) ((y / (float)imgShape.height) * 10.0f);
		int prevProgress = (int) (((y - 1) / (float)imgShape.height) * 10.0f);
		if(prevProgress != currProgress)
		{
			printf(".");
			fflush(stdout);
		}
	}

	printf("\n");

	return resultImg;
}


/*
int main(int args, char *argv[])
{
	if(args != 7)
	{
		printf("[usage] %s no-flash-img flash-img spaceStdDev sigStdDev kernelsize out-img\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	CFloatImage noFlashImg;
	CFloatImage flashImg;
	CFloatImage outImg;

	ImageIO::ReadFile(noFlashImg, argv[1]);
	ImageIO::ReadFile(flashImg, argv[2]);

	float spaceStdDev = (float) atof(argv[3]);
	float sigStdDev = (float) atof(argv[4]);
	int kernelSize = atoi(argv[5]);

	int kernelWidth = kernelSize * 2 + 1;


	CFloatImage flashBase = ImageProcessing::BilateralFilter(flashImg, 
															 spaceStdDev, 
															 sigStdDev, 
															 kernelWidth);
	CFloatImage detailImg = flashImg.Clone();
	ImageProcessing::Operate(flashBase, flashBase, 0.02f, ImageProcessing::OP_ADD);
	ImageProcessing::Operate(detailImg, detailImg, 0.02f, ImageProcessing::OP_ADD);
	ImageProcessing::Operate(detailImg, detailImg, flashBase, ImageProcessing::OP_DIV);

	outImg = ImageProcessing::CrossBilateralFilter(noFlashImg, flashImg, detailImg,
												   spaceStdDev, 
												   sigStdDev, 
												   kernelWidth);

	printf("\n");

	ImageIO::WriteFile(outImg, argv[6]);

	return EXIT_SUCCESS;
}
//*/

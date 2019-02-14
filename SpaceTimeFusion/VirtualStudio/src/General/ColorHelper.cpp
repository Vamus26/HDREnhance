#include "ColorHelper.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include "ColorHelper.h"

CTransform3x3 ColorHelper::rgbTolmsMAT;
CTransform3x3 ColorHelper::lmsTolabMAT;
CTransform3x3 ColorHelper::lmsTorgbMAT;
CTransform3x3 ColorHelper::labTolmsMAT;

ColorHelper::_InitColorHelper ColorHelper::_InitColorHelper::init;



CFloatImage ColorHelper::RGBtoLuminance(CFloatImage &src)
{
	CShape imgShape = src.Shape();
	if(imgShape.nBands == 1)
	{
		return src;
	}
	else
	{
		ENSURE(imgShape.nBands == 3);
	}
	CFloatImage lumImage(imgShape.width, imgShape.height, 1);

	uint colorPixelAddr = 0;
	uint lumPixelAddr   = 0;
	for(int y = 0; y < imgShape.height; y++)
	{
		for(int x = 0; x < imgShape.width; x++, colorPixelAddr += imgShape.nBands, lumPixelAddr++)
		{
			float lum = 0;
			lum += src[colorPixelAddr + 0] * 0.299f;
			lum += src[colorPixelAddr + 1] * 0.587f;
			lum += src[colorPixelAddr + 2] * 0.114f;
			lumImage[lumPixelAddr] = lum;
		}
	}

	return lumImage;
}

void ColorHelper::RGBtoHSV(CFloatImage &dstHSV_Img, const CFloatImage &srcRGB_Img)
{
	CShape imgShape = srcRGB_Img.Shape();
	ENSURE(imgShape == dstHSV_Img.Shape());
	ENSURE(imgShape.nBands == 3);

	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, pixelAddr += imgShape.nBands)
	{ 
		ColorHelper::RGBtoHSV(srcRGB_Img[pixelAddr + 0],  srcRGB_Img[pixelAddr + 1],  srcRGB_Img[pixelAddr + 2],
							  &dstHSV_Img[pixelAddr + 0], &dstHSV_Img[pixelAddr + 1], &dstHSV_Img[pixelAddr + 2]);
	}
}

void ColorHelper::HSVtoRGB(CFloatImage &dstRGB_Img, const CFloatImage &srcHSV_Img)
{
	CShape imgShape = srcHSV_Img.Shape();
	ENSURE(imgShape == dstRGB_Img.Shape());
	ENSURE(imgShape.nBands == 3);

	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, pixelAddr += imgShape.nBands)
	{ 
		ColorHelper::HSVtoRGB(&dstRGB_Img[pixelAddr + 0], &dstRGB_Img[pixelAddr + 1], &dstRGB_Img[pixelAddr + 2],
							   srcHSV_Img[pixelAddr + 0],  srcHSV_Img[pixelAddr + 1],  srcHSV_Img[pixelAddr + 2]);
	}
}

void ColorHelper::RGBtoLAB(CFloatImage &dst, const CFloatImage &src)
{
	CShape imgShape = dst.Shape();
	ENSURE(imgShape == src.Shape());
	ENSURE(imgShape.nBands == 3);

	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	{
		for(int x = 0; x < imgShape.width; x++, pixelAddr += imgShape.nBands)
		{
			CVector3 rgbVec;
			rgbVec[0] = src[pixelAddr + 0];
			rgbVec[1] = src[pixelAddr + 1];
			rgbVec[2] = src[pixelAddr + 2];

			CVector3 lmsVec = ColorHelper::rgbTolmsMAT * rgbVec;

			//if(abs(lmsVec[0]) < FLT_EPSILON) lmsVec[0] = FLT_EPSILON; 
			//if(abs(lmsVec[1]) < FLT_EPSILON) lmsVec[1] = FLT_EPSILON;
			//if(abs(lmsVec[2]) < FLT_EPSILON) lmsVec[2] = FLT_EPSILON;
			//lmsVec[0] = log10(lmsVec[0]);
			//lmsVec[1] = log10(lmsVec[1]);
			//lmsVec[2] = log10(lmsVec[2]);

			CVector3 labVec = ColorHelper::lmsTolabMAT * lmsVec;
			dst[pixelAddr + 0] = (float) labVec[0];
			dst[pixelAddr + 1] = (float) labVec[1];
			dst[pixelAddr + 2] = (float) labVec[2];
		}
	}
}

void ColorHelper::LABtoRGB(CFloatImage &dst, const CFloatImage &src)
{
	CShape imgShape = dst.Shape();
	ENSURE(imgShape == src.Shape());
	ENSURE(imgShape.nBands == 3);

	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	{
		for(int x = 0; x < imgShape.width; x++, pixelAddr += imgShape.nBands)
		{
			CVector3 labVec;
			labVec[0] = src[pixelAddr + 0];
			labVec[1] = src[pixelAddr + 1];
			labVec[2] = src[pixelAddr + 2];

			CVector3 lmsVec = ColorHelper::labTolmsMAT * labVec;

			//lmsVec[0] = pow(10.0, lmsVec[0]);
			//lmsVec[1] = pow(10.0, lmsVec[1]);
			//lmsVec[2] = pow(10.0, lmsVec[2]);

			CVector3 rgbVec = ColorHelper::lmsTorgbMAT * lmsVec;
			dst[pixelAddr + 0] = (float) rgbVec[0];
			dst[pixelAddr + 1] = (float) rgbVec[1];
			dst[pixelAddr + 2] = (float) rgbVec[2];
		}
	}
}


void ColorHelper::RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min, max, delta;

	min = std::min(std::min(r, g), b);
	max = std::max(std::max(r, g), b);
	*v = max;				// v

	delta = max - min;

	if( max != 0 )
		*s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}

	if( r == max )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan

	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;

}

void ColorHelper::HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;			// sector 0 to 5
	i = (int) floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}

}
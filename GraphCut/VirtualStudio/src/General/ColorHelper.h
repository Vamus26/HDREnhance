#ifndef __COLOR_HELPER_H__
#define __COLOR_HELPER_H__

#include "Transform-3x3.h"
#include "Image.h"

class ColorHelper
{	
	class _InitColorHelper {	
		_InitColorHelper() 
		{
			ColorHelper::rgbTolmsMAT[0][0] =  0.3811; ColorHelper::rgbTolmsMAT[0][1] =  0.5783; ColorHelper::rgbTolmsMAT[0][2] =  0.0402;
			ColorHelper::rgbTolmsMAT[1][0] =  0.1967; ColorHelper::rgbTolmsMAT[1][1] =  0.7244; ColorHelper::rgbTolmsMAT[1][2] =  0.0782;
			ColorHelper::rgbTolmsMAT[2][0] =  0.0241; ColorHelper::rgbTolmsMAT[2][1] =  0.1288; ColorHelper::rgbTolmsMAT[2][2] =  0.8444;

			ColorHelper::lmsTolabMAT[0][0] =  0.5774; ColorHelper::lmsTolabMAT[0][1] =  0.5774; ColorHelper::lmsTolabMAT[0][2] =  0.5774;
			ColorHelper::lmsTolabMAT[1][0] =  0.4082; ColorHelper::lmsTolabMAT[1][1] =  0.4082; ColorHelper::lmsTolabMAT[1][2] = -0.8165;
			ColorHelper::lmsTolabMAT[2][0] =  0.7071; ColorHelper::lmsTolabMAT[2][1] = -0.7071; ColorHelper::lmsTolabMAT[2][2] =  0.0000;

			ColorHelper::labTolmsMAT[0][0] =  0.5774; ColorHelper::labTolmsMAT[0][1] =  0.4082; ColorHelper::labTolmsMAT[0][2] =  0.7071;
			ColorHelper::labTolmsMAT[1][0] =  0.5774; ColorHelper::labTolmsMAT[1][1] =  0.4082; ColorHelper::labTolmsMAT[1][2] = -0.7071;
			ColorHelper::labTolmsMAT[2][0] =  0.5774; ColorHelper::labTolmsMAT[2][1] = -0.8165; ColorHelper::labTolmsMAT[2][2] =  0.0000;

			ColorHelper::lmsTorgbMAT[0][0] =  4.4679; ColorHelper::lmsTorgbMAT[0][1] = -3.5873; ColorHelper::lmsTorgbMAT[0][2] =  0.1193;
			ColorHelper::lmsTorgbMAT[1][0] = -1.2186; ColorHelper::lmsTorgbMAT[1][1] =  2.3809; ColorHelper::lmsTorgbMAT[1][2] = -0.1624;
			ColorHelper::lmsTorgbMAT[2][0] =  0.0497; ColorHelper::lmsTorgbMAT[2][1] = -0.2439; ColorHelper::lmsTorgbMAT[2][2] =  1.2045;
		}
		static _InitColorHelper init;
	};

public:	
	static CTransform3x3 rgbTolmsMAT;
	static CTransform3x3 lmsTolabMAT;
	static CTransform3x3 labTolmsMAT;
	static CTransform3x3 lmsTorgbMAT;

	static void RGBtoLAB(CFloatImage &dst, const CFloatImage &src);
	static void LABtoRGB(CFloatImage &dst, const CFloatImage &src);

	static void RGBtoHSV(CFloatImage &dstHSV_Img, const CFloatImage &srcRGB_Img);
	static void HSVtoRGB(CFloatImage &dstRGB_Img, const CFloatImage &srcHSV_Img);

	static CFloatImage RGBtoLuminance(CFloatImage &src);

	static void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v );
	static void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );
};

#endif



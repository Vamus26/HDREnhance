#ifndef __MATH_HELPER_H__
#define __MATH_HELPER_H__
#include "Definitions.h"
#include "Image.h"
#include <float.h>

class MathHelper
{
public:

	typedef enum
	{
		X_AXIS,
		Y_AXIS
	} Axis;


	static CFloatImage CreateGuassianKernel2D(int kernelSize, float stdDev);

	static CFloatImage CreateGuassianKernel1D(Axis axis, int kernelSize, float stdDev);	

	static void NormalizeImageIntegral(CFloatImage &image);

	//source-paper: reconstruction filters in computer graphics
	static double MitchellKernelVal(float dist)
	{
		static const double b = 1/3.0; 
		static const double c = 1/3.0; 		

		if(dist < 0.0f)
		{
			dist = -dist;
		}
		
		if(dist >= 2)
		{
			return 0.0;
		}

		double val;
		if(dist < 1)
		{
			val  = (12 - 9*b - 6*c)   * (dist * dist * dist);
			val += (-18 + 12*b + 6*c) * (dist * dist);
			val += (6 - 2*b);
		}
		else
		{
			val  = (-b - 6*c)     * (dist * dist * dist);
			val += (6*b + 30*c)   * (dist * dist);
			val += (-12*b - 48*c) * dist;
			val += (8*b + 24*c);
		}
		val = val / 6;

		return val;
	}

};

#endif



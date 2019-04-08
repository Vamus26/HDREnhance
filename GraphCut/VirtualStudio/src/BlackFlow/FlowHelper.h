#ifndef __FLOW_HELPER_H__
#define __FLOW_HELPER_H__
#pragma warning(disable:4239)

#include "Image.h"
#include "ImageProcessing.h"

const float FlowHelper_INVALID_FLOW = sqrt(FLT_MAX);

class FlowHelper
{
public:

	static void WriteFlowField(const char *fname, 
							   CFloatImage &flowField);

	static void WriteFlowField(string fname, 
							   CFloatImage &flowField)
	{
		WriteFlowField(fname.c_str(), flowField);
	}

	static void ReadFlowField(const char *fname, 
							  CFloatImage &flowField,
							  float scale = 1.0f);

	static void ReadFlowField(string fname, 
							  CFloatImage &flowField, float scale = 1.0f)
	{
		ReadFlowField(fname.c_str(), flowField, scale);
	}

	template <class T>
	static void WarpImage(CImageOf<T> &warppedImg,  CByteImage &warppedMask, 
						  CImageOf<T> &sourceImage, CByteImage &sourceMask, bool useSourceMask,
						  CFloatImage &flowField,
						  bool useSampling = true,
						  T defaultVal = 0);

	template <class T>
	static void WarpImage(CImageOf<T> &warppedImg,  CByteImage &warppedMask, 
						  CImageOf<T> &sourceImage, 
						  CFloatImage &flowField,
						  bool useSampling = true,
						  T defaultVal = 0);

	static void ComputeFlow(CFloatImage &prevFrame, 
							CFloatImage &currFrame,
							CFloatImage &flowField);

private:
	static void copyFrameIntensity(float *buff, CFloatImage &img);
	
	static void setFlowField(float *u, float *v, CFloatImage &errImg, CFloatImage &flowField);

	static void setScalarField(float *field, CFloatImage &img);
};

#include "FlowHelper.inl"

#endif

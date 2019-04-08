#ifndef __MVFLOW_H__
#define __MVFLOW_H__

#include "Definitions.h"
#include "Image.h"
#include "Transform-3x3.h"
#include "Transform-2x2.h"

typedef Transform3x3<float> Tensor;
typedef CImageOf<Tensor> CTensorImage;

const static float MVFLOW_PSI_ED = 0.1f;
const static float MVFLOW_PSI_ED_SQR = (MVFLOW_PSI_ED * MVFLOW_PSI_ED);

const static float MVFLOW_PSI_ES = 0.001f;
const static float MVFLOW_PSI_ES_SQR = (MVFLOW_PSI_ES * MVFLOW_PSI_ES);

class MVFlow
{
public:
	typedef struct _MVFlowParams
	{
		float alphaBC;
		float alphaGC;
		float beta;
		static _MVFlowParams Default();
		void Dump(string prefix);
	}MVFlowParams;

private:
	MVFlowParams params;

	CFloatImage currImg;
	CFloatImage nextImg;

	CFloatImage warpImg;

	CFloatImage u;
	CFloatImage v;
	CFloatImage du;
	CFloatImage dv;

	CFloatImage Ix;
	CFloatImage Iy;
	CFloatImage Iz;

	CFloatImage Ixx;
	CFloatImage Ixy;
	CFloatImage Iyy;

	CFloatImage Ixz;	
	CFloatImage Iyz;

	CFloatImage currImgDX;
	CFloatImage currImgDY;

	CTensorImage tensorS;
	CTensorImage tensorT;

	CFloatImage psiDiff_D1;
	CFloatImage psiDiff_D2;
	CFloatImage psiDiff_S;


public:
	MVFlow(CFloatImage currImage, CFloatImage nextImage);

	void ComputeFlow(MVFlowParams mvFlowParams);

private:
	void allocIntermediateData();

private: //MVFlow-Intermediate
	void resetUV();
	void reset_dU_dV();
	void initCurrImgDXY();
	void updateUV();

	void computeDataIterK();

	void computeWarpImg();

	void computeGradImgs(const CFloatImage &srcImg, 
						 CFloatImage &gradX,
						 CFloatImage &gradY);

	void computeTensors();	

private: //MVFlow-solver.cpp
	void computeGauB_SeidelIter();
	Transform2x2<double> computeMatA(int x, int y);
	Vector2<double> computeRHS(int x, int y);
	void computePsiDiffImgs();

private:
	void DumpDebugImgs();

private:
	template <class T>
	T reflectCoord(const T coord, const T upperLimit)
	{
		if(coord < 0.0f)
			return -coord;

		T overFlow = coord - upperLimit;
		if(overFlow > 0)
			return upperLimit - overFlow;
		else 
			return coord;
	}

	float psiDiffS(float sSqr)
	{
		ENSURE(sSqr > -MVFLOW_PSI_ES_SQR);
		return 1.0f / 2.0f * sqrt(sSqr + MVFLOW_PSI_ES_SQR);
	}

	float psiDiffD(float sSqr)
	{
		ENSURE(sSqr > -MVFLOW_PSI_ED_SQR);
		return 1.0f / 2.0f * sqrt(sSqr + MVFLOW_PSI_ED_SQR);
	}

};

#endif
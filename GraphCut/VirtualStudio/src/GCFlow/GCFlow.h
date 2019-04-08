#ifndef __GCFLOW_H__
#define __GCFLOW_H__

#include "Definitions.h"
#include "Image.h"
#include "Transform-3x3.h"
#include "Transform-2x2.h"
#include "energy.h"

const float GCFLOW_DATA_COST_INF = 1e+10F;
const float GCFLOW_EDGE_COST_INF = 1e+05F;

const int GCFLOW_Connectivity     = 2;
const int GCFLOW_DX[GCFLOW_Connectivity] = {-1,  0};
const int GCFLOW_DY[GCFLOW_Connectivity] = { 0, -1};

class GCFlow
{
public:
	typedef struct _GCFlowParams
	{
		float alphaBC;
		float alphaGC;
		float beta;
		int startU;
		int endU;
		int startV;
		int endV;
		int maxIterations;
		static _GCFlowParams Default();
		void Dump(string prefix);
	}GCFlowParams;

private:
	GCFlowParams params;

	CFloatImage currImg;
	CFloatImage nextImg;

	CImageOf<IntPair> labelImage;
	CImageOf<Energy::Var> graphNodes;

	CShape imgShape;
	CShape maskShape;

public:
	GCFlow(CFloatImage currImage, CFloatImage nextImage);

	void ComputeFlow(GCFlowParams gcFlowParams, CFloatImage &uImg, CFloatImage &vImg);

private:

	bool expand(IntPair label);

	double computeCurrEnergy();
	float dataCost(int x, int y, IntPair label);
	float edgeCost(IntPair label1, IntPair label2);

private:
};

#endif
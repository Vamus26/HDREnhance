#ifndef __PIECES_BLENDER_H__
#define __PIECES_BLENDER_H__

#pragma warning(disable : 4512)
#pragma warning(disable : 4239)

#include "RegistrationDefs.h"
#include "energy.h"
#include "GC_Stitch_Defs.h"

class PiecesBlender
{
	CShape      imgShape;
	CShape      maskShape;

	CFloatImage &objectiveImage;
	RegisteredPieces &pieces;

	bool					  useDisparityObj;
	const CImageOf<short>     &disparitySources;
	const CFloatImage         &disparityMap;
    const vector<CFloatImage> &sourceImages;
	const vector<Images>      &interSourceDisparities;
	
	CImageOf<Energy::Var> graphNodes;
	CImageOf<short>       labelImage;
	CByteImage            spaceMask;

	RegisteredPiece       objPiece;

	double currentEnergy;

public:	
	typedef struct _PiecesBlenderParams
	{
		bool  useGradCosts;
		float temporalCostScale;
		float dataCostScale;
		float energyEpsilon;
		int   maxIterations;
		float occlusionCost;
		float occPropCost;

		static _PiecesBlenderParams Default()
		{
			_PiecesBlenderParams params;
			params.useGradCosts         = true;
			params.dataCostScale        = 1.0f;
			params.temporalCostScale    = 1.0f;
			params.energyEpsilon        = 0.2f;
			params.maxIterations        = 5;
			params.occlusionCost        = 0.5f;
			params.occPropCost          = 2;
			return params;
		}

		void Dump(const string prefix)
		{
			printf("%sPiecesBlender Options:\n", prefix.c_str());
			if(useGradCosts == false)
			{
				printf("%s  useGradCosts         = false\n", prefix.c_str());
			}
			else
			{
				printf("%s  useGradCosts         = true\n",  prefix.c_str());
			}
			printf("%s  dataCostScale        = %f\n", prefix.c_str(), dataCostScale);
			printf("%s  temporalCostScale    = %f\n", prefix.c_str(), temporalCostScale);
			printf("%s  maxIterations        = %i\n", prefix.c_str(), maxIterations);
		}
	} PiecesBlenderParams;

	PiecesBlender(CFloatImage &targetImage, RegisteredPieces &registeredPieces,
				  const CImageOf<short> &disparitySources = CImageOf<short>(), 
				  const CFloatImage &disparityMap = CFloatImage(), 
				  const vector<CFloatImage> &sourceImages = vector<CFloatImage>(), 
				  const vector<Images> &interSourceDisparities = vector<Images>(),
				  bool useDisparityObj = false);

	void Blend(CFloatImage &dstImg, 
			   CImageOf<short> &dstLabels,
			   PiecesBlender::PiecesBlenderParams params = PiecesBlender::PiecesBlenderParams::Default());

	static void ComputeFinalBlend(CFloatImage &dst,
													const CImageOf<short> &labelImage,
													const RegisteredPieces &pieces, 
													const CByteImage &dstMask = CByteImage(), bool fillMask = false);


private:

	void initLabels();
	
	inline float dataCostTemporal(RegisteredPiece &piece, uint nodeAddr, const PiecesBlenderParams params);

	inline float dataCost(uint pixelAddr, uint nodeAddr, short label, const PiecesBlenderParams params);

	inline float edgeCost(uint pixelAddrP, uint nodeAddrP, 
						  uint pixelAddrQ, uint nodeAddrQ, 
		 				  short label1, short label2,
						  int neighbourType,
						  const PiecesBlenderParams params);
	
	double computeEnergy(const PiecesBlenderParams params);

	void expand(short label, const PiecesBlenderParams params);

	void initGradData(PiecesBlenderParams params);
};

#endif

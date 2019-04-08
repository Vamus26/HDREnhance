#pragma once
#include "FloatImagePyramid.h"


#define DEFAULT_NUMLEVELS 4
#define DEFAULT_LASTLEVEL 0 
#define DEFAULT_MAXITERSPERLEVEL 3
#define DEFAULT_NORMXCORRELCOEFTHRESHOLD 0.95f
#define DEFAULT_EIGVALTHRESHOLD 1.0f
#define DEFAULT_CONDNUMTHRESHOLD 100.0f

class COpticFlow
{
public:
	COpticFlow(void);
	~COpticFlow(void);

	void SetInputImages( const CRGBAImg &in_imgA, const CRGBAImg &in_imgB );
	void SetParams( int in_iNumLevels, int in_iLastLevel = DEFAULT_LASTLEVEL, 
		int in_iMaxItersPerLevel = DEFAULT_MAXITERSPERLEVEL, 
		float in_fNormXCorrelCoefThreshold = DEFAULT_NORMXCORRELCOEFTHRESHOLD, 
		float in_fEigValThreshold = DEFAULT_EIGVALTHRESHOLD, 
		float in_fCondNumThreshold = DEFAULT_CONDNUMTHRESHOLD ) ;

	void ComputeFlow() ;
	void GetAbsoluteFlow( CFloatImg & out_AbsFlowX, CFloatImg & out_AbsFlowY, 
		bool in_bSmoothFlow = true ) ;
	void GetFlow( CFloatImg & out_FlowX, CFloatImg & out_FlowY, bool in_bSmoothFlow = true ) ;
	void SetFlow( CFloatImg & FlowX, CFloatImg & FlowY );
	void InterpolateImage( CRGBAImg & out_imgInterp, float in_fT, bool bForceEqualMix=false ) ;

	void ConstructAllPyramids() ;
	void Warp( CFloatImg &in_ImgOrig, CFloatImg &out_ImgWarped, float in_fFlowFactor, 
		CFloatImg &CoordX, CFloatImg &CoordY, CFloatImg &in_FlowX, CFloatImg &in_FlowY ) ;
	void Warp( CRGBAImg &in_ImgOrig, CRGBAImg &out_ImgWarped, float in_fFlowFactor, 
		CFloatImg &CoordX, CFloatImg &CoordY, CFloatImg &in_FlowX, CFloatImg &in_FlowY ) ;
	void ComputeDerivatives( CFloatImg &in_pA, CFloatImg &in_pB, 
		CFloatImg &out_pDerX, CFloatImg &out_pDerY, CFloatImg &out_pDerT );
	void ComputeIncFlow( CFloatImg &in_pDerX, CFloatImg &in_pDerY, 
		CFloatImg &in_pDerT, CFloatImg &out_pFlowX, CFloatImg &out_pFlowY );
	void ComputeFlowAtPixel( float a11, float a22, float a12, float b1, float b2, 
		float &u, float &v ) ;
	void SmoothFlow( CFloatImg &out_pFlowX, CFloatImg &out_pFlowY ) ;
	void BlendFloatImages( CFloatImg &in_pImgA, CFloatImg &in_pImgB, CFloatImg &out_pDest, 
		float in_fBlendFactor ) ;

	CRGBAImg m_imgA ;
	CRGBAImg m_imgB ;

	int m_iNumLevels ;
	int m_iLastLevel ;
	int m_iMaxItersPerLevel ;
	float m_fNormXCorrelCoefThreshold ;
	float m_fEigValThreshold ;
	float m_fCondNumThreshold ;

	CFloatImagePyramid m_pyrImgA ;
	CFloatImagePyramid m_pyrImgB ;
	CFloatImagePyramid m_pyrWarpedA ;
	CFloatImagePyramid m_pyrWarpedB ;
	CFloatImagePyramid m_pyrDerX ;
	CFloatImagePyramid m_pyrDerY ;
	CFloatImagePyramid m_pyrDerT ;
	CFloatImagePyramid m_pyrFlowX ;
	CFloatImagePyramid m_pyrFlowY ;
	CFloatImagePyramid m_pyrCoordX ;
	CFloatImagePyramid m_pyrCoordY ;
};

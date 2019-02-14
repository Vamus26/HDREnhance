#include "OpticFlow.h"

typedef CVec2f FlowPix;

COpticFlow::COpticFlow(void)
{
	m_iNumLevels = DEFAULT_NUMLEVELS ;
	m_iLastLevel = DEFAULT_LASTLEVEL ;
	m_iMaxItersPerLevel = DEFAULT_MAXITERSPERLEVEL ;
	m_fNormXCorrelCoefThreshold = DEFAULT_NORMXCORRELCOEFTHRESHOLD ;
	m_fEigValThreshold = DEFAULT_EIGVALTHRESHOLD ;
	m_fCondNumThreshold = DEFAULT_CONDNUMTHRESHOLD ;
}

COpticFlow::~COpticFlow(void)
{
}

void COpticFlow::SetInputImages( const CRGBAImg &in_imgA, const CRGBAImg &in_imgB )
{
	m_imgA = in_imgA;
	m_imgB = in_imgB;
}

void COpticFlow::SetParams( int in_iNumLevels, int in_iLastLevel, 
						    int in_iMaxItersPerLevel, float in_fNormXCorrelCoefThreshold,
							float in_fEigThreshold, float in_fCondNumThreshold )
{
	m_iNumLevels = in_iNumLevels ;
	m_iLastLevel = in_iLastLevel ;
	m_iMaxItersPerLevel = in_iMaxItersPerLevel ;
	m_fNormXCorrelCoefThreshold = in_fNormXCorrelCoefThreshold ;
	m_fEigValThreshold = in_fEigThreshold ;
	m_fCondNumThreshold = in_fCondNumThreshold ;
}

void COpticFlow::ConstructAllPyramids()
{
	int iFullResWidth  = m_imgA.Width() ;
	int iFullResHeight = m_imgA.Height() ;

	m_pyrImgA.SetBase( m_imgA ) ;
	m_pyrImgA.SetNumLevels( m_iNumLevels ) ;
	m_pyrImgA.Construct() ;
	m_pyrImgB.SetBase( m_imgB ) ;
	m_pyrImgB.SetNumLevels( m_iNumLevels ) ;
	m_pyrImgB.Construct() ;

	m_pyrWarpedA.SetEmptyBase( iFullResWidth, iFullResHeight ) ;
	m_pyrWarpedA.SetNumLevels( m_iNumLevels ) ;
	m_pyrWarpedA.Allocate() ;
	m_pyrWarpedB.SetEmptyBase( iFullResWidth, iFullResHeight ) ;
	m_pyrWarpedB.SetNumLevels( m_iNumLevels ) ;
	m_pyrWarpedB.Allocate() ;

	m_pyrDerX.SetEmptyBase( iFullResWidth, iFullResHeight ) ;
	m_pyrDerX.SetNumLevels( m_iNumLevels ) ;
	m_pyrDerX.Allocate() ;
	m_pyrDerY.SetEmptyBase( iFullResWidth, iFullResHeight ) ;
	m_pyrDerY.SetNumLevels( m_iNumLevels ) ;
	m_pyrDerY.Allocate() ;
	m_pyrDerT.SetEmptyBase( iFullResWidth, iFullResHeight ) ;
	m_pyrDerT.SetNumLevels( m_iNumLevels ) ;
	m_pyrDerT.Allocate() ;

	m_pyrFlowX.SetEmptyBase( iFullResWidth, iFullResHeight ) ;
	m_pyrFlowX.SetNumLevels( m_iNumLevels ) ;
	m_pyrFlowX.Allocate() ;
	m_pyrFlowY.SetEmptyBase( iFullResWidth, iFullResHeight ) ;
	m_pyrFlowY.SetNumLevels( m_iNumLevels ) ;
	m_pyrFlowY.Allocate() ;

	m_pyrCoordX.SetEmptyBase( iFullResWidth, iFullResHeight ) ;
	m_pyrCoordX.SetNumLevels( m_iNumLevels ) ;
	m_pyrCoordX.Allocate() ;
	m_pyrCoordY.SetEmptyBase( iFullResWidth, iFullResHeight ) ;
	m_pyrCoordY.SetNumLevels( m_iNumLevels ) ;
	m_pyrCoordY.Allocate() ;

	for ( int iCurLevel = m_iNumLevels - 1; iCurLevel >= m_iLastLevel; iCurLevel-- )
	{
		int iWidth  = m_pyrCoordX[iCurLevel]->Width();
		int iHeight = m_pyrCoordX[iCurLevel]->Height();

		CFloatImg *pfCoordX = m_pyrCoordX[iCurLevel];
		CFloatImg *pfCoordY = m_pyrCoordY[iCurLevel];

		for ( int y = 0; y < iHeight; y++ )
		{
			float *pX = pfCoordX->Ptr(y);
			float *pY = pfCoordY->Ptr(y);

			for ( int x = 0; x < iWidth; x++ )
			{
				pX[x] = (float)x;
				pY[x] = (float)y;
			}
		}
	}
}

inline void COpticFlow::ComputeFlowAtPixel( float a11, float a22, float a12, 
												 float b1, float b2, 
												 float & out_dFlowX, float & out_dFlowY )
{    
	//------------------------------------------------------------------------------------------------------
	// compute both eigenvalues of left-hand side normal equation matrix
	//------------------------------------------------------------------------------------------------------
	float a11plusa22 = a11 + a22 ;
	float sqrtterm = ::sqrt( a11*a11 - 2*a11*a22 + a22*a22 + 4*a12*a12 ) ;
	float eig1 = 0.5f * ( a11plusa22 + sqrtterm ) ; // the larger eigenvalue
	float eig2 = 0.5f * ( a11plusa22 - sqrtterm ) ; // the smaller eigenvalue

	if ( eig1 < m_fEigValThreshold )
	{
		//------------------------------------------------------------------------------------------------------
		// No structure in the patch: zero partials = zero flow
		//------------------------------------------------------------------------------------------------------
 		return ;
	}
	else if ( eig1 > m_fCondNumThreshold * eig2 )
	{
		//------------------------------------------------------------------------------------------------------
		// APERTURE AREA: Set to a vector in the direction of the large eigenvector: compute normal flow.
		//------------------------------------------------------------------------------------------------------

		// Flow is given by alpha*Phi where Phi is a normalized eigenvector corresponding to the larger eigenvalue
		// and alpha is given by transpose(Phi)*B/lambda where lambda is the larger eigenvector and B is (b1,b2)

		if ( fabs(a12) > 1.0e-3 )
		{
			float eigV1 = ( eig1 - a22 ) / a12 ; // The first component of an eigenvector corresponding to the largest eigenvalue
			float normSq = 1 + eigV1 * eigV1 ;  // The square of the norm of the eigenvector
			
			float out2 = ( eigV1 * b1 + b2 ) / ( eig1 * normSq ) ;//the y component of the flow
			float out1 = out2 * eigV1 ;//the x component of the flow
			
			out_dFlowX += out1 ;
			out_dFlowY += out2 ;
		}
		//These result from simplifications of the alpha*Phi that result when a12=0. Note that this is required because otherwise we get div by zero above
		//Note also that if a11=a12 we never enter the APERTURE AREA code.
		else if ( a11 > a22 )
		{
			//if a11=0 we enter eig1<m_dEigTao
			out_dFlowX += b1/a11;
		}
		else 
		{	
			//if a22=0 we enter eig1<m_dEigTao
			out_dFlowY += b2/a22;
		}
	}
	else
	{
		//------------------------------------------------------------------------------------------------------
		// Non-aperture area: We can successfully compute the flow via matrix inversion.
		//------------------------------------------------------------------------------------------------------

		// No need to see if determinant is small: recall: det(A) = product of eigenvalues
		float oneoverdet = 1.0f / ( a11*a22 - a12*a12 ) ;
		out_dFlowX += ( a22*b1 - a12*b2 ) * oneoverdet ;
		out_dFlowY += ( a11*b2 - a12*b1 ) * oneoverdet ;
	}
}

void COpticFlow::ComputeFlow()
{
	int iCurLevel, iCurIter;

	ConstructAllPyramids() ;

	for ( iCurLevel = m_iNumLevels - 1 ; iCurLevel >= m_iLastLevel ; iCurLevel-- )
	{
		for ( iCurIter = 0 ; iCurIter < m_iMaxItersPerLevel ; iCurIter++ )
		{
			if ( iCurLevel == (m_iNumLevels - 1) && iCurIter == 0 )
			{
				// We are in the VERY first iteration in the whole loop
				ComputeDerivatives( *m_pyrImgA[iCurLevel], *m_pyrImgB[iCurLevel], 
					*m_pyrDerX[iCurLevel], *m_pyrDerY[iCurLevel], *m_pyrDerT[iCurLevel] ) ;
			}
			else
			{
				Warp( *m_pyrImgA[iCurLevel], *m_pyrWarpedA[iCurLevel],  0.5f, 
					*m_pyrCoordX[iCurLevel], *m_pyrCoordY[iCurLevel], 
					*m_pyrFlowX[iCurLevel], *m_pyrFlowY[iCurLevel] ) ;
				Warp( *m_pyrImgB[iCurLevel], *m_pyrWarpedB[iCurLevel], -0.5f, 
					*m_pyrCoordX[iCurLevel], *m_pyrCoordY[iCurLevel], 
					*m_pyrFlowX[iCurLevel], *m_pyrFlowY[iCurLevel] ) ;
				ComputeDerivatives( *m_pyrWarpedA[iCurLevel], *m_pyrWarpedB[iCurLevel], 
					*m_pyrDerX[iCurLevel], *m_pyrDerY[iCurLevel], *m_pyrDerT[iCurLevel] ) ;
			}

			ComputeIncFlow( *m_pyrDerX[iCurLevel], *m_pyrDerY[iCurLevel], 
				*m_pyrDerT[iCurLevel], *m_pyrFlowX[iCurLevel], *m_pyrFlowY[iCurLevel] ) ;

			SmoothFlow( *m_pyrFlowX[iCurLevel], *m_pyrFlowY[iCurLevel] ) ;
		}

		if ( iCurLevel != m_iLastLevel )
		{
			int w = m_pyrFlowX[iCurLevel-1]->Width(), h = m_pyrFlowX[iCurLevel-1]->Height();
			CFloatImg temp(w,h);

			// upsample flow
			m_pyrFlowX.Expand( iCurLevel ) ;
			m_pyrFlowY.Expand( iCurLevel ) ;
			//iplMultiplySFP( m_pyrFlowX[iCurLevel-1], m_pyrFlowX[iCurLevel-1], 2.0f ) ;
			m_pyrFlowX[iCurLevel-1]->CopyTo( temp );
			VtScaleImage( temp, *m_pyrFlowX[iCurLevel-1], 2.0f);
			//iplMultiplySFP( m_pyrFlowY[iCurLevel-1], m_pyrFlowY[iCurLevel-1], 2.0f ) ;
			m_pyrFlowY[iCurLevel-1]->CopyTo( temp );
			VtScaleImage( temp, *m_pyrFlowY[iCurLevel-1], 2.0f);
		}
	}

	// upsample flow to all remaining uncalculated levels
	for ( iCurLevel = m_iLastLevel ; iCurLevel >= 1 ; iCurLevel-- )
	{
		int w = m_pyrFlowX[iCurLevel-1]->Width(), h = m_pyrFlowX[iCurLevel-1]->Height();
		CFloatImg temp(w,h);

		// upsample flow
		m_pyrFlowX.Expand( iCurLevel ) ;
		m_pyrFlowY.Expand( iCurLevel ) ;
		//iplMultiplySFP( m_pyrFlowX[iCurLevel-1], m_pyrFlowX[iCurLevel-1], 2.0f ) ;
		m_pyrFlowX[iCurLevel-1]->CopyTo( temp );
		VtScaleImage( temp, *m_pyrFlowX[iCurLevel-1], 2.0f);
		//iplMultiplySFP( m_pyrFlowY[iCurLevel-1], m_pyrFlowY[iCurLevel-1], 2.0f ) ;
		m_pyrFlowY[iCurLevel-1]->CopyTo( temp );
		VtScaleImage( temp, *m_pyrFlowY[iCurLevel-1], 2.0f);
	}
}

void COpticFlow::GetFlow( CFloatImg &out_FlowX, CFloatImg &out_FlowY, bool in_bSmoothFlow )
{
	//out_FlowX.Alias( m_pyrFlowX[0] ) ;
	//out_FlowY.Alias( m_pyrFlowY[0] ) ;

	//if ( in_bSmoothFlow )
	//{
	//	Vis3x3MedianImpl( out_FlowX, out_FlowX ) ;
	//	Vis3x3MedianImpl( out_FlowY, out_FlowY ) ;
	//}

	int w = m_pyrFlowX[0]->Width(), h = m_pyrFlowX[0]->Height();
	if (out_FlowX.Width()!=w || out_FlowX.Height()!=h)
		out_FlowX.Create( w, h );
	if (out_FlowY.Width()!=w || out_FlowY.Height()!=h)
		out_FlowY.Create( w, h );
	m_pyrFlowX[0]->CopyTo( out_FlowX );
	m_pyrFlowY[0]->CopyTo( out_FlowY );
	if (in_bSmoothFlow)
		SmoothFlow( out_FlowX, out_FlowY );
}
 
void COpticFlow::SetFlow( CFloatImg & FlowX, CFloatImg & FlowY )
{
	//IplImage *ipl_flowx = VisIplCreateImage( FlowX, true );
	//IplImage *ipl_flowy = VisIplCreateImage( FlowY, true );

	//VisIplDeleteImage( m_pyrFlowX[0] );
	//VisIplDeleteImage( m_pyrFlowY[0] );

	//*m_pyrFlowX[0] = *ipl_flowx;
	//*m_pyrFlowY[0] = *ipl_flowy;	

	// assume resolution is the same
	FlowX.CopyTo( *m_pyrFlowX[0] );
	FlowY.CopyTo( *m_pyrFlowY[0] );
}

void COpticFlow::GetAbsoluteFlow( CFloatImg & out_AbsFlowX, CFloatImg & out_AbsFlowY, 
								  bool in_bSmoothFlow  )
{
	//IplImage * pIPLAbsoluteFlowX = CreateFloatImage( m_imgA.Width(), m_imgA.Height() ) ;
	//IplImage * pIPLAbsoluteFlowY = CreateFloatImage( m_imgA.Width(), m_imgA.Height() ) ;
	int w = m_imgA.Width(), h = m_imgA.Height();
	if (out_AbsFlowX.Width()!=w || out_AbsFlowX.Height()!=h)
		out_AbsFlowX.Create( w, h );
	if (out_AbsFlowY.Width()!=w || out_AbsFlowY.Height()!=h)
		out_AbsFlowY.Create( w, h );

	//iplAdd( m_pyrCoordX[0], m_pyrFlowX[0], pIPLAbsoluteFlowX ) ;
	//iplAdd( m_pyrCoordY[0], m_pyrFlowY[0], pIPLAbsoluteFlowY ) ;
	VtAddImages( *m_pyrCoordX[0], *m_pyrFlowX[0], out_AbsFlowX );
	VtAddImages( *m_pyrCoordY[0], *m_pyrFlowY[0], out_AbsFlowY );

	//out_AbsFlowX.Alias( pIPLAbsoluteFlowX ) ;
	//out_AbsFlowY.Alias( pIPLAbsoluteFlowY ) ;

	//if ( in_bSmoothFlow )
	//{
	//	Vis3x3MedianImpl( out_AbsFlowX, out_AbsFlowX ) ;
	//	Vis3x3MedianImpl( out_AbsFlowY, out_AbsFlowY ) ;
	//}
	if (in_bSmoothFlow)
		SmoothFlow( out_AbsFlowX, out_AbsFlowY );

	//iplDeallocateImage( pIPLAbsoluteFlowX ) ;
	//iplDeallocateImage( pIPLAbsoluteFlowY ) ;
}

void COpticFlow::SmoothFlow( CFloatImg &out_pFlowX, CFloatImg &out_pFlowY )
{
	// IPL doc says iplFixedFilter supports in-place proc, but it doesn't seem to! So we have to use a temp image.... 
	//IplImage * pTempImage = iplCloneImage( out_pFlowX ) ;
	//iplFixedFilter( pTempImage, out_pFlowX, IPL_GAUSSIAN_5x5 ) ;

	//iplCopy( out_pFlowY, pTempImage ) ;
	//iplFixedFilter( pTempImage, out_pFlowY, IPL_GAUSSIAN_5x5 ) ;

	//iplDeallocateImage( pTempImage ) ;

	CFloatImg temp(out_pFlowX.Width(),out_pFlowX.Height());
	out_pFlowX.CopyTo( temp );
	VtGaussianSmooth( temp, out_pFlowX, 2.0f, Extend );
	out_pFlowY.CopyTo( temp );
	VtGaussianSmooth( temp, out_pFlowY, 2.0f, Extend );
}

void COpticFlow::InterpolateImage( CRGBAImg & out_imgInterp, float in_fT,
								   bool bForceEqualMix )
{
	if (in_fT<0.0f || in_fT>1.0f) return;
	//assert( 0.0 <= in_fT && in_fT <= 1.0 ) ;

	//IplImage * pIPLRGBAImageA  = VisIplCreateImage( m_imgA, true ) ;
	//IplImage * pIPLRGBAImageB  = VisIplCreateImage( m_imgB, true ) ;
	//IplImage * pIPLInterpImage = VisIplCreateImage( out_imgInterp, true ) ;

	float fForwardFlowPercentage = in_fT ;
	float fBackwardFlowPercentage = -( 1.0f - in_fT ) ;

	//IplImage * iplTempImageA = VisIplCreateImage( m_imgA, true ) ; // can't use clone?
	//IplImage * iplTempImageB = VisIplCreateImage( m_imgB, true) ; // can't use clone?
	int w = m_imgA.Width(), h = m_imgA.Height();
	CRGBAImg WarpedImageA(w,h), WarpedImageB(w,h);

	//Warp( pIPLRGBAImageA, iplTempImageA, fForwardFlowPercentage,  m_pyrCoordX[0], m_pyrCoordY[0], m_pyrFlowX[0], m_pyrFlowY[0] ) ;
	//Warp( pIPLRGBAImageB, iplTempImageB, fBackwardFlowPercentage, m_pyrCoordX[0], m_pyrCoordY[0], m_pyrFlowX[0], m_pyrFlowY[0] ) ;
	Warp( m_imgA, WarpedImageA, fForwardFlowPercentage, 
		  *m_pyrCoordX[0], *m_pyrCoordY[0], *m_pyrFlowX[0], *m_pyrFlowY[0] ) ;
	Warp( m_imgB, WarpedImageB, fBackwardFlowPercentage, 
		  *m_pyrCoordX[0], *m_pyrCoordY[0], *m_pyrFlowX[0], *m_pyrFlowY[0] ) ;

	//int iBlendA = ::floor( 255.0f * ( 1.0f - in_fT ) + 0.5f ) ;
	//int iBlendB = 255 - iBlendA ;
	//iplAlphaCompositeC( iplTempImageA, iplTempImageB, pIPLInterpImage, IPL_COMPOSITE_PLUS, iBlendA, iBlendB, FALSE, TRUE ) ;
	if (out_imgInterp.Width()!=w || out_imgInterp.Height()!=h)
		out_imgInterp.Create(w,h);
	VtBlendImages( WarpedImageA, WarpedImageB, out_imgInterp, 
		bForceEqualMix ? 0.5f : (1.0f - in_fT) );

	//out_imgInterp.Alias( pIPLInterpImage ) ;

	//VisIplDeleteImage( pIPLRGBAImageA ) ;
	//VisIplDeleteImage( pIPLRGBAImageB ) ;
	//VisIplDeleteImage( pIPLInterpImage ) ;

	//VisIplDeleteImage( iplTempImageA ) ;
	//VisIplDeleteImage( iplTempImageB ) ;
}

void COpticFlow::Warp( CFloatImg &in_ImgOrig, CFloatImg &out_ImgWarped, float in_fFlowFactor, 
					   CFloatImg &CoordX, CFloatImg &CoordY, 
					   CFloatImg &in_FlowX, CFloatImg &in_FlowY )
{
	int r, c, w = in_FlowX.Width(), h = in_FlowX.Height();
	//CFloatImg IPLTotalFlowX( w, h ) ;
	//CFloatImg IPLTotalFlowY( w, h ) ;

	//iplMultiplySFP( in_pFlowX, pIPLTotalFlowX, in_fFlowFactor ) ;
	//iplMultiplySFP( in_pFlowY, pIPLTotalFlowY, in_fFlowFactor ) ;

	//iplAdd( pIPLTotalFlowX, pCoordX, pIPLTotalFlowX ) ;
	//iplAdd( pIPLTotalFlowY, pCoordY, pIPLTotalFlowY ) ;

	//iplRemap( in_pImgOrig, pIPLTotalFlowX, pIPLTotalFlowY, out_pImgWarped, IPL_INTER_LINEAR ) ;
	float background = 0.0f;
	CFlowImg imgFlow(w,h);
	int c2;
	float *xy = new float[2*w], *dxdy = new float[2*w], *out = new float[2*w];
	for (r=0; r<h; r++)
	{
		FlowPix *f = imgFlow.Ptr(r);
		float *x = CoordX.Ptr(r), *y = CoordY.Ptr(r);
		float *dx = in_FlowX.Ptr(r), *dy = in_FlowY.Ptr(r);

#if ORIG_CODE
		for (c=0; c<w; c++)
		{
			f[c][0] = x[c] + in_fFlowFactor*dx[c];
			f[c][1] = y[c] + in_fFlowFactor*dy[c];
		}
#else // ORIG_CODE
		for (c2=c=0; c2<w; c2++,c+=2)
		{
			xy[c] = x[c2]; xy[c+1] = y[c2];
			dxdy[c] = dx[c2]; dxdy[c+1] = dy[c2];
		}
		VtScaleAccSpan( xy, dxdy, out, in_fFlowFactor, 2*w );
		for (c2=c=0; c2<w; c2++,c+=2)
		{
			f[c2][0] = out[c]; f[c2][1] = out[c+1];
		}		
#endif // ORIG_CODE
	}
	delete []xy; delete []dxdy;
	vt::Warp( in_ImgOrig, imgFlow, &background, out_ImgWarped );
	//iplDeallocateImage( pIPLTotalFlowX ) ;
	//iplDeallocateImage( pIPLTotalFlowY ) ;
}

void COpticFlow::Warp( CRGBAImg &in_ImgOrig, CRGBAImg &out_ImgWarped, float in_fFlowFactor, 
					   CFloatImg &CoordX, CFloatImg &CoordY, 
					   CFloatImg &in_FlowX, CFloatImg &in_FlowY )
{
	int i, r, c, w = in_FlowX.Width(), h = in_FlowX.Height();
	//CFloatImg IPLTotalFlowX( w, h ) ;
	//CFloatImg IPLTotalFlowY( w, h ) ;

	//iplMultiplySFP( in_pFlowX, pIPLTotalFlowX, in_fFlowFactor ) ;
	//VtScaleImage( in_FlowX, IPLTotalFlowX, in_fFlowFactor );
	//iplMultiplySFP( in_pFlowY, pIPLTotalFlowY, in_fFlowFactor ) ;
	//VtScaleImage( in_FlowY, IPLTotalFlowY, in_fFlowFactor );

	//iplAdd( pIPLTotalFlowX, pCoordX, pIPLTotalFlowX ) ;
	//VtAddImages( IPLTotalFlowX, CoordX, IPLTotalFlowX );
	//iplAdd( pIPLTotalFlowY, pCoordY, pIPLTotalFlowY ) ;
	//VtAddImages( IPLTotalFlowY, CoordY, IPLTotalFlowY );

	//iplRemap( in_pImgOrig, pIPLTotalFlowX, pIPLTotalFlowY, out_pImgWarped, IPL_INTER_LINEAR ) ;
	CByteImg in[3], out[3];
	for (i=0; i<3; i++)
	{
		in[i].Create(w,h);
		out[i].Create(w,h);
	}
	BYTE background = 0;
	CFlowImg imgFlow(w,h);
	for (r=0; r<h; r++)
	{
		FlowPix *f = imgFlow.Ptr(r);
		float *x = CoordX.Ptr(r), *y = CoordY.Ptr(r);
		float *dx = in_FlowX.Ptr(r), *dy = in_FlowY.Ptr(r);
		RGBAPix *p = in_ImgOrig.Ptr(r);
		BYTE *q0 = in[0].Ptr(r), *q1 = in[1].Ptr(r), *q2 = in[2].Ptr(r);
		for (c=0; c<w; c++)
		{
			f[c][0] = x[c] + in_fFlowFactor*dx[c];
			f[c][1] = y[c] + in_fFlowFactor*dy[c];
			q0[c] = p[c].r; q1[c] = p[c].g; q2[c] = p[c].b;
		}
	}
	for (i=0; i<3; i++)
		vt::Warp( in[i], imgFlow, &background, out[i] );
	for (r=0; r<h; r++)
	{
		RGBAPix *q = out_ImgWarped.Ptr(r);
		BYTE *p0 = out[0].Ptr(r), *p1 = out[1].Ptr(r), *p2 = out[2].Ptr(r);
		for (c=0; c<w; c++)
		{
			q[c].r = p0[c]; q[c].g = p1[c]; q[c].b = p2[c]; 
			q[c].a = (p0[c]==background && p1[c]==background && p2[c]==background)
				? 0 : 255;
		}
	}
	//iplDeallocateImage( pIPLTotalFlowX ) ;
	//iplDeallocateImage( pIPLTotalFlowY ) ;
}

void COpticFlow::ComputeDerivatives( CFloatImg &in_pA, CFloatImg &in_pB, 
									 CFloatImg &out_pDerX, CFloatImg &out_pDerY, 
									 CFloatImg &out_pDerT )
{
	// Simoncelli 3-tap filters.. see paper

	float afB[3] = { 0.2333755f, 0.5552490f, 0.2333755f } ;
	//IplConvKernelFP * pBX = iplCreateConvKernelFP( 3, 1, 1, 0, afB ) ;
	//IplConvKernelFP * pBY = iplCreateConvKernelFP( 1, 3, 0, 1, afB ) ;
	C1dKernel B; B.Create(3,1,afB);

	float afD[3] = { -0.453014f, 0.0f, 0.453014f } ;
	//IplConvKernelFP * pDX = iplCreateConvKernelFP( 3, 1, 1, 0, afD ) ;
	//IplConvKernelFP * pDY = iplCreateConvKernelFP( 1, 3, 0, 1, afD ) ;
	C1dKernel D; D.Create(3,1,afD);

	int iWidth  = in_pA.Width();
	int iHeight = in_pB.Height();

	CFloatImg DerXA( iWidth, iHeight ) ;
	CFloatImg DerXB( iWidth, iHeight ) ;
	CFloatImg DerYA( iWidth, iHeight ) ;
	CFloatImg DerYB( iWidth, iHeight ) ;
	CFloatImg BlurA( iWidth, iHeight ) ;
	CFloatImg BlurB( iWidth, iHeight ) ;

	//iplConvolveSep2DFP( in_pA, pDerXA, pDX, pBY ) ; 
	VtSeparableFilter( in_pA, DerXA, D, B, Extend );
	//iplConvolveSep2DFP( in_pB, pDerXB, pDX, pBY ) ; 
	VtSeparableFilter( in_pB, DerXB, D, B, Extend );
	//iplConvolveSep2DFP( in_pA, pDerYA, pBX, pDY ) ; 
	VtSeparableFilter( in_pA, DerYA, B, D, Extend );
	//iplConvolveSep2DFP( in_pB, pDerYB, pBX, pDY ) ;
	VtSeparableFilter( in_pB, DerYB, B, D, Extend );
	//iplConvolveSep2DFP( in_pA, pBlurA, pBX, pBY ) ; 
	VtSeparableFilter( in_pA, BlurA, B, B, Extend );
	//iplConvolveSep2DFP( in_pB, pBlurB, pBX, pBY ) ;
	VtSeparableFilter( in_pB, BlurB, B, B, Extend );

	BlendFloatImages( DerXA, DerXB, out_pDerX, 0.5f ) ;
	BlendFloatImages( DerYA, DerYB, out_pDerY, 0.5f ) ;
	//iplSubtract( pBlurB, pBlurA, out_pDerT ) ;
	VtSubImages( BlurB, BlurA, out_pDerT );
}

void COpticFlow::BlendFloatImages( CFloatImg &in_pImgA, CFloatImg &in_pImgB, 
								   CFloatImg &out_pDest, float in_fBlendFactor )
{
	//iplSubtract( in_pImgB, in_pImgA, out_pDest ) ;
	//iplMultiplySFP( out_pDest, out_pDest, (1.0f - in_fBlendFactor) ) ;
	//iplAdd( in_pImgA, out_pDest, out_pDest ) ;
	int r, c, w = in_pImgA.Width(), h = in_pImgA.Height();
	float a1 = in_fBlendFactor, a2 = 1.0f - a1;

	for (r=0; r<h; r++)
	{
		float *q = out_pDest.Ptr(r);
		float *pA = in_pImgA.Ptr(r);
		float *pB = in_pImgB.Ptr(r);

		for (c=0; c<w; c++)
		{
			q[c] = a1*pA[c] + a2*pB[c];
		}
	}
}

void COpticFlow::ComputeIncFlow( CFloatImg &in_pDerX, CFloatImg &in_pDerY, 
								 CFloatImg &in_pDerT, CFloatImg &out_pFlowX, 
								 CFloatImg &out_pFlowY )
{
	int iWidth  = in_pDerX.Width();
	int iHeight = in_pDerX.Height();

	CFloatImg DerX2( iWidth, iHeight ) ;
	CFloatImg DerY2( iWidth, iHeight ) ;
	CFloatImg DerXDerY( iWidth, iHeight ) ;
	CFloatImg DerXDerT( iWidth, iHeight ) ;
	CFloatImg DerYDerT( iWidth, iHeight ) ;

	//iplSquare( in_pDerX, pDerX2 ) ;
	VtMulImages( in_pDerX, in_pDerX, DerX2 );
	//iplSquare( in_pDerY, pDerY2 ) ;
	VtMulImages( in_pDerY, in_pDerY, DerY2 );
	//iplMultiply( in_pDerX, in_pDerY, pDerXDerY ) ;
	VtMulImages( in_pDerX, in_pDerY, DerXDerY );
	//iplMultiply( in_pDerX, in_pDerT, pDerXDerT ) ;
	VtMulImages( in_pDerX, in_pDerT, DerXDerT );
	//iplMultiply( in_pDerY, in_pDerT, pDerYDerT ) ;
	VtMulImages( in_pDerY, in_pDerT, DerYDerT );

	CFloatImg SumDerX2( iWidth, iHeight ) ;
	CFloatImg SumDerY2( iWidth, iHeight ) ;
	CFloatImg SumDerXDerY( iWidth, iHeight ) ;
	CFloatImg SumDerXDerT( iWidth, iHeight ) ;
	CFloatImg SumDerYDerT( iWidth, iHeight ) ;

	//iplFixedFilter( pDerX2, pSumDerX2, IPL_GAUSSIAN_5x5 ) ;
	VtGaussianSmooth( DerX2, SumDerX2, 2.0f, Extend );
	//iplFixedFilter( pDerY2, pSumDerY2, IPL_GAUSSIAN_5x5 ) ;
	VtGaussianSmooth( DerY2, SumDerY2, 2.0f, Extend );
	//iplFixedFilter( pDerXDerY, pSumDerXDerY, IPL_GAUSSIAN_5x5 ) ;
	VtGaussianSmooth( DerXDerY, SumDerXDerY, 2.0f, Extend );
	//iplFixedFilter( pDerXDerT, pSumDerXDerT, IPL_GAUSSIAN_5x5 ) ;
	VtGaussianSmooth( DerXDerT, SumDerXDerT, 2.0f, Extend );
	//iplFixedFilter( pDerYDerT, pSumDerYDerT, IPL_GAUSSIAN_5x5 ) ;
	VtGaussianSmooth( DerYDerT, SumDerYDerT, 2.0f, Extend );

	float *pfSumDerX2    = SumDerX2.Ptr(0);
	float *pfSumDerY2    = SumDerY2.Ptr(0);
	float *pfSumDerXDerY = SumDerXDerY.Ptr(0);
	float *pfSumDerXDerT = SumDerXDerT.Ptr(0);
	float *pfSumDerYDerT = SumDerYDerT.Ptr(0);
	float *pfFlowX       = out_pFlowX.Ptr(0);
	float *pfFlowY       = out_pFlowY.Ptr(0);
	
	for ( int i = 0; i < iWidth * iHeight; i++ )
	{
		ComputeFlowAtPixel( pfSumDerX2[i], pfSumDerY2[i], pfSumDerXDerY[i], 
			-pfSumDerXDerT[i], -pfSumDerYDerT[i], pfFlowX[i], pfFlowY[i] ) ; 
	}
}

#if 0
void CVisOpticalFlow::WarpImageUsingTopLevelFlow( CVisRGBAByteImage & out_imgWarped )
{
	IplImage * pIPLInputImage  = VisIplCreateImage( m_imgA, true ) ;
	IplImage * pIPLWarpedImage = VisIplCreateImage( m_imgB, true ) ;

	Warp( pIPLInputImage, pIPLWarpedImage, 1.0, m_pyrCoordX[0], m_pyrCoordY[0], m_pyrFlowX[0], m_pyrFlowY[0] ) ;

	out_imgWarped.Alias( pIPLWarpedImage ) ;

	VisIplDeleteImage( pIPLInputImage ) ;
	VisIplDeleteImage( pIPLWarpedImage ) ;
}

void CVisOpticalFlow::ComputeFlowOnEntireSequence( CVisRGBAByteSequence & in_seq, CVisSequence<CVisGrayFloatPixel> & out_FlowX, CVisSequence<CVisGrayFloatPixel> & out_FlowY, bool in_bReturnAbsoluteFlow )
{
	in_seq.SetMRULength( 2 ) ;
	out_FlowX.SetMRULength( 1 ) ;
	out_FlowY.SetMRULength( 1 ) ;
	out_FlowX.Resize( in_seq.Length() - 1 ) ; 
	out_FlowY.Resize( in_seq.Length() - 1 ) ;
	out_FlowX.SetLengthMax( in_seq.Length() - 1 ) ;
	out_FlowY.SetLengthMax( in_seq.Length() - 1 ) ;

	for ( int i = 0 ; i <= in_seq.Length() - 2 ; i++ )
	{
		SetInputImages( in_seq[i], in_seq[i+1] ) ;
		ComputeFlow() ;
		if ( in_bReturnAbsoluteFlow )
		{
			GetAbsoluteFlow( out_FlowX[i], out_FlowY[i] ) ;
		}
		else
		{
			GetFlow( out_FlowX[i], out_FlowY[i] ) ;
		}
	}
}

void CVisOpticalFlow::WarpNeighborhoodToFrame( CVisRGBAByteSequence & in_seq, CVisRGBAByteSequence & out_seq, int in_iFirstFrame, int in_iAnchor, int in_iSize )
{
	CVisRGBAByteSequence seqTemp ;
	seqTemp.SetLengthMax( in_iSize ) ;
	seqTemp.Resize( in_iSize ) ;
	int iCurFrame = in_iFirstFrame ;
	for ( int i = 0 ; i < in_iSize ; i++ )
	{
		seqTemp[i] = in_seq[iCurFrame++] ;
	}

	int iAnchor = in_iAnchor - in_iFirstFrame ;

	CVisRGBAByteImage imageTemp1 ;
	imageTemp1.Allocate( in_seq[0].Shape() ) ;

	out_seq.SetLengthMax( in_iSize ) ;
	out_seq.Resize( in_iSize ) ;

	for ( i = 0 ; i <= iAnchor - 1 ; i++ )
	{
		for ( int j = i ; j <= iAnchor - 1 ; j++ )
		{
			SetInputImages( j == i ? seqTemp[j] : imageTemp1, seqTemp[j+1] ) ;
			ComputeFlow() ;
			WarpImageUsingTopLevelFlow( imageTemp1 ) ;

		}

		out_seq[i] = imageTemp1 ;
	}

	out_seq[iAnchor] = seqTemp[iAnchor] ;

	for ( i = seqTemp.Length() - 1 ; i >= iAnchor + 1 ; i-- )
	{
		for ( int j = i ; j >= iAnchor + 1 ; j-- )
		{
			SetInputImages( j == i ? seqTemp[j] : imageTemp1, seqTemp[j-1] ) ;
			ComputeFlow() ;
			WarpImageUsingTopLevelFlow( imageTemp1 ) ;
		}

		out_seq[i] = imageTemp1 ;
	}
}
#endif
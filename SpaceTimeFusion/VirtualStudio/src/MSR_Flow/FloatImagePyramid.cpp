#include "FloatImagePyramid.h"

#ifndef BYTE
typedef unsigned char BYTE;
#endif


static inline BYTE ClampFloatToByte( float x, BYTE l, BYTE h )
{
	if ( x > h )
		return h ;
	else if ( x < l )
		return l ;
	else
		return (BYTE) x ;
}

static void CopyPixels_RGBAByte2Float( CFloatImg &dst, const CRGBAImg &src )
{
	int r, c, w = src.Shape().width, h = src.Shape().height;

	CShape dstShape = src.Shape();
	dstShape.nBands = 1;
	dst.ReAllocate(dstShape);

	for (r=0; r<h; r++)
	{
		uchar *p = (uchar *) src.PixelAddress(0, r, 0);
		float *q = (float *) dst.PixelAddress(0, r, 0);

		for (c=0; c<w; c++)
		{
			q[c] = 0.299f * p[3*c + 0] + 0.587f * p[3*c + 1] + 0.114f * p[3*c + 2];
		}
	}
}

static void CopyPixels_Float2RGBAByte( CRGBAImg &dst, const CFloatImg &src )
{
	int r, c, w = src.Shape().width, h = src.Shape().height;

	CShape dstShape = src.Shape();
	dstShape.nBands = 3;
	dst.ReAllocate(dstShape);

	for (r=0; r<h; r++)
	{
		uchar *q = (uchar *) dst.PixelAddress(0, r, 0);
		float *p = (float *) src.PixelAddress(0, r, 0);

		for (c=0; c<w; c++)
		{
			int val = (p[c]<0.0f) ? 0 : ((p[c]>255.0f) ? 255 : (int)(p[c]+0.5));

			q[3*c + 0] = q[3*c + 1] = q[3*c + 2] = (uchar) val;
		}
	}
}

CFloatImagePyramid::CFloatImagePyramid()
{
	m_iNumLevels = 0 ;
	m_bRGBAImgSetAsBase = true;
	m_pFloatImg = NULL;
	m_ppLevels = NULL ;
}

CFloatImagePyramid::~CFloatImagePyramid()
{
	Destroy() ;
	m_iNumLevels = 0 ;
	m_pFloatImg = NULL;
	m_ppLevels = NULL ;
}

void CFloatImagePyramid::Destroy()
{
	if ( m_ppLevels == NULL ) return ;

	for ( int i = 0 ; i < m_iNumLevels ; i++ )
	{
		if ( m_ppLevels[i] != NULL )
		{
			delete [] m_ppLevels[i];
		}
	}

	delete [] m_ppLevels ;
}

void CFloatImagePyramid::SetNumLevels( int in_iNumLevels )
{
	m_iNumLevels = in_iNumLevels ;
}

void CFloatImagePyramid::SetEmptyBase( int iWidth, int iHeight )
{
	m_iEmptyWidth = iWidth ;
	m_iEmptyHeight = iHeight ;
	m_bRGBAImgSetAsBase = false;
}

CFloatImg * CFloatImagePyramid::operator[]( int in_iLevel )
{
	return m_ppLevels[in_iLevel] ;
}

bool CFloatImagePyramid::Allocate()
{
	Destroy() ;

	m_ppLevels = new CFloatImg * [ m_iNumLevels ] ;
	if ( m_ppLevels == NULL ) return false ;

	int iWidth ;
	int iHeight ;
	bool bFailed = false ;

	if ( m_bRGBAImgSetAsBase )
	{
		iWidth  = m_RGBAImg.Shape().width;
		iHeight = m_RGBAImg.Shape().height;
	}
	else
	{
		iWidth = m_iEmptyWidth;
		iHeight = m_iEmptyHeight;
	}
	m_ppLevels[0] = new CFloatImg( iWidth, iHeight, 1 ) ;
	if ( m_ppLevels[0] == NULL ) return false ;

	for ( int i=1; i<m_iNumLevels ; i++ )
	{
		m_ppLevels[i] = new CFloatImg( m_ppLevels[i-1]->Shape().width/2, 
									   m_ppLevels[i-1]->Shape().height/2,
									   1) ;
		if ( m_ppLevels[i] == NULL ) 
		{
			bFailed = true ;
			break ;
		}
		else
			m_ppLevels[i]->ClearPixels();
	}

	if ( bFailed )
	{
		Destroy() ;
		return false ;
	}

	return true ;
}

void CFloatImagePyramid::Construct()
{
	bool bSuccess = Allocate() ;

	// Level 0
	if (m_bRGBAImgSetAsBase)
	{
		int iWidth = m_RGBAImg.Shape().width;
		int iHeight = m_RGBAImg.Shape().height;
		CFloatImg temp( iWidth, iHeight, 1 );
		CopyPixels_RGBAByte2Float( temp, m_RGBAImg );
		VtGaussianSmooth( temp, *m_ppLevels[0], 2.0f, Extend );

		for ( int i = 0 ; i < m_iNumLevels - 1 ; i++ )
		{
			Reduce( i ) ;
	////	Expand( i+1 ) ;
		}
	}
	else
	{
		for ( int i = 0 ; i < m_iNumLevels - 1 ; i++ )
			m_ppLevels[i]->ClearPixels();
	}
}

void CFloatImagePyramid::Reduce( int in_iFromLevel )
{
	VtZoom( *m_ppLevels[in_iFromLevel+1], *m_ppLevels[in_iFromLevel],
            2.0f, 0.0f, 2.0f, 0.0f );
}

void CFloatImagePyramid::Expand( int in_iFromLevel )
{
	VtZoom( *m_ppLevels[in_iFromLevel-1], *m_ppLevels[in_iFromLevel],
            0.5f, 0.0f, 0.5f, 0.0f );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Image entry points
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void CFloatImagePyramid::SetBase( const CRGBAImg & in_RGBAImg )
{
	m_RGBAImg = in_RGBAImg ;
	m_bRGBAImgSetAsBase = true ;
}

void CFloatImagePyramid::GetImageAtLevel( CRGBAImg & out_Image, int in_iLevel )
{
	CopyPixels_Float2RGBAByte( out_Image, *m_ppLevels[in_iLevel] );
}

#pragma once

#include "Image.h"

typedef CFloatImage CFloatImg;
typedef CByteImage CRGBAImg;

class CFloatImagePyramid
{
public:
	CFloatImagePyramid(void);
	~CFloatImagePyramid(void);

	void Destroy() ;
	void SetNumLevels( int in_iNumLevels ) ;
	bool Allocate() ;
	void Construct() ;
	void Expand( int in_iFromLevel ) ;
	void Reduce( int in_iFromLevel ) ;
	CFloatImg * operator[]( int in_iLevel ) ;

	void SetEmptyBase( int iWidth, int iHeight ) ;
	void SetBase( const CRGBAImg & in_Image ) ;
	void GetImageAtLevel( CRGBAImg & out_Image, int in_iLevel ) ;

private :

	int m_iNumLevels ;
	int m_iEmptyWidth, m_iEmptyHeight;
	bool m_bRGBAImgSetAsBase;
	CFloatImg *m_pFloatImg;
	CFloatImg **m_ppLevels;
	CRGBAImg m_RGBAImg ;
};

///////////////////////////////////////////////////////////////////////////
//
// NAME
//  Image.cpp -- a simple reference-counted image structure
//
// SEE ALSO
//  Image.h             definition and explanation of these classes
//
// Copyright © Richard Szeliski, 2001.  See Copyright.h for more details
//
///////////////////////////////////////////////////////////////////////////

#include "Image.h"

//
// struct CShape: shape of image (width x height x nbands)
//

bool CShape::operator==(const CShape& ref)
{
    // Are two shapes the same?
    return (width  == ref.width  &&
            height == ref.height &&
			depth  == ref.depth  &&
            nBands == ref.nBands);
}

bool CShape::SameIgnoringNBands(const CShape& ref)
{
    // Are two shapes the same ignoring the number of bands?
    return (width  == ref.width  &&
            height == ref.height &&
			depth  == ref.depth);
}

bool CShape::operator!=(const CShape& ref)
{
    // Are two shapes not the same?
    return ! ((*this) == ref);
}


//
// class CImage : generic (weakly typed) image
//

void CImage::SetDefaults()
{
    // Set internal state to default values
    m_pTI       = 0;         // pointer to type_info class
    m_bandSize  = 0;         // size of each band in bytes
    m_pixSize   = 0;         // stride between pixels in bytes
    m_rowSize   = 0;         // stride between rows in bytes
	m_frameSize = 0;           
    m_memStart  = 0;         // start of addressable memory


    // Set default attribute values
    alphaChannel = 3;       // which channel contains alpha (for compositing)
    origin[0] = 0;          // x and y coordinate origin (for some operations)
    origin[1] = 0;          // x and y coordinate origin (for some operations)
	origin[2] = 0;          // x and y coordinate origin (for some operations)
    borderMode = eBorderReflect;   // border behavior for neighborhood operations...
}

CImage::CImage()
{
    // Default constructor
    SetDefaults();
}

CImage::CImage(CShape s, const type_info& ti, int cS)
{
    SetDefaults();
    ReAllocate(s, ti, cS, 0, true);
}

void CImage::ReAllocate(CShape s, const type_info& ti, int bandSize,
                        bool evenIfShapeDiffers)
{
    if (! evenIfShapeDiffers && s == m_shape && ti == *m_pTI && bandSize == m_bandSize)
        return;
    ReAllocate(s, ti, bandSize, 0, true);
}

void CImage::ReAllocate(CShape s, const type_info& ti, int bandSize,
                        void *memory, bool deleteWhenDone)
{
    // Set up the type_id, shape, and size info
    m_shape     = s;                        // image shape (dimensions)
    m_pTI       = &ti;                      // pointer to type_info class
    m_bandSize  = bandSize;                 // size of each band in bytes
    m_pixSize   = m_bandSize * s.nBands;    // stride between pixels in bytes

    // Do the real allocation work
    m_rowSize   = m_pixSize * s.width;
	m_frameSize = m_rowSize * s.height;
    int nBytes  = m_frameSize * s.depth;
    if ((memory == NULL) && (nBytes > 0))          // allocate if necessary
    {
        memory = new double[(nBytes + 7)/ 8];
        if (memory == NULL)
            throw CError("CImage::Reallocate: could not allocate %d bytes", nBytes);
    }
    m_memStart = (char *) memory;           // start of addressable memory
    m_memory.ReAllocate(nBytes, memory, deleteWhenDone);
}

void CImage::DeAllocate()
{
    // Release the memory & set to default values
    ReAllocate(CShape(), *(const type_info *) 0, 0, 0, false);
    SetDefaults();
}

void CImage::ClearPixels(void)
{
	memset(m_memStart, 0, m_shape.depth * m_frameSize);
}

void CImage::SetFrame(int z)
{
	ENSURE((z >= 0) && (z < m_shape.depth));

	m_shape.depth = 1;
	m_memStart += (m_frameSize * z);
	origin[2] = 0;
}

//
// class CImageOf<T>: strongly typed image
//

#include <float.h>

template<> uchar CImageOf<uchar>::MinVal(void)     { return 0;         }
template<> uchar CImageOf<uchar>::MaxVal(void)     { return UCHAR_MAX; }
template<> int   CImageOf<int  >::MinVal(void)     { return INT_MIN;   }
template<> int   CImageOf<int  >::MaxVal(void)     { return INT_MAX;   }
template<> float CImageOf<float>::MinVal(void)     { return -FLT_MAX;  }
template<> float CImageOf<float>::MaxVal(void)     { return FLT_MAX;   }



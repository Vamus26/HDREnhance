#ifndef __IMAGE_INL__
#define __IMAGE_INL__

///////////////////////////////////////////////////////////////////////////
//
// NAME
//  Image.inl -- a simple reference-counted image structure
//
// DESCRIPTION
//  The incline function definitions to go with Image.h
//
// SEE ALSO
//  Image.h             more complete description
//
// Copyright © Richard Szeliski, 2001.  See Copyright.h for more details
//
///////////////////////////////////////////////////////////////////////////

//
// struct CShape: shape of image (width x height x nbands)
//

inline bool CShape::InBounds(int x, int y)
{
    // Is given pixel address valid?
    return ((0 <= x) && (x < width) &&
            (0 <= y) && (y < height));
}

inline bool CShape::InBounds(uint x, uint y)
{
    // Is given pixel address valid?
    return ((0 <= x) && (x < (uint) width) &&
            (0 <= y) && (y < (uint) height));
}


inline bool CShape::InBounds(float x, float y)
{
    // Is given pixel address valid?
    return ((0 <= x) && (x <= (width - 1)) &&
            (0 <= y) && (y <= (height - 1)));
}

inline bool CShape::InBounds(double x, double y)
{
    // Is given pixel address valid?
    return ((0 <= x) && (x <= (width - 1)) &&
            (0 <= y) && (y <= (height - 1)));
}

inline bool CShape::InBounds(int x, int y, int b)
{
    // Is given pixel address valid?
    return ((0 <= x) && (x < width)  &&
            (0 <= y) && (y < height) &&
            (0 <= b) && (b < nBands));
}

inline bool CShape::InBounds(int x, int y, int z, int b)
{
    // Is given pixel address valid?
    return ((0 <= x) && (x < width)   &&
            (0 <= y) && (y < height)  &&
			(0 <= z) && (z < depth)   &&
            (0 <= b) && (b < nBands));
}

inline uint CShape::NodeIndex(int x, int y, int z)
{
	return (z * (width * height)) + (y * width) + x;
}

inline uint CShape::NodeIndex(int x, int y)
{
	return (y * width) + x;
}

inline uint CShape::PixelIndex(int x, int y, int b)
{
	return (y * width * nBands) + (x * nBands) + b;
}

inline uint CShape::PixelIndex(int x, int y, int z, int b)
{
	int rowSize = width * nBands;
	return (z * rowSize * height) + (y * rowSize) + (x * nBands) + b;
}

//
// class CImage : generic (weakly typed) image
//

inline void* CImage::PixelAddress(int x, int y, int band) const
{
    // This could also go into the implementation file (CImage.cpp):
    return (void *) &m_memStart[(y * m_rowSize) + (x * m_pixSize) + (band * m_bandSize)];
}

inline void* CImage::PixelAddress(int x, int y, int z, int band) const
{
    // This could also go into the implementation file (CImage.cpp):
    return (void *) &m_memStart[(z * m_frameSize) + (y * m_rowSize) + (x * m_pixSize) + (band * m_bandSize)];
}


inline uint CImage::PixelIndex(int x, int y, int band) const
{
    // This could also go into the implementation file (CImage.cpp):

	return ((y * (m_shape.width * m_shape.nBands)) + (x * m_shape.nBands) + band);
	
}

inline uint CImage::PixelIndex(int x, int y, int z, int band) const
{
    // This could also go into the implementation file (CImage.cpp):    
	int rowWidth = m_shape.width * m_shape.nBands;
	return ((z * (rowWidth * m_shape.height)) + (y * rowWidth) + (x * m_shape.nBands) + band);
}

inline uint CImage::NodeIndex(int x, int y)
{
    // This could also go into the implementation file (CImage.cpp):

	return ((y * m_shape.width) + x);
	
}

inline uint CImage::NodeIndex(int x, int y, int z)
{
    // This could also go into the implementation file (CImage.cpp):    
	return (z * (m_shape.width * m_shape.height)) + (y * m_shape.width) + x;
}


//  Strongly typed image;
//    these are defined inline so user-defined image types can be supported:

template <class T>
inline CImageOf<T>::CImageOf(void) :
CImage(CShape(), typeid(T), sizeof(T)) {}

template <class T>
inline CImageOf<T>::CImageOf(CShape s) :
CImage(s, typeid(T), sizeof(T)) {}

template <class T>
inline CImageOf<T>::CImageOf(int width, int height, int nBands) :
CImage(CShape(width, height, nBands), typeid(T), sizeof(T)) {}

template <class T>
inline void CImageOf<T>::ReAllocate(CShape s, bool evenIfShapeDiffers)
{
    CImage::ReAllocate(s, typeid(T), sizeof(T), evenIfShapeDiffers);
}

template <class T>
inline void CImageOf<T>::ReAllocate(CShape s, T *memory, bool deleteWhenDone)
{
	ReAllocate(s, typeid(T), sizeof(T), memory, deleteWhenDone);
}
    
template <class T>
inline T& CImageOf<T>::Pixel(int x, int y, int band) const
{
    return *(T *) PixelAddress(x, y, band);
}

template <class T>
inline T& CImageOf<T>::Pixel(int x, int y, int z, int band) const
{
    return *((T *) PixelAddress(x, y, z, band));
}

template <class T>
inline T& CImageOf<T>::operator[](int index) const
{
    return *((T *) (m_memStart + (index * m_bandSize)));
}

template <class T>
inline CImageOf<T> CImageOf<T>::GetFrame(int z)
{
    CImageOf<T> retval = *this;
	retval.SetFrame(z);
    return retval;
}

template <class T>
inline CImageOf<T> CImageOf<T>::Clone() const
{
    CImageOf<T> clone = *this;
	clone.ReAllocate(this->Shape(), true);
	memcpy(clone.m_memStart, this->m_memStart, this->m_frameSize * this->m_shape.depth);
    return clone;
}

#endif


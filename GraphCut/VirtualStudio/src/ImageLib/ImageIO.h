///////////////////////////////////////////////////////////////////////////
//
// NAME
//  FileIO.h -- image file input/output
//
// DESCRIPTION
//  Read/write image files, potentially using an interface to an
//  external package.
//
//  You can either pass and empty (unitialized) image to ReadFile,
//  or one you have already allocated (with a specific pixel type).
//  
//  If you don't initialize the image, the type of the returned image
//  (e.g., 1 band vs. 4 band) will be determined by the image file.
//  If you do initialize the image, it will be re-allocated if necessary,
//  and the data will be coerced into the type you specified.
//
// SEE ALSO
//  FileIO.cpp          implementation
//
// Copyright © Richard Szeliski, 2001.  See Copyright.h for more details
//
///////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_IO_H__
#define __IMAGE_IO_H__

#include "Image.h"
#include "tiff.h"

class ImageIO
{
public:

	template <class T>
	static void WriteFileTGA(CImageOf<T> &img, const char* filename);

	template <class T>
	static void ReadFileTGA(CImageOf<T> &img, const char* filename);

	static void WriteFileTiff(CByteImage &img, const char* filename, 
							  int compressionType = COMPRESSION_NONE);

	static void WriteFileTiff(CByteImage &img, string filename, 
							  int compressionType = COMPRESSION_NONE)
	{
		WriteFileTiff(img, filename.c_str(), compressionType);
	}

	static void WriteFileTiff(CFloatImage &img, const char* filename,
							  int compressionType = COMPRESSION_NONE);

	static void WriteFileTiff(CFloatImage &img, string filename, 
							  int compressionType = COMPRESSION_NONE)
	{
		WriteFileTiff(img, filename.c_str(), compressionType);
	}

	static void ReadFileTiff(CByteImage &img,  const char* filename);
	static void ReadFileTiff(CFloatImage &img, const char* filename);

	static void ReadFile(CFloatImage &img,  const char* filename, float scale);
	static void ReadFile(CFloatImage &img,  const char* filename);
	static void WriteFile(CFloatImage& img, const char* filename);

	static void ReadFile(CByteImage &img,  const char* filename);
	static void WriteFile(CByteImage &img, const char* filename);

	static void ReadFile(CFloatImage &img, string filename, float scale)
	{
		ReadFile(img, filename.c_str(), scale);
	}
	static void ReadFile(CFloatImage &img, string filename)
	{
		ReadFile(img, filename.c_str());
	}
	static void ReadFile(CByteImage &img, string filename)
	{
		ReadFile(img, filename.c_str());
	}

	static void WriteFile(CFloatImage& img, const string &filename)
	{
		WriteFile(img, filename.c_str());
	}
	static void WriteFile(CByteImage& img, const string &filename)
	{
		WriteFile(img, filename.c_str());
	}

	static void WriteVideo(CFloatImage &video, const char* filePrefix);
	static void WriteVideo(CFloatImage &video, string filePrefix)
	{
		WriteVideo(video, filePrefix.c_str());
	}


	static CShape GetImageShape(const char *fileName,  float scale = 1.0f);
	static CShape GetImageShape(string fileName, float scale = 1.0f)
	{
		return GetImageShape(fileName.c_str(), scale);
	}		
	static CShape GetImageShapeTiff(const char *fileName, float scale = 1.0f);

	template<class T>
	static void ReadRaw(CImageOf<T> &rawImage, 
						const char *fname,
						float scale = 1.0f);

	template<class T>
	static void ReadRaw(CImageOf<T> &rawImage, 
						string fname,
						float scale = 1.0f)
	{
		ReadRaw(rawImage, fname.c_str(), scale);
	}

	template<class T>
	static void WriteRaw(CImageOf<T> &image,
						 const char *fname);

	template<class T>
	static void WriteRaw(CImageOf<T> &rawImage,
						 string fname)
	{
		WriteRaw(rawImage, fname.c_str());
	}
};

#include "ImageIO.inl"
#include "ImageIO-TGA.inl"

#endif



///////////////////////////////////////////////////////////////////////////
//
// NAME
//  FileIO.cpp -- image file input/output
//
// DESCRIPTION
//  Read/write image files, potentially using an interface to an
//  external package.
//
//  Currently, only Targa input/output is supported (grayscale and color
//  byte images), and only a subset of Targa formats in supported.
//
// SEE ALSO
//  FileIO.h            longer description
//
// Copyright © Richard Szeliski, 2001.  See Copyright.h for more details
//
///////////////////////////////////////////////////////////////////////////

#include "ImageIO.h"
#include "Debug.h"
#include "ImageProcessing.h"



void ImageIO::ReadFile(CFloatImage &img, const char* filename, float scale)
{
	if(scale == 1.0f)
	{
		ReadFile(img, filename);
	}
	else
	{
		CFloatImage tempImg;
		ReadFile(tempImg, filename);
		if(scale > 1.0f)
			ImageProcessing::ScaleImage(img, tempImg, scale, false);
		else
			ImageProcessing::ScaleImage(img, tempImg, scale);
	}
}

void ImageIO::WriteFile(CFloatImage& img, const char* filename)
{
    // Determine the file extension

	const char *dot = strrchr(filename, '.');
	if(dot == NULL)
	{
		REPORT_FAILURE("No file extention found - %s", filename);
	}

	if(strcmp(dot, ".tga") == 0)
    {
		WriteFileTGA(img, filename);
    }
	else if(strcmp(dot, ".tif") == 0)
    {
		WriteFileTiff(img, filename);
	}
	else if(strcmp(dot, ".raw") == 0)
	{
		WriteRaw(img, filename);
	}
    else
	{
		REPORT_FAILURE("File type not supported - %s", filename);
	}
}

void ImageIO::WriteFile(CByteImage &img, const char* filename)
{
    // Determine the file extension
    const char *dot = strrchr(filename, '.');
	if(dot == NULL)
	{
		REPORT_FAILURE("No file extention found - %s", filename);
	}

	if(strcmp(dot, ".tga") == 0)
    {
		WriteFileTGA(img, filename);
    }
	else if(strcmp(dot, ".tif") == 0)
    {
		WriteFileTiff(img, filename);
	}
	else if(strcmp(dot, ".raw") == 0)
	{
		WriteRaw(img, filename);
	}
    else
	{
		REPORT_FAILURE("File type not supported - %s", filename);
	}
}

void ImageIO::ReadFile(CFloatImage &img, const char* filename)
{
    // Determine the file extension
    const char *dot = strrchr(filename, '.');
	if(dot == NULL)
	{
		REPORT_FAILURE("No file extention found - %s", filename);
	}

	if(strcmp(dot, ".tga") == 0)
    {
		ReadFileTGA(img, filename);
    }
	else if(strcmp(dot, ".tif") == 0)
    {
		ReadFileTiff(img, filename);
	}
	else if(strcmp(dot, ".raw") == 0)
	{
		ReadRaw(img, filename);
	}
    else
	{
		REPORT_FAILURE("File type not supported - %s", filename);
	}
}

void ImageIO::ReadFile(CByteImage &img, const char* filename)
{
    // Determine the file extension
    const char *dot = strrchr(filename, '.');
	if(dot == NULL)
	{
		REPORT_FAILURE("No file extention found - %s", filename);
	}

	if(strcmp(dot, ".tga") == 0)
    {
		ReadFileTGA(img, filename);
    }
	else if(strcmp(dot, ".tif") == 0)
    {
		ReadFileTiff(img, filename);
	}
	else if(strcmp(dot, ".raw") == 0)
	{
		ReadRaw(img, filename);
	}
    else
	{
		REPORT_FAILURE("File type not supported - %s", filename);
	}
}

CShape ImageIO::GetImageShape(const char *fileName, float scale)
{
	CShape imgShape;

	const char *dot = strrchr(fileName, '.');
	if(dot == NULL)
	{
		REPORT_FAILURE("No file extention found - %s", fileName);
	}

	if(strcmp(dot, ".tif") == 0)
    {
		imgShape = GetImageShapeTiff(fileName, scale);
    }
	else
    {
		CFloatImage testImg;
		ImageIO::ReadFile(testImg, fileName, scale);	
		imgShape = testImg.Shape();		
	}

	return imgShape;
}

void ImageIO::WriteVideo(CFloatImage &video, const char* filePrefix)
{
	char frameName[1024];

	CShape videoShape = video.Shape();
	ENSURE(videoShape.depth > 1);
	for(int iFrame = 0; iFrame < videoShape.depth; iFrame++)
	{
		sprintf(frameName, "%s%05i.tga", filePrefix, iFrame);
		CFloatImage currFrame = video.GetFrame(iFrame);
		ImageIO::WriteFile(currFrame, frameName);
	}
}



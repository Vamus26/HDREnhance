#include "ImageIO.h"

#include "tiffio.h"

void ImageIO::WriteFileTiff(CByteImage &img, const char* filename, int compressionType)
{
	CShape imgShape = img.Shape();
	ENSURE((imgShape.nBands == 1) || (imgShape.nBands == 3));

	int sampleperpixel = imgShape.nBands;

	TIFF *out= TIFFOpen(filename, "w");	
	ENSURE(out != NULL);
	
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, imgShape.width);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, imgShape.height);      
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);   
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);                  
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_BOTLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	TIFFSetField(out, TIFFTAG_COMPRESSION, compressionType);
	int dataCount = imgShape.width * imgShape.height * imgShape.nBands;
    if(TIFFWriteEncodedStrip(out, 0, 
							 img.PixelAddress(0, 0, 0), dataCount) != dataCount)
	{
		REPORT_FAILURE("Error while writing file - %s\n", filename);
	}
	TIFFClose(out);
}

void ImageIO::WriteFileTiff(CFloatImage &img, const char* filename, int compressionType)
{
	CByteImage byteImage = ImageProcessing::FloatToByteImg(img);
	ImageIO::WriteFileTiff(byteImage, filename, compressionType);
}

CShape ImageIO::GetImageShapeTiff(const char *filename, float scale)
{
	CShape imgShape;

	TIFF* tif = TIFFOpen(filename, "r");
	ENSURE(tif != NULL);

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,      &imgShape.width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH,     &imgShape.height);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &imgShape.nBands);
	ENSURE((imgShape.nBands == 1) || (imgShape.nBands == 3));

	TIFFClose(tif); 

	ENSURE(scale > 0.0f);
	imgShape.width  = (int) (imgShape.width * scale);
	imgShape.height = (int) (imgShape.height * scale);

	return imgShape;
}


void ImageIO::ReadFileTiff(CByteImage &img, const char* filename)
{
	TIFF* tif = TIFFOpen(filename, "r");
	ENSURE(tif != NULL);

	CShape imgShape;
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,      &imgShape.width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH,     &imgShape.height);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &imgShape.nBands);
	ENSURE((imgShape.nBands == 1) || (imgShape.nBands == 3));
	img.ReAllocate(imgShape);

	unsigned short currField;
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &currField);
	ENSURE(currField == 8);

	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &currField);
	ENSURE(currField == PLANARCONFIG_CONTIG);

	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC,  &currField);
	ENSURE(currField == PHOTOMETRIC_RGB);	

	int stripCount = TIFFNumberOfStrips(tif);
	int stripSize  = TIFFStripSize(tif);
	for(int iStrip = 0; iStrip < stripCount; iStrip++)
	{
		uchar *dataPtr = (uchar *) img.PixelAddress(0, 0, 0);
		dataPtr += (iStrip * stripSize);

		if(TIFFReadEncodedStrip(tif, iStrip, dataPtr, stripSize) == -1)
		{
			REPORT_FAILURE("Error while reading file - %s\n", filename);
		}
	}

	TIFFGetField(tif, TIFFTAG_ORIENTATION,  &currField);
	switch(currField)
	{
	case ORIENTATION_TOPLEFT:
		img = ImageProcessing::FlipVertical(img);
		break;
	case ORIENTATION_BOTLEFT:
		break;
	default:
		REPORT_FAILURE("Unknown image orientation\n");
		break;
	}

	TIFFClose(tif); 
}


void ImageIO::ReadFileTiff(CFloatImage &img, const char* filename)
{
	CByteImage byteImage;
	ImageIO::ReadFileTiff(byteImage, filename);
	img = ImageProcessing::ByteToFloatImg(byteImage);
}



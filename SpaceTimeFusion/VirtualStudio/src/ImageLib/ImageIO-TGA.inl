#include "ImageIO.h"
#include "Debug.h"
#include "ImageProcessing.h"

//
//  Truevision Targa (TGA):  support 24 bit RGB and 32-bit RGBA files
//

typedef unsigned char uchar;

struct CTargaHead
{
    uchar idLength;     // number of chars in identification field
    uchar colorMapType;	// color map type
    uchar imageType;	// image type code
    uchar cMapOrigin[2];// color map origin
    uchar cMapLength[2];// color map length
    uchar cMapBits;     // color map entry size
    short x0;			// x-origin of image
    short y0;			// y-origin of image
    short width;		// width of image
    short height;		// height of image
    uchar pixelSize;    // image pixel size
    uchar descriptor;   // image descriptor byte
};

// Image data type codes
const int TargaRawColormap	= 1;
const int TargaRawRGB		= 2;
const int TargaRawBW		= 3;
const int TargaRunColormap	= 9;
const int TargaRunRGB		= 10;
const int TargaRunBW		= 11;

// Descriptor fields
const int TargaAttrBits     = 15;
const int TargaScreenOrigin = (1<<5);
const int TargaCMapSize		= 256;
const int TargaCMapBands    = 3;
/*
const int TargaInterleaveShift = 6;
const int TargaNON_INTERLEAVE	0
const int TargaTWO_INTERLEAVE	1
const int TargaFOUR_INTERLEAVE	2
const int PERMUTE_BANDS		1
*/


template <class T>
void ImageIO::ReadFileTGA(CImageOf<T> &img, const char* filename)
{
	ENSURE((typeid(T) == typeid(float)) || (typeid(T) == typeid(uchar)));
    // Open the file and read the header
    FILE *stream = fopen(filename, "rb");
    if (stream == 0)
	{
        REPORT_FAILURE("ReadFileTGA: could not open %s", filename);
	}
    CTargaHead h;
    if (fread(&h, sizeof(CTargaHead), 1, stream) != 1)
	{
	    REPORT_FAILURE("ReadFileTGA(%s): file is too short", filename);
	}

    // Throw away the image descriptor
    if (h.idLength > 0)
    {
        char* tmp = new char[h.idLength];
        size_t nread = fread(tmp, sizeof(uchar), h.idLength, stream);
        delete [] tmp;   // throw away this data
        if (nread != h.idLength)
		{
	        REPORT_FAILURE("ReadFileTGA(%s): file is too short", filename);
		}
    }

    bool reverseRows = (h.descriptor & TargaScreenOrigin) != 0;
    int fileBytes = (h.pixelSize + 7) / 8;

	if(fileBytes >= 4)
	{
		REPORT_FAILURE("ReadFileTGA(%s): Only 24-bit or lower bpp supported", filename);
	}

    bool grayRamp = false;
    if (h.colorMapType == 1)
    {
		REPORT_FAILURE("TGA type not supported");
    }
    bool isGray = 
        h.imageType == TargaRawBW || h.imageType == TargaRunBW ||
        grayRamp &&
        (h.imageType == TargaRawColormap || h.imageType == TargaRunColormap);
    bool isRaw = h.imageType == TargaRawBW || h.imageType == TargaRawRGB ||
        h.imageType == TargaRawRGB && isGray;

    // Determine the image shape
    CShape sh(h.width, h.height, (isGray) ? 1 : 3);
    
    // Allocate the image if necessary
    img.ReAllocate(sh);

	float div;
	uchar *buff = NULL;
	if(typeid(T) == typeid(uchar))
	{
		div = 1.0f;
	}
	else
	{
		div = 255.0f;
		buff = new uchar[sh.width * sh.nBands];
		ENSURE(buff != NULL);
	}

    // Read in the rows
    for (int y = 0; y < sh.height; y++)
    {
        int yr = reverseRows ? (sh.height - 1 - y) : y;
        T* ptr = (T*) img.PixelAddress(0, yr, 0);
        if (fileBytes == sh.nBands && isRaw)
        {
            size_t n = sh.width*sh.nBands;
			if(typeid(T) == typeid(uchar))
			{				
    			if (fread(ptr, sizeof(uchar), n, stream) != n)
				{
    				REPORT_FAILURE("ReadFileTGA(%s): file is too short", filename);
				}
			}
			else
			{				
				if (fread(buff, sizeof(uchar), n, stream) != n)
				{
    				REPORT_FAILURE("ReadFileTGA(%s): file is too short", filename);
				}

				for(size_t i = 0; i < n; i++)
				{
					ptr[i] = (T) (buff[i] / div);
				}
			}

			if(sh.nBands == 3)
			{
				uint addr = 0;
				for(int x = 0; x < sh.width; x++, addr += 3)
				{
					T temp        = ptr[addr];
					ptr[addr]     = ptr[addr + 2];
					ptr[addr + 2] = temp;
				}
			}
        }
        else
        {
			REPORT_FAILURE("TGA type not supported");
        }
    }

    if (fclose(stream))
	{
        REPORT_FAILURE("ReadFileTGA(%s): error closing file", filename);
	}

	delete [] buff;
}

template <class T>
void ImageIO::WriteFileTGA(CImageOf<T> &img, const char* filename)
{
	ENSURE((typeid(T) == typeid(float)) || (typeid(T) == typeid(uchar)));

    // Only 1, 3, or 4 bands supported
    CShape sh  = img.Shape();
    int nBands = sh.nBands;
    if (nBands != 1 && nBands != 3 && nBands != 4)
	{
        REPORT_FAILURE("WriteFileTGA(%s): can only write 1, 3, or 4 bands. Given %i bands", filename, nBands);
	}
	ENSURE(sh.depth == 1);

      // Fill in the header structure
    CTargaHead h;
    memset(&h, 0, sizeof(h));
    h.imageType = (nBands == 1) ? TargaRawBW : TargaRawRGB;
        // TODO:  is TargaRawBW the right thing, or only binary?
    h.width     = (short) sh.width;
    h.height    = (short) sh.height;
    h.pixelSize = (uchar) (8 * nBands);
    bool reverseRows = false;   // TODO: when is this true?

    // Open the file and write the header
    FILE *stream = fopen(filename, "wb");
    if (stream == 0)
	{
        REPORT_FAILURE("WriteFileTGA: could not open %s", filename);
	}
    if (fwrite(&h, sizeof(CTargaHead), 1, stream) != 1)
	{
	    REPORT_FAILURE("WriteFileTGA(%s): couldn't write image header", filename);
	}

	uchar *buff = NULL;
	buff = new uchar[sh.width*sh.nBands];
	ENSURE(buff != NULL);

    // Write out the rows
    for (int y = 0; y < sh.height; y++)
    {
        int yr = reverseRows ? (sh.height - 1 - y) : y;
        size_t n = sh.width*sh.nBands;

		if((typeid(T) == typeid(uchar)) &&
		   (sh.nBands == 1))
		{
			uchar* ptr = (uchar *) img.PixelAddress(0, yr, 0);
    		if(fwrite(ptr, sizeof(uchar), n, stream) != n)
			{
	    	    REPORT_FAILURE("WriteFileTGA(%s): couldn't write image data", filename);
			}
		}
		else
		{
			T* ptr = (T*) img.PixelAddress(0, yr, 0);
			
			if(typeid(T) == typeid(uchar))
			{
				for(size_t i = 0; i < n; i++)
				{
					buff[i] = (uchar) ptr[i];
				}
			}
			else
			{
			
				for(size_t i = 0; i < n; i++)
				{
					float val = ptr[i] * 255.0f;
					val = min(val, 255.0f);
					val = max(val, 0.0f);
					buff[i] = (uchar) val;
				}
			}

			if(sh.nBands == 3)
			{
				uint addr = 0;
				for(int x = 0; x < sh.width; x++, addr += sh.nBands)
				{
					uchar temp     = buff[addr];
					buff[addr]     = buff[addr + 2];
					buff[addr + 2] = temp;
				}
			}

    		if (fwrite(buff, sizeof(uchar), n, stream) != n)
			{
				REPORT_FAILURE("WriteFileTGA(%s): couldn't write image data", filename);
			}
		}
    }

    if (fclose(stream))
	{
		REPORT_FAILURE("WriteFileTGA(%s): error closing file", filename);
	}

	delete [] buff;
}



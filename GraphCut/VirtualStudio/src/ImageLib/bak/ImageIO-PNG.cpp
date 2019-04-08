#ifndef __IMAGE_IO_PNG_INL__
#define __IMAGE_IO_PNG_INL__

#include "png.h"
#include "ImageIO.h"

#pragma warning(disable : 4611)

void ImageIO::WriteFilePNG(CByteImage &img, const char* fileName)
{
	CShape imgShape = img.Shape();
	ENSURE((imgShape.nBands == 1) || (imgShape.nBands == 3))
	int color_type = (imgShape.nBands == 1) ? PNG_COLOR_TYPE_GRAY : PNG_COLOR_TYPE_RGB;

	// open the file
	FILE *fp = fopen(fileName, "wb");
	ENSURE(fp != NULL);

	// create the needed data structures
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	ENSURE(png_ptr != NULL);

	png_infop info_ptr = png_create_info_struct(png_ptr);
	ENSURE(info_ptr != NULL);

	// do the setjmp thingy
	ENSURE(setjmp(png_jmpbuf(png_ptr)) == 0);

	// set up the io
	png_init_io(png_ptr, fp);	
	
	// write the header
	png_set_IHDR(png_ptr, info_ptr, 
				 imgShape.width, imgShape.height, 
				 8, color_type, PNG_INTERLACE_NONE, 
				 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	
	//png_write_info(png_ptr, info_ptr);
	//exit(0);

	//png_set_flush(png_ptr, 10);	

	// write the image
	png_bytep *row_pointers = new png_bytep[imgShape.height];
	for (int y = 0 ; y < imgShape.height ; y++)
		row_pointers[y] = (png_byte *) img.PixelAddress(0, imgShape.height - 1 - y, 0);

	png_set_rows(png_ptr,  info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, png_voidp_NULL);

	//png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, info_ptr);

	delete row_pointers;

	// clean up memory, and finish
	fclose(fp);
	png_destroy_write_struct(&png_ptr, &info_ptr);
}

#endif
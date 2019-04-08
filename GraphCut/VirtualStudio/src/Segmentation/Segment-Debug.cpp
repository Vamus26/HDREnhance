#include "Segment.h"

//assumes segment bounds is correct as tested by CheckBoundTighness
void Segment::CheckSizeAndColorConsistency(const CIntImage &labelImg, const CFloatImage &smoothImg) const
{
	CShape imgShape = smoothImg.Shape();

	int segPixelsFound = 0;
	ENSURE(imgShape.nBands <= 3);
	Vector3<float> avgSegCol(0.0f, 0.0f, 0.0f);	
	for(int y = this->bounds.minY; y <= this->bounds.maxY; y++)
	for(int x = this->bounds.minX; x <= this->bounds.maxX; x++)
	{
		int currPixelLabel = labelImg.Pixel(x, y, 0);
		if(currPixelLabel == this->id)
		{
			segPixelsFound++;
			for(int channel = 0; channel < imgShape.nBands; channel++)
			{
				//multiply and div by 255 to simulate integer disk write/read
				avgSegCol[channel] += ((int) (smoothImg.Pixel(x, y, channel) * 255.0f)) / 255.0f;
			}
		}
	}

	ENSURE(segPixelsFound == this->size);

	for(int channel = 0; channel < imgShape.nBands; channel++)
	{
		avgSegCol[channel] /= this->size;
		ENSURE(fabs(this->avgCol[channel] - avgSegCol[channel]) < 0.003f);		
	}
}

void Segment::CheckBoundTighness(const CIntImage &labelImg) const
{
	CShape imgShape = labelImg.Shape();

	CHECK_RANGE(this->bounds.minX, 0, imgShape.width - 1);
	CHECK_RANGE(this->bounds.maxX, 0, imgShape.width - 1);
	CHECK_RANGE(this->bounds.minY, 0, imgShape.height - 1);
	CHECK_RANGE(this->bounds.maxY, 0, imgShape.height - 1);

	ENSURE(this->bounds.minX <= this->bounds.maxX);
	ENSURE(this->bounds.minY <= this->bounds.maxY);

	const int sides = 4;
	int yStart[sides] = {this->bounds.minY, this->bounds.maxY, this->bounds.minY, this->bounds.minY};
	int yEnd[sides]   = {this->bounds.minY, this->bounds.maxY, this->bounds.maxY, this->bounds.maxY};
	int xStart[sides] = {this->bounds.minX, this->bounds.minX, this->bounds.minX, this->bounds.maxX};
	int xEnd[sides]   = {this->bounds.maxX, this->bounds.maxX, this->bounds.minX, this->bounds.maxX};

	for(int iSide = 0; iSide < sides; iSide++)
	{
		bool pixelFound = false;
		for(int y = yStart[iSide]; (y <= yEnd[iSide]) && (pixelFound == false); y++)
		for(int x = xStart[iSide]; (x <= xEnd[iSide]) && (pixelFound == false); x++)
		{
			if(labelImg.Pixel(x, y, 0) == this->id)
			{
				pixelFound = true;
			}
		}
		ENSURE(pixelFound == true);
	}	
}

void Segment::Dump() const
{
	PRINT_INT(this->id);
	PRINT_INT(this->size);
	printf("Bounds: ");
	this->bounds.Dump();
}

CByteImage Segment::GetSegVis(const CIntImage &labelImg) const
{
	CByteImage segVis(labelImg.Shape());
	segVis.ClearPixels();

	for(int y = this->bounds.minY; y <= this->bounds.maxY; y++)
	for(int x = this->bounds.minX; x <= this->bounds.maxX; x++)
	{
		int currPixelLabel = labelImg.Pixel(x, y, 0);
		if(currPixelLabel == this->id)
		{
			segVis.Pixel(x, y, 0) = MASK_VALID;
		}
	}

	return segVis;
}

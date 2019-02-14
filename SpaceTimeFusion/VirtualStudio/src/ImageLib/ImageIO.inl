template<class T>
void ImageIO::WriteRaw(CImageOf<T> &rawImage, const char *fname)
{
	CShape imgShape = rawImage.Shape();	
	ENSURE(imgShape.depth == 1);

	uint frameSize = imgShape.width * imgShape.height * imgShape.nBands;
	T *tempBuffer  = new T[frameSize]; 
	ENSURE(tempBuffer != NULL);

	uint nodeAddr = 0;
	for (int y = 0; y < imgShape.height; y++)
	{
		for(int x = 0; x < imgShape.width; x++)
		{
			for(int b = 0; b < imgShape.nBands; b++, nodeAddr++)
			{
				tempBuffer[nodeAddr] = rawImage[nodeAddr];
			}
		}
	}

	FILE *filePtr = fopen(fname, "wb");
	ENSURE(filePtr != NULL);
	if((fwrite(&imgShape.width,  sizeof(imgShape.width),  1, filePtr) != 1) ||
	   (fwrite(&imgShape.height, sizeof(imgShape.height), 1, filePtr) != 1) ||
	   (fwrite(&imgShape.nBands, sizeof(imgShape.nBands), 1, filePtr) != 1))
	{
		REPORT_FAILURE("Error writing raw file");
	}
	if(fwrite(tempBuffer, sizeof(*tempBuffer), frameSize, filePtr) != frameSize)
	{
		REPORT_FAILURE("Error writing raw file");
	}
	fclose(filePtr);

	delete [] tempBuffer;
}


template<class T>
void ImageIO::ReadRaw(CImageOf<T> &rawImage,
					  const char *fname, 
					  float scale)
{	
	FILE *filePtr = fopen(fname, "rb");
	if(filePtr == NULL)
	{
		PRINT_STR(fname);
	}
	ENSURE(filePtr != NULL);

	CShape imgShape;
	imgShape.depth  = 1;
	if((fread(&imgShape.width,  sizeof(imgShape.width),  1, filePtr) != 1) ||
	   (fread(&imgShape.height, sizeof(imgShape.height), 1, filePtr) != 1) ||
	   (fread(&imgShape.nBands, sizeof(imgShape.nBands), 1, filePtr) != 1))
	{
		REPORT_FAILURE("Error reading raw file");
	}

	uint frameSize = imgShape.width * imgShape.height * imgShape.nBands;
	T *tempBuffer  = new T[frameSize]; 
	ENSURE(tempBuffer != NULL);

	if(fread(tempBuffer, sizeof(*tempBuffer), frameSize, filePtr) != frameSize)
	{
		REPORT_FAILURE("Error reading raw file");
	}
	fclose(filePtr);

	rawImage.ReAllocate(imgShape);
	uint nodeAddr = 0;
	for (int y = 0; y < imgShape.height; y++)
	{
		for(int x = 0; x < imgShape.width; x++)
		{
			for(int b = 0; b < imgShape.nBands; b++, nodeAddr++)
			{
				rawImage[nodeAddr] = tempBuffer[nodeAddr];
			}
		}
	}

	delete [] tempBuffer;

	if(scale != 1.0f)
	{			
		REPORT_FAILURE("Scaling raw images not implemented\n");
	}
}



#include "RegistrationDefs.h"
#include "ImageProcessing.h"
#include "FlowHelper.h"
#include "Disk.h"

#pragma warning(disable : 4127)

void RegisteredPieces::WriteRegData(const char *fileName, const RegisteredPiece &piece)
{
	ofstream outStream(fileName);
	ENSURE(outStream.is_open());

	outStream << piece.id << '\n';

	outStream.close();
}

void RegisteredPieces::ReadRegData(const char *fileName, RegisteredPiece &piece)
{
	ifstream inStream(fileName);
	ENSURE(inStream.is_open());

	inStream >> piece.id;
	ENSURE(piece.id >= 0);

	inStream.close();
}

CByteImage RegisteredPieces::ReadRegPiecesSpaceMask(const char *path, bool readFast)
{
	char fileName[1024];

	CByteImage spaceMask;
	
	int iPiece = 0;
	while(true)
	{		
		if(readFast == true)
			sprintf(fileName, "%s-mask%05i.raw", path, iPiece);
		else
			sprintf(fileName, "%s-mask%05i.tga", path, iPiece);

		if(Disk::DoesFileExist(fileName) == false) break;

		CByteImage currPieceMask;
		if(readFast == true)
			ImageIO::ReadRaw(currPieceMask, fileName);
		else
			ImageIO::ReadFile(currPieceMask, fileName);

		if(iPiece == 0)
		{
			spaceMask.ReAllocate(currPieceMask.Shape());
			spaceMask.ClearPixels();
		}
		else
		{
			ENSURE(spaceMask.Shape() == currPieceMask.Shape());
		}

		CShape maskShape = spaceMask.Shape();
		uint nodeAddr = 0;
		for(int y = 0; y < maskShape.height; y++)
		{
			for(int x = 0; x < maskShape.width; x++, nodeAddr++)
			{
				if(currPieceMask[nodeAddr] == MASK_VALID)
				{
					spaceMask[nodeAddr] = MASK_VALID;
				}
			}
		}

		iPiece++;
	}

	return spaceMask;
}



RegisteredPieces RegisteredPieces::ReadRegisteredPieces(const CFloatImage &targetImage, int dataCostWinSize, const char *path, bool readFast)
{
	char fileName[1024];

	RegisteredPieces registeredPieces;

	int iPiece = 0;
	while(true)
	{
		RegisteredPiece piece;

		if(readFast == true)
		{
			sprintf(fileName, "%s-image%05i.raw", path, iPiece);
			if(Disk::DoesFileExist(fileName) == false)
			{
				break;
			}
			ImageIO::ReadRaw(piece.image, fileName);

			sprintf(fileName, "%s-mask%05i.raw", path, iPiece);
			ImageIO::ReadRaw(piece.mask, fileName);

			sprintf(fileName, "%s-filter%05i.raw", path, iPiece);
			ImageIO::ReadRaw(piece.filteredImage, fileName);
		}
		else
		{
			sprintf(fileName, "%s-image%05i.tga", path, iPiece);
			if(Disk::DoesFileExist(fileName) == false)
			{
				break;
			}
			ImageIO::ReadFile(piece.image, fileName);

			sprintf(fileName, "%s-mask%05i.tga", path, iPiece);
			ImageIO::ReadFile(piece.mask, fileName);

			sprintf(fileName, "%s-filter%05i.tga", path, iPiece);
			ImageIO::ReadFile(piece.filteredImage, fileName);
		}

		sprintf(fileName, "%s-dm%05i.map", path, iPiece);
		FlowHelper::ReadFlowField(fileName, piece.disparityMap);

		/*
		int nodeCount = piece.disparityMap.Shape().width * piece.disparityMap.Shape().height;
		uint disparityAddr = 0;
		for(int iNode = 0; iNode < nodeCount; iNode++, disparityAddr += 3)
		{
			if(piece.disparityMap[disparityAddr] == UNDEFINED_DISPARITY)
			{
				piece.mask[iNode] = MASK_INVALID;
			}
			else
			{
				piece.mask[iNode] = MASK_VALID;
			}
		}
		//*/

		sprintf(fileName, "%s-data%05i.txt", path, iPiece);
		if(Disk::DoesFileExist(fileName) == false)
		{
			sprintf(fileName, "%s-data-borrow%05i.txt", path, iPiece);
		}
		RegisteredPieces::ReadRegData(fileName, piece);

		piece.ComputeEdgeImages();
		piece.ComputeColDistSqrImage(targetImage, dataCostWinSize);

		registeredPieces.push_back(piece);
		iPiece++;
	}

	ENSURE(registeredPieces.size() > 0);
	CShape imgShape       = registeredPieces[0].image.Shape();
	CShape maskShape      = registeredPieces[0].mask.Shape();
	CShape disparityShape = registeredPieces[0].disparityMap.Shape();
	ENSURE(imgShape.SameIgnoringNBands(maskShape));
	ENSURE(imgShape.SameIgnoringNBands(disparityShape));
	ENSURE(maskShape.nBands == 1);
	ENSURE(disparityShape.nBands == 3);
	iPiece = 0;
	for(vector<RegisteredPiece>::iterator piece = registeredPieces.begin();
		piece != registeredPieces.end();
		piece++, iPiece++)
	{
		ENSURE(imgShape  == piece->image.Shape());
		ENSURE(imgShape  == piece->filteredImage.Shape());
		ENSURE(maskShape == piece->mask.Shape());
		ENSURE(disparityShape == piece->disparityMap.Shape());
		if(Connectivity == 4)
		{
			ENSURE(maskShape == piece->sobelImageNDE.Shape());
			ENSURE(maskShape == piece->sobelImagePDE.Shape());
		}
		ENSURE(maskShape == piece->sobelImageHE.Shape());
		ENSURE(maskShape == piece->sobelImageVE.Shape());
		ENSURE(maskShape == piece->colDistSqrImg.Shape());
		ENSURE(piece->id >= 0);
	}

	return registeredPieces;
}

void RegisteredPieces::WriteRegisteredPieces(RegisteredPieces &registeredPieces, const char *path, bool writeFast, int iStartPiece, bool borrowTag)
{
	ENSURE((borrowTag == true) || (iStartPiece == 0));
	char fileName[1024];

	int iPiece = iStartPiece;
	for(vector<RegisteredPiece>::iterator piece = registeredPieces.begin();
		piece != registeredPieces.end();
		piece++, iPiece++)
	{
		if(writeFast == true)
		{
			sprintf(fileName, "%s-image%05i.raw", path, iPiece);
			ImageIO::WriteRaw(piece->image, fileName);

			sprintf(fileName, "%s-mask%05i.raw", path, iPiece);
			ImageIO::WriteRaw(piece->mask, fileName);

			sprintf(fileName, "%s-filter%05i.raw", path, iPiece);
			ImageIO::WriteRaw(piece->filteredImage, fileName);
		}
		else
		{
			sprintf(fileName, "%s-image%05i.tga", path, iPiece);
			ImageIO::WriteFile(piece->image, fileName);

			sprintf(fileName, "%s-mask%05i.tga", path, iPiece);
			ImageIO::WriteFile(piece->mask, fileName);

			sprintf(fileName, "%s-filter%05i.tga", path, iPiece);
			ImageIO::WriteFile(piece->filteredImage, fileName);
		}

		sprintf(fileName, "%s-dm%05i.map", path, iPiece);
		FlowHelper::WriteFlowField(fileName, piece->disparityMap);

		if(borrowTag == false)
		{
			sprintf(fileName, "%s-data%05i.txt", path, iPiece);
		}
		else
		{
			sprintf(fileName, "%s-data-borrow%05i.txt", path, iPiece);
		}
		RegisteredPieces::WriteRegData(fileName, *piece);
	}
}

void RegisteredPieces::AppendRegisteredPieces(RegisteredPieces &registeredPieces, const char *path, bool writeFast)
{
	char fileName[1024];

	int iPiece = 0;
	while(true)
	{
		sprintf(fileName, "%s-data%05i.txt", path, iPiece);

		if(Disk::DoesFileExist(fileName) == false) break;

		iPiece++;
	}

	RegisteredPieces::WriteRegisteredPieces(registeredPieces, path, writeFast, iPiece, true);
}

void RegisteredPiece::ComputeEdgeImages()
{
	CShape maskShape = mask.Shape();
	int connectivity = Connectivity;

	sobelImageHE.ReAllocate(maskShape);
	sobelImageVE.ReAllocate(maskShape);
	if(connectivity == 4)
	{
		sobelImagePDE.ReAllocate(maskShape);
		sobelImageNDE.ReAllocate(maskShape);
		ImageProcessing::GetImagesSobel(sobelImageHE, 
										sobelImageVE, 
										sobelImagePDE,
										sobelImageNDE,
										image, 
										mask, true);
	}
	else
	{
		ImageProcessing::GetImagesSobel(sobelImageHE, 
										sobelImageVE, 
										image, 
										mask, true);
	}
}

void RegisteredPiece::ComputeColDistSqrImage(const CFloatImage &targetImg, int windowSize, 
											 const CByteImage &targetMask, bool useTargetMask)
{
	CShape imgShape = this->filteredImage.Shape();
	INSIST(imgShape == targetImg.Shape());

	if(useTargetMask == true)
	{
		INSIST(targetMask.Shape().SameIgnoringNBands(imgShape));
		INSIST(targetMask.Shape().nBands == 1);
	}

	this->colDistSqrImg.ReAllocate(this->mask.Shape());
	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, nodeAddr++, pixelAddr += imgShape.nBands)
	{
        if((this->mask[nodeAddr] == MASK_VALID) &&
		   ((useTargetMask == false) || (targetMask[nodeAddr] == MASK_VALID)))
		{
			this->colDistSqrImg[nodeAddr] = 0.0f;
			for(int iChannel = 0; iChannel < imgShape.nBands; iChannel++)
			{
                float channelDiff = targetImg[pixelAddr + iChannel] - 
									    this->filteredImage[pixelAddr + iChannel];
				this->colDistSqrImg[nodeAddr] += (channelDiff * channelDiff);
			}
		}
		else
		{
			this->colDistSqrImg[nodeAddr] = FLT_MAX;
		}
	}

	if(windowSize == 1) return;

	ENSURE(windowSize % 2 == 1);
	ENSURE(windowSize > 0);
	int halfWidth = windowSize / 2;

	CFloatImage colDistSqrWinImg(this->mask.Shape());
	nodeAddr  = 0;
	pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width; x++, nodeAddr++, pixelAddr += imgShape.nBands)
	{	

        if(this->mask[nodeAddr] == MASK_VALID)
		{
			colDistSqrWinImg[nodeAddr] = 0;

			int windowSupport = 0;
			for(int yShift = -halfWidth; yShift <= halfWidth; yShift++)
			for(int xShift = -halfWidth; xShift <= halfWidth; xShift++)
			{
				int xx = x + xShift;
				int yy = y + yShift;
				if((imgShape.InBounds(xx, yy)) &&
				   (this->mask.Pixel(xx, yy, 0) == MASK_VALID) &&
				   ((useTargetMask == false) || (targetMask.Pixel(xx, yy, 0) == MASK_VALID)))
				{
					colDistSqrWinImg[nodeAddr] += this->colDistSqrImg.Pixel(xx, yy, 0);
					windowSupport++;
				}
			}

			if(windowSupport > 0)
				colDistSqrWinImg[nodeAddr] /= windowSupport;
			else
				colDistSqrWinImg[nodeAddr] = FLT_MAX;
		}
		else
		{
			colDistSqrWinImg[nodeAddr] = FLT_MAX;
		}
	}

	this->colDistSqrImg.ClearPixels();
	nodeAddr  = 0;
	pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, nodeAddr++, pixelAddr += imgShape.nBands)
	{
		this->colDistSqrImg[nodeAddr] = FLT_MAX;
        if(this->mask[nodeAddr] == MASK_VALID)
		{	
			for(int yShift = -halfWidth; yShift <= halfWidth; yShift++)
			for(int xShift = -halfWidth; xShift <= halfWidth; xShift++)
			{
				int xx = x + xShift;
				int yy = y + yShift;
				if((imgShape.InBounds(xx, yy)) &&
				   (this->mask.Pixel(xx, yy, 0) == MASK_VALID))
				{
					this->colDistSqrImg[nodeAddr] = std::min<float>(this->colDistSqrImg[nodeAddr], 
																	colDistSqrWinImg.Pixel(xx, yy, 0));
				}
			}
		}
	}
}

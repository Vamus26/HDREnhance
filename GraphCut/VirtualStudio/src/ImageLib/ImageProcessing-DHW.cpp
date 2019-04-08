#include "ImageProcessing.h"

CIntImage ImageProcessing::MatchImageHist(const CIntImage &targetImg,
										  const CIntImage &srcImg,
										  int minChannelVal, int maxChannelVal,
										  const CByteImage &targetMask, bool useTargetMask,
										  const CByteImage &srcMask, bool useSrcMask)
{
	CShape targetShape = targetImg.Shape();
	INSIST(targetShape.nBands == 1);
	CShape srcShape = srcImg.Shape();	
	INSIST(srcShape.nBands == 1);

	if(useTargetMask)
	{
		INSIST(targetMask.Shape() == targetShape);
	}
	if(useSrcMask)
	{
		INSIST(srcMask.Shape() == srcShape);
	}

	INSIST(maxChannelVal > minChannelVal);
	int histRange     = maxChannelVal - minChannelVal + 1;
	float *srcHist    = ImageProcessing::GetNormalizedImageHist(srcImg,    minChannelVal, maxChannelVal, srcMask,    useSrcMask);
	float *targetHist = ImageProcessing::GetNormalizedImageHist(targetImg, minChannelVal, maxChannelVal, targetMask, useTargetMask); 	

	float *srcCulmHist    = ImageProcessing::GetCumlImageHist(srcHist,    histRange);
	float *targetCulmHist = ImageProcessing::GetCumlImageHist(targetHist, histRange);


	int *histMapping = ImageProcessing::GetHistMappingUsingDHW(targetCulmHist, srcCulmHist, histRange, histRange);

	CIntImage warpedImg(targetShape);
	int nodeCount = targetShape.width * targetShape.height * targetShape.nBands;
	//for(int iNode = 0; iNode < nodeCount; i


	delete [] targetHist;
	delete [] srcHist;
	delete [] targetCulmHist;
	delete [] srcCulmHist;

	return CIntImage();
}


int* ImageProcessing::GetImageHist(const CIntImage &img,
								   int minChannelVal, int maxChannelVal,
								   const CByteImage &imgMask, bool useImgMask)
{
	CShape imgShape = img.Shape();
	INSIST(imgShape.nBands == 1);
	if(useImgMask)
	{
		INSIST(imgMask.Shape() == imgShape);
	}

	INSIST(maxChannelVal > minChannelVal);
	int histRange = maxChannelVal - minChannelVal + 1;

	int *imgHist = new int[histRange]; 
	INSIST(imgHist != NULL);
	memset(imgHist, 0, sizeof(*imgHist) * histRange);

	int nodeCount  = 0;
	uint nodeAddr  = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, nodeAddr++)
	{
		if((useImgMask) && (imgMask[nodeAddr] == MASK_INVALID)) continue;

		INSIST_RANGE(img[nodeAddr], minChannelVal, maxChannelVal);
		int histIndex = img[nodeAddr] - minChannelVal;
		imgHist[histIndex]++;
		nodeCount++;
	}
	INSIST(nodeCount > 0);

	return imgHist;
}


float* ImageProcessing::GetNormalizedImageHist(const CIntImage &img,
											   int minChannelVal, int maxChannelVal,
											   const CByteImage &imgMask, bool useImgMask)
{
	CShape imgShape = img.Shape();
	INSIST(imgShape.nBands == 1);
	if(useImgMask)
	{
		INSIST(imgMask.Shape() == imgShape);
	}

	INSIST(maxChannelVal > minChannelVal);
	int histRange = maxChannelVal - minChannelVal + 1;

	float *imgHist = new float[histRange]; 
	INSIST(imgHist != NULL);
	memset(imgHist, 0, sizeof(*imgHist) * histRange);

	int nodeCount  = 0;
	uint nodeAddr  = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, nodeAddr++)
	{
		if((useImgMask) && (imgMask[nodeAddr] == MASK_INVALID)) continue;

		INSIST_RANGE(img[nodeAddr], minChannelVal, maxChannelVal);
		int histIndex = img[nodeAddr] - minChannelVal;
		imgHist[histIndex]++;
		nodeCount++;
	}
	INSIST(nodeCount > 0);

	for(int iHistElem = 0; iHistElem < histRange; iHistElem++)
	{
		imgHist[iHistElem] /= nodeCount;
	}

	return imgHist;
}


float* ImageProcessing::GetCumlImageHist(const float *imgHist, int histRange)
{
	INSIST(histRange > 0);

	float *cumlImgHist = new float[histRange]; 
	INSIST(cumlImgHist != NULL);

	for(int iHistElem = 0; iHistElem < histRange; iHistElem++)
	{
		cumlImgHist[iHistElem] = imgHist[iHistElem];
		if(iHistElem > 0)
		{
			cumlImgHist[iHistElem] += cumlImgHist[iHistElem - 1];
		}
	}

	INSIST(fabs(cumlImgHist[histRange - 1] - 1.0f) < 0.00001f);

	return cumlImgHist;
}

int* ImageProcessing::GetHistMappingUsingDHW(float const *targetCulmHist, 
 											 float const *srcCulmHist,
											 int targetHistRange,
											 int srcHistRange,
											 int maxTargetHistCompression,
											 int maxSrcHistCompression)
{
	INSIST(targetHistRange > 0);
	INSIST(srcHistRange    > 0);

	float **costTable = new float*[targetHistRange];
	int **mapSrc      = new int*[targetHistRange];
	int **mapTarget   = new int*[targetHistRange];
	INSIST(costTable != NULL);
	INSIST(mapSrc    != NULL);
	INSIST(mapTarget != NULL);
	for(int iTargetHist = 0; iTargetHist < targetHistRange; iTargetHist++)
	{
		costTable[iTargetHist] = new float[srcHistRange];
		mapSrc[iTargetHist]    = new int[srcHistRange];
		mapTarget[iTargetHist] = new int[srcHistRange];
		INSIST(costTable[iTargetHist] != NULL);
		INSIST(mapSrc[iTargetHist]    != NULL);
		INSIST(mapTarget[iTargetHist] != NULL);
	}
	
	for(int iTargetHist = 0; iTargetHist < targetHistRange; iTargetHist++)
	for(int iSrcHist    = 0; iSrcHist    < srcHistRange;    iSrcHist++)
	{
		float tableElemCost    = FLT_MAX;
		int tableElemMapSrc    = INT_MAX;
		int tableElemMapTarget = INT_MAX;

		for(int iCompression = 0; iCompression <= 1; iCompression++)
		{		
			int maxHistCompression;
			if(iCompression == 0)
				maxHistCompression = maxSrcHistCompression;
			else
				maxHistCompression = maxTargetHistCompression;

			for(int iHistCompression = 1; iHistCompression <= maxHistCompression; iHistCompression++)
			{
				int jSrcHist, jTargetHist;

				if(iCompression == 0)
				{
				    jSrcHist    = iSrcHist - iHistCompression;
					jTargetHist = iTargetHist - 1;
				}
				else
				{
					jSrcHist    = iSrcHist - 1;
					jTargetHist = iTargetHist - iHistCompression;
				}

				float currCost;

				if((jSrcHist == -1) && (jTargetHist == -1))
					currCost = fabs(targetCulmHist[iTargetHist] - srcCulmHist[iSrcHist]);
				else if ((jSrcHist < 0) || (jTargetHist < 0))
					continue;					
				else
				{
					currCost = costTable[jTargetHist][jSrcHist] + 
					           fabs((targetCulmHist[iTargetHist] - targetCulmHist[jTargetHist]) -
									(srcCulmHist[iSrcHist]       - srcCulmHist[jSrcHist]));
				}

				if(currCost < tableElemCost)
				{
					tableElemCost      = currCost;
					tableElemMapSrc    = jSrcHist;
					tableElemMapTarget = jTargetHist;
				}
			}
		}
		INSIST(tableElemCost != FLT_MAX);

		costTable[iTargetHist][iSrcHist] = tableElemCost;
		mapSrc[iTargetHist][iSrcHist]    = tableElemMapSrc;
		mapTarget[iTargetHist][iSrcHist] = tableElemMapTarget;
	}

	int* histMapping = new int[targetHistRange];
	INSIST(histMapping != NULL);

	int srcHistI    = srcHistRange    - 1;
	int targetHistI = targetHistRange - 1;
	while((srcHistI >= 0) && (targetHistI >= 0))
	{
		PRINT_DOUBLE(costTable[targetHistI][srcHistI]);
		PRINT_INT(targetHistI);
		PRINT_INT(srcHistI);

		int mapSrcHistI    = mapSrc[targetHistI][srcHistI];
		int mapTargetHistI = mapTarget[targetHistI][srcHistI];

		ENSURE(mapSrcHistI    < srcHistI);
		ENSURE(mapTargetHistI < targetHistI);

		int targetHistJump = targetHistI - mapTargetHistI;
		int srcHistJump    = srcHistI    - mapSrcHistI;
		ENSURE((targetHistJump == 1) || (srcHistJump == 1));

		if(srcHistJump > 1)
		{
			histMapping[targetHistI] = srcHistI - (srcHistJump / 2);
		}
		else
		{
			for(int iTargetHist = mapTargetHistI + 1; iTargetHist <= targetHistI; iTargetHist++)
			{
				histMapping[iTargetHist] = srcHistI;
			}
		}

		srcHistI = mapSrcHistI;
		targetHistI = mapTargetHistI;
	}
	INSIST((srcHistI == -1) && (targetHistI == -1));

	for(int iTargetHist = 0; iTargetHist < targetHistRange; iTargetHist++)
	{
		delete [] costTable[iTargetHist];
		delete [] mapSrc[iTargetHist];
		delete [] mapTarget[iTargetHist];
	}
	delete [] costTable;
	delete [] mapSrc;
	delete [] mapTarget;

	return histMapping;
}

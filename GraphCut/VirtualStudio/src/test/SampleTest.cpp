#include "SampleTest.h"
#include "ImageProcessing.h"
#include "ColorHelper.h"
#include <algorithm>


SampleTest::SampleTest(CFloatImage hiRes1, CFloatImage hiRes2)
{
	ENSURE(hiRes1.Shape() == hiRes2.Shape());

	this->hiRes1 = hiRes1;
	this->hiRes2 = hiRes2;

	ImageProcessing::ScaleImage(this->lowRes1, this->hiRes1, 0.5f);
	ImageProcessing::ScaleImage(this->lowRes2, this->hiRes2, 0.5f);


	//CShape hiResShape = this->hiRes2.Shape();
	//for(int y = 0; y < hiResShape.height; y++)
	//for(int x = 0; x < hiResShape.width;  x++)
	//for(int channel = 0; channel < hiResShape.nBands;  channel++)
	//{
	//	float col = ((x / 20) % 2) != ((y / 20) % 2) ? 1.0f : 0.0f;
	//	this->hiRes2.Pixel(x, y, channel) = col;
	//}

	//this->lowRes1 = ColorHelper::RGBtoLuminance(this->lowRes1);
	//this->lowRes2 = ColorHelper::RGBtoLuminance(this->lowRes2);
}

void SampleTest::Resample(int searchWinSize)
{
	ENSURE(searchWinSize > 0);

	CShape probImgSize((searchWinSize * 2) + 1, (searchWinSize * 2) + 1, this->lowRes1.Shape().nBands);
	this->currPixelSampleProb.ReAllocate(probImgSize);

	CShape lowResShape = this->lowRes1.Shape();

	//int x = 66;
	//int y = 46;
	//y = lowResShape.height - y - 1;
	//
	//computePixelSampleProbImg(x, y, searchWinSize);	
	//ImageIO::WriteFile(this->currPixelSampleProb, "1.tga");	
	//CFloatImage hiResSampleProbImg((searchWinSize * 4) + 1, (searchWinSize * 4) + 1, 3);
	//scaleProbImg(this->currPixelSampleProb, hiResSampleProbImg);
	//ImageIO::WriteFile(hiResSampleProbImg, "2.tga");
	//normalizePixelProbImg(hiResSampleProbImg);
	//ImageIO::WriteFile(hiResSampleProbImg, "3.tga");
	//exit(0);

	CFloatImage hiResSampleProb((searchWinSize * 4) + 1, (searchWinSize * 4) + 1, this->lowRes1.Shape().nBands);

	CFloatImage newHiRes1(this->hiRes1.Shape());
	newHiRes1.ClearPixels();

	CFloatImage newLowRes1(this->lowRes1.Shape());
	newLowRes1.ClearPixels();

	for(float y = 0; y < lowResShape.height - 0.5f; y += 0.5f)	
	{
		for(float x = 0; x < lowResShape.width - 0.5f; x += 0.5f)
		{	
			computePixelSampleProbImg(x, y, (float) searchWinSize);	
			//discardLowProbs(this->currPixelSampleProb, 1);
			normalizePixelProbImg(this->currPixelSampleProb);
		
			if((x - ((int) x) == 0.0f) &&
			   (y - ((int) y) == 0.0f))
			{
				resample(newLowRes1, this->lowRes2, (int) x, (int) y, this->currPixelSampleProb, searchWinSize, 1.0f);
			}

			scaleProbImg(this->currPixelSampleProb, hiResSampleProb);

			resample(newHiRes1, this->hiRes2, 
				     (int) (2 * x), (int ) (2 * y), 
					 hiResSampleProb, searchWinSize * 2, 1.0f);
		}
		printf(".");
	}
	printf("\n");

	ImageIO::WriteFile(this->hiRes1, "hiRes1-groundTruth.tga");
	ImageIO::WriteFile(newHiRes1, "newHiRes1.tga");

	ImageIO::WriteFile(this->lowRes1, "lowRes1-groundTruth.tga");
	ImageIO::WriteFile(newLowRes1, "newLowRes1.tga");
}

void SampleTest::computePixelSampleProbImg(float x, float y, float searchWinSize)
{
	CShape lowResShape = this->lowRes1.Shape();


	uint nodeAddr = 0;
	for(float yOffset = -searchWinSize; yOffset <= searchWinSize; yOffset += 1.0f)
	for(float xOffset = -searchWinSize; xOffset <= searchWinSize; xOffset += 1.0f)
	{
		float xx = x + xOffset;
		float yy = y + yOffset;

		for(int channel = 0; channel < lowResShape.nBands; channel++, nodeAddr++)
		{

			if(lowResShape.InBounds(xx, yy))
			{
				this->currPixelSampleProb[nodeAddr] = computePixelSampleProbImg(x, y, xx, yy, channel);
			}
			else
			{
				this->currPixelSampleProb[nodeAddr] = 0.0f;
			}
		}
	}
}

float SampleTest::computePixelSampleProbImg(float x, float y, float xx, float yy, int channel)
{
	float supportSize = 1.0f;

	CShape lowResShape = this->lowRes1.Shape();

	float spaceVar = -2.0f * 0.0025f;
	float sigVar   = -2.0f * 0.0000075f;
	float noiseVar = -2.0f * 0.00075f;

	float matchProb = 0.0f;
	float matchProbWeigh = 0.0f;

	float centerSrcPixelVal = ImageProcessing::MitchellInterlopation(this->lowRes1, x, y, channel);

	for(float yOffset = -supportSize; yOffset <= supportSize; yOffset++)
	for(float xOffset = -supportSize; xOffset <= supportSize; xOffset++)
	{
		float srcX = x + xOffset;
		float srcY = y + yOffset;
		if(lowResShape.InBounds(srcX, srcY))
		{
			float dstX = xx + xOffset;
			float dstY = yy + yOffset;
			if(lowResShape.InBounds(dstX, dstY))
			{
				float srcPixelVal    = ImageProcessing::MitchellInterlopation(this->lowRes1, srcX, srcY, channel);
				float srcPixelDist   = sqrt((float) (xOffset * xOffset) + (yOffset * yOffset));
				float sigDiff        = srcPixelVal - centerSrcPixelVal;				
				float srcPixelWeight = exp(srcPixelDist / spaceVar) * exp(sigDiff * sigDiff / sigVar);

				float dstPixelVal = ImageProcessing::MitchellInterlopation(this->lowRes2, dstX, dstY, channel);

				sigDiff = srcPixelVal - dstPixelVal;
				matchProb += exp(sigDiff * sigDiff / noiseVar) * srcPixelWeight;
				matchProbWeigh += srcPixelWeight;								
			}
		}
	}

	ENSURE(matchProbWeigh != 0.0f);
	matchProb /= matchProbWeigh;

	return matchProb;
}

void SampleTest::discardLowProbs(CFloatImage &probImgOrg, int probsToSave)
{
	CFloatImage probImg = probImgOrg.Clone();
	CShape pixelProbImgShape = probImg.Shape();	
	probImgOrg.ClearPixels();

	for(int channel = 0; channel < pixelProbImgShape.nBands; channel++)
	{
		
		vector< std::pair<float, std::pair<int,int> > > probCoords;

		uint nodeAddr = 0;
		for(int y = 0; y < pixelProbImgShape.height; y++)
		for(int x = 0; x < pixelProbImgShape.width; x++, nodeAddr += pixelProbImgShape.nBands)
		{
			probCoords.push_back(std::make_pair(probImg[nodeAddr + channel], 
												std::make_pair(x, y)));
		}	

		std::sort(probCoords.begin(), probCoords.end());

		ENSURE(probsToSave <= (int) probCoords.size());
		
		for(uint iProb = (uint) probCoords.size() - 1; 
			iProb > probCoords.size() - 1 - probsToSave; iProb--)
		{
			int probX = probCoords[iProb].second.first;
			int probY = probCoords[iProb].second.second;
			float prob = probCoords[iProb].first;

			probImgOrg.Pixel(probX, probY, channel) = prob;
		}
	}

	normalizePixelProbImg(probImgOrg);
}

void SampleTest::normalizePixelProbImg(CFloatImage &probImg)
{
	CShape pixelProbImgShape = probImg.Shape();
	//ENSURE(pixelProbImgShape.nBands == 1);

	float probSum[3] = {0.0f};
	int elemCount = pixelProbImgShape.width * pixelProbImgShape.height * pixelProbImgShape.nBands;
	for(int iElem = 0; iElem < elemCount; iElem += pixelProbImgShape.nBands)
	{
		for(int channel = 0; channel < pixelProbImgShape.nBands; channel++)
		{
			probSum[channel] += probImg[iElem + channel];
		}
	}

	for(int channel = 0; channel < pixelProbImgShape.nBands; channel++)
	{
		ENSURE(probSum[channel] != 0.0f);
	}

	for(int iElem = 0; iElem < elemCount; iElem += pixelProbImgShape.nBands)
	{
		for(int channel = 0; channel < pixelProbImgShape.nBands; channel++)
		{
			probImg[iElem + channel] /= probSum[channel];
		}
	}
}

void SampleTest::resample(CFloatImage &dstImg, const CFloatImage &srcImg, int dstX, int dstY, 
						  CFloatImage &sampleProbImg, int searchWinSize, float scale)
{
	CShape dstImgShape = dstImg.Shape();
	if(dstImgShape.InBounds(dstX, dstY) == false)
		return;

	for(int channel = 0; channel < dstImgShape.nBands; channel++)
	{
		dstImg.Pixel(dstX, dstY, channel) = 0.0f;
	}

	float sampleWeight[3] = {0.0f};
	uint sampleNodeAddr = 0;
	for(int yOffset = -searchWinSize; yOffset <= searchWinSize; yOffset++)
	for(int xOffset = -searchWinSize; xOffset <= searchWinSize; xOffset++, sampleNodeAddr += dstImgShape.nBands)
	{
		float srcX = dstX + (xOffset * scale);
		float srcY = dstY + (yOffset * scale);
		if(dstImgShape.InBounds(srcX, srcY))
		{
			for(int channel = 0; channel < dstImgShape.nBands; channel++)
			{
				dstImg.Pixel(dstX, dstY, channel) += sampleProbImg[sampleNodeAddr + channel] * 
					ImageProcessing::BilinearIterpolation(srcImg, srcX, srcY, channel);

				sampleWeight[channel] += sampleProbImg[sampleNodeAddr + channel];
			}			
		}		
	}

	//for(int channel = 0; channel < dstImgShape.nBands; channel++)
	//{	
	//	ENSURE(sampleWeight[channel] > 0.0f);
	//}

	for(int channel = 0; channel < dstImgShape.nBands; channel++)
	{
		if(sampleWeight[channel] > 0.0f)
			dstImg.Pixel(dstX, dstY, channel) /= sampleWeight[channel];
	}
}


void SampleTest::fillInGaps(CFloatImage &dstImg)
{
	CShape dstImgShape = dstImg.Shape();

	for(int y = 0; y < dstImgShape.height; y += 2)
	for(int x = 1; x < dstImgShape.width; x += 2)
	{
		for(int channel = 0; channel < dstImgShape.nBands; channel++)
		{
			dstImg.Pixel(x, y, channel) = dstImg.Pixel(x - 1, y, channel);
			if(x < (dstImgShape.width - 1))
			{
				dstImg.Pixel(x, y, channel) += dstImg.Pixel(x + 1, y, channel);
				dstImg.Pixel(x, y, channel) /= 2.0f;
			}
		}
	}


	for(int y = 1; y < dstImgShape.height; y += 2)
	for(int x = 0; x < dstImgShape.width; x++)
	{
		for(int channel = 0; channel < dstImgShape.nBands; channel++)
		{
			dstImg.Pixel(x, y, channel) = dstImg.Pixel(x, y - 1, channel);
			if(y < (dstImgShape.height - 1))
			{
				dstImg.Pixel(x, y, channel) += dstImg.Pixel(x, y + 1, channel);
				dstImg.Pixel(x, y, channel) /= 2.0f;
			}
		}
	}
}

void SampleTest::scaleProbImg(CFloatImage &srcProbImg, CFloatImage &dstProbImg)
{
	CShape dstShape = dstProbImg.Shape();
	CShape srcShape = srcProbImg.Shape();

	ENSURE(dstShape.width == srcShape.width * 2 - 1);
	ENSURE(dstShape.height == srcShape.height * 2 - 1);

	dstProbImg.ClearPixels();

	for(int y = 0; y < dstShape.height; y += 2)
	{
		for(int x = 0; x < dstShape.width; x += 2)
		{
			for(int channel = 0; channel < dstShape.nBands; channel++)
			{
				dstProbImg.Pixel(x, y, channel) = srcProbImg.Pixel(x / 2, y / 2, channel);
			}
		}
	}

}




/*
SampleTest::SampleTest(CFloatImage hiRes1, CFloatImage hiRes2)
{
	ENSURE(hiRes1.Shape() == hiRes2.Shape());

	this->hiRes1 = hiRes1;
	this->hiRes2 = hiRes2;

	ImageProcessing::ScaleImage(this->lowRes1, this->hiRes1, 0.5f);
	ImageProcessing::ScaleImage(this->lowRes2, this->hiRes2, 0.5f);

	this->lowRes1 = ColorHelper::RGBtoLuminance(this->lowRes1);
	this->lowRes2 = ColorHelper::RGBtoLuminance(this->lowRes2);
}

void SampleTest::Resample(int searchWinSize)
{
	ENSURE(searchWinSize > 0);

	CShape probImgSize((searchWinSize * 2) + 1, (searchWinSize * 2) + 1, 1);
	this->currPixelSampleProb.ReAllocate(probImgSize);

	CShape lowResShape = this->lowRes1.Shape();


	//int x = 66;
	//int y = 46;
	//y = lowResShape.height - y - 1;
	//
	//computePixelSampleProbImg(x, y, searchWinSize);	
	//ImageIO::WriteFile(this->currPixelSampleProb, "1.tga");	
	//CFloatImage hiResSampleProbImg((searchWinSize * 4) + 1, (searchWinSize * 4) + 1, 1);
	//scaleProbImg(this->currPixelSampleProb, hiResSampleProbImg);
	//ImageIO::WriteFile(hiResSampleProbImg, "2.tga");
	//discardLowProbs(hiResSampleProbImg, 2);
	//ImageIO::WriteFile(hiResSampleProbImg, "3.tga");
	//exit(0);


	CFloatImage newHiRes1(this->hiRes1.Shape());
	newHiRes1.ClearPixels();

	CFloatImage newLowRes1(this->lowRes1.Shape());
	newLowRes1.ClearPixels();

	CFloatImage hiResSampleProb((searchWinSize * 4) + 1, (searchWinSize * 4) + 1, 1);

	for(float y = 0; y < lowResShape.height - 0.5f; y += 0.5f)
	{
		for(float x = 0; x < lowResShape.width - 0.5f; x += 0.5f)
		{	
			computePixelSampleProbImg(x, y, (float) searchWinSize);	
			//discardLowProbs(this->currPixelSampleProb, 10);
			normalizePixelProbImg(this->currPixelSampleProb);
		
			if((x - ((int) x) == 0.0f) &&
			   (y - ((int) y) == 0.0f))
			{
				resample(newLowRes1, this->lowRes2, (int) x, (int) y, this->currPixelSampleProb, searchWinSize);
			}

			//ImageProcessing::ScaleImage(hiResSampleProb, this->currPixelSampleProb, 2.0, false);			

			scaleProbImg(this->currPixelSampleProb, hiResSampleProb);

			resample(newHiRes1, this->hiRes2, 
				     (int) (2 * x), (int ) (2 * y), 
					 hiResSampleProb, searchWinSize * 2);
		}
		printf(".");
	}
	printf("\n");

	//fillInGaps(newHiRes1);
	ImageIO::WriteFile(this->hiRes1, "hiRes1-groundTruth.tga");
	ImageIO::WriteFile(newHiRes1, "newHiRes1.tga");

	ImageIO::WriteFile(this->lowRes1, "lowRes1-groundTruth.tga");
	ImageIO::WriteFile(newLowRes1, "newLowRes1.tga");
}

void SampleTest::computePixelSampleProbImg(float x, float y, float searchWinSize)
{
	CShape lowResShape = this->lowRes1.Shape();

	uint nodeAddr = 0;
	for(float yOffset = -searchWinSize; yOffset <= searchWinSize; yOffset++)
	for(float xOffset = -searchWinSize; xOffset <= searchWinSize; xOffset++, nodeAddr++)
	{
		float xx = x + xOffset;
		float yy = y + yOffset;

		if(lowResShape.InBounds(xx, yy))
		{
			this->currPixelSampleProb[nodeAddr] = computePixelSampleProbImg(x, y, xx, yy);
		}
		else
		{
			this->currPixelSampleProb[nodeAddr] = 0.0f;
		}
	}
}

float SampleTest::computePixelSampleProbImg(float x, float y, float xx, float yy)
{
	float supportSize = 1.0f;

	CShape lowResShape = this->lowRes1.Shape();

	float spaceVar = -2.0f * 0.0025f;
	float sigVar   = -2.0f * 0.0000075f;
	float noiseVar = -2.0f * 0.00075f;

	float matchProb = 0.0f;
	float matchProbWeigh = 0.0f;

	float centerSrcPixelVal = ImageProcessing::MitchellInterlopation(this->lowRes1, x, y, 0);

	for(float yOffset = -supportSize; yOffset <= supportSize; yOffset++)
	for(float xOffset = -supportSize; xOffset <= supportSize; xOffset++)
	{
		float srcX = x + xOffset;
		float srcY = y + yOffset;
		if(lowResShape.InBounds(srcX, srcY))
		{
			float dstX = xx + xOffset;
			float dstY = yy + yOffset;
			if(lowResShape.InBounds(dstX, dstY))
			{
				float srcPixelVal    = ImageProcessing::MitchellInterlopation(this->lowRes1, srcX, srcY, 0);
				float srcPixelDist   = sqrt((float) (xOffset * xOffset) + (yOffset * yOffset));
				float sigDiff        = srcPixelVal - centerSrcPixelVal;				
				float srcPixelWeight = exp(srcPixelDist / spaceVar) * exp(sigDiff * sigDiff / sigVar);

				float dstPixelVal = ImageProcessing::MitchellInterlopation(this->lowRes2, dstX, dstY, 0);

				sigDiff = srcPixelVal - dstPixelVal;
				matchProb += exp(sigDiff * sigDiff / noiseVar) * srcPixelWeight;
				matchProbWeigh += srcPixelWeight;								
			}
		}
	}

	ENSURE(matchProbWeigh != 0.0f);
	matchProb /= matchProbWeigh;

	return matchProb;
}

void SampleTest::discardLowProbs(CFloatImage &probImg, int probsToSave)
{
	CShape pixelProbImgShape = probImg.Shape();
	
	vector< std::pair<float, std::pair<int,int> > > probCoords;

	uint nodeAddr = 0;
	for(int y = 0; y < pixelProbImgShape.height; y++)
	for(int x = 0; x < pixelProbImgShape.width; x++, nodeAddr++)
	{
		probCoords.push_back(std::make_pair(probImg[nodeAddr], 
			                                std::make_pair(x, y)));
	}	

	std::sort(probCoords.begin(), probCoords.end());

	ENSURE(probsToSave <= (int) probCoords.size());
	probImg.ClearPixels();
	for(uint iProb = (uint) probCoords.size() - 1; 
		iProb > probCoords.size() - 1 - probsToSave; iProb--)
	{
		int probX = probCoords[iProb].second.first;
		int probY = probCoords[iProb].second.second;
		float prob = probCoords[iProb].first;

		probImg.Pixel(probX, probY, 0) = prob;
	}		

	normalizePixelProbImg(probImg);
}

void SampleTest::normalizePixelProbImg(CFloatImage &probImg)
{
	CShape pixelProbImgShape = probImg.Shape();
	ENSURE(pixelProbImgShape.nBands == 1);

	float probSum = 0.0f;
	int elemCount = pixelProbImgShape.width * pixelProbImgShape.height;
	for(int iElem = 0; iElem < elemCount; iElem++)
	{
		probSum += probImg[iElem];
	}

	ENSURE(probSum != 0.0f);

	for(int iElem = 0; iElem < elemCount; iElem++)
	{
		probImg[iElem] /= probSum;
	}
}

void SampleTest::resample(CFloatImage &dstImg, const CFloatImage &srcImg, int dstX, int dstY, 
						  CFloatImage &sampleProbImg, int searchWinSize)
{
	CShape dstImgShape = dstImg.Shape();
	if(dstImgShape.InBounds(dstX, dstY) == false)
		return;

	for(int channel = 0; channel < dstImgShape.nBands; channel++)
	{
		dstImg.Pixel(dstX, dstY, channel) = 0.0f;
	}

	CShape sampleProbImgShape = sampleProbImg.Shape();
	float sampleWeight = 0.0f;
	uint sampleNodeAddr = 0;
	for(int yOffset = -searchWinSize; yOffset <= searchWinSize; yOffset++)
	for(int xOffset = -searchWinSize; xOffset <= searchWinSize; xOffset++, sampleNodeAddr++)
	{
		int srcX = dstX + xOffset;
		int srcY = dstY + yOffset;
		if(dstImgShape.InBounds(srcX, srcY))
		{
			for(int channel = 0; channel < dstImgShape.nBands; channel++)
			{
				dstImg.Pixel(dstX, dstY, channel) += sampleProbImg[sampleNodeAddr] * srcImg.Pixel(srcX, srcY, channel);				
			}
			sampleWeight += sampleProbImg[sampleNodeAddr];			
		}		
	}

	ENSURE(sampleWeight > 0.0f);

	if(sampleWeight > 0.0f)
	{
		for(int channel = 0; channel < dstImgShape.nBands; channel++)
		{
			dstImg.Pixel(dstX, dstY, channel) /= sampleWeight;
		}
	}
}


void SampleTest::fillInGaps(CFloatImage &dstImg)
{
	CShape dstImgShape = dstImg.Shape();

	for(int y = 0; y < dstImgShape.height; y += 2)
	for(int x = 1; x < dstImgShape.width; x += 2)
	{
		for(int channel = 0; channel < dstImgShape.nBands; channel++)
		{
			dstImg.Pixel(x, y, channel) = dstImg.Pixel(x - 1, y, channel);
			if(x < (dstImgShape.width - 1))
			{
				dstImg.Pixel(x, y, channel) += dstImg.Pixel(x + 1, y, channel);
				dstImg.Pixel(x, y, channel) /= 2.0f;
			}
		}
	}


	for(int y = 1; y < dstImgShape.height; y += 2)
	for(int x = 0; x < dstImgShape.width; x++)
	{
		for(int channel = 0; channel < dstImgShape.nBands; channel++)
		{
			dstImg.Pixel(x, y, channel) = dstImg.Pixel(x, y - 1, channel);
			if(y < (dstImgShape.height - 1))
			{
				dstImg.Pixel(x, y, channel) += dstImg.Pixel(x, y + 1, channel);
				dstImg.Pixel(x, y, channel) /= 2.0f;
			}
		}
	}
}

void SampleTest::scaleProbImg(CFloatImage &srcProbImg, CFloatImage &dstProbImg)
{
	CShape dstShape = dstProbImg.Shape();
	CShape srcShape = srcProbImg.Shape();

	ENSURE(dstShape.width == srcShape.width * 2 - 1);
	ENSURE(dstShape.height == srcShape.height * 2 - 1);

	dstProbImg.ClearPixels();

	for(int y = 0; y < dstShape.height; y += 2)
	{
		for(int x = 0; x < dstShape.width; x += 2)
		{
			dstProbImg.Pixel(x, y, 0) = srcProbImg.Pixel(x / 2, y / 2, 0);
		}
	}
}
*/
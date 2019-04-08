#include "Segmenter.h"
#include "msImageProcessor.h"
#include "ImageProcessing.h"
#include "ImageIO.h"
#include "Disk.h"

void Segmenter::CreateSegmentation(const Camera &camera, int iFrame, 
								   Segmentation &segmentation, SegmenterParams sParams)
{
	this->params = sParams;
	ENSURE(segmentation.segments.size() == 0);

	printf("Segmenting - %s, frame-%05i\n", camera.id.c_str(), iFrame);
	string frameName = camera.GetFrameName(iFrame);
	CByteImage imgToSeg;
	ImageIO::ReadFile(imgToSeg, frameName);

	createSegmentation(imgToSeg, segmentation);
		
	segmentation.Save(camera, iFrame);

	segmentation.fullyComputed = false;

	segmentation.CheckSegmentationConsistency(this->params.segBreakerParams.minSegSize,
											  this->params.segBreakerParams.maxSegSize);
}

void Segmenter::createSegmentation(const CByteImage &img, Segmentation &segmentation)
{	
	printf("\tCreating initial segmentation\n");
	int segCount = createLabelData(img, segmentation);
	ENSURE(segCount > 0);
	ENSURE(img.Shape().SameIgnoringNBands(segmentation.labelImg.Shape()));

	segmentation.CreateSegments(segCount);
	ENSURE((int) segmentation.segments.size() == segCount);

	printf("\tBreaking large segments\n");
	breakLargeSegments(segmentation);	
}

int Segmenter::createLabelData(const CByteImage &img, Segmentation &segmentation)
{	
	CByteImage smoothImg = img.Clone();	
	for(int i = 0; i < this->params.smoothingLevel; i++)
	{
		ImageProcessing::DirectionalSmoothing(smoothImg);
	}

	//check
	segmentation.smoothImg = ImageProcessing::ByteToFloatImg(smoothImg);
	//segmentation.smoothImg = ImageProcessing::ByteToFloatImg(img);
	//segmentation.smoothImg = ImageProcessing::BilateralFilter(segmentation.smoothImg, 
	//							this->params.smoothImgSpaceD, 
	//							this->params.smoothImgSigD, 
	//							this->params.smoothImgKernelHW);
	//CByteImage smoothImg = ImageProcessing::FloatToByteImg(segmentation.smoothImg);

	byte *imgData = (byte *) smoothImg.PixelAddress(0,0,0);

	CShape imgShape = segmentation.smoothImg.Shape();
	ENSURE((imgShape.nBands == 1) || (imgShape.nBands == 3));
	imageType imgType = (imgShape.nBands == 1) ? GRAYSCALE : COLOR;
	
	msImageProcessor *imgProcessor = new msImageProcessor();

	imgProcessor->DefineImage(imgData, imgType, 
							  imgShape.height, imgShape.width);

	//CFloatImage edgeMap(imgShape.width, imgShape.height, 1);
	//edgeMap.ClearPixels();
	//for(int y = 0; y < imgShape.height; y++)
	//{
	//	for(int x = 0; x < imgShape.width; x++)
	//	{
	//		if(((x % 100) == 0) ||
	//		   ((y % 100) == 0))
	//		{
	//			edgeMap.Pixel(x, y, 0) = 1.0f;
	//		}
	//	}
	//}
	//ImageIO::WriteFile(edgeMap, "2.tga");


	//imgProcessor->SetWeightMap((float *) edgeMap.PixelAddress(0,0,0), 1.0f);


	////check - use edge weight map
	imgProcessor->Segment(this->params.spatialRadius,
		                  this->params.colorRadius,
						  this->params.segBreakerParams.minSegSize,
						  this->params.speedUp);
		
	CShape labelImgShape(imgShape.width, imgShape.height, 1);
	segmentation.labelImg.ReAllocate(labelImgShape);
	int *labelImgData = (int *) segmentation.labelImg.PixelAddress(0, 0, 0);
	int segCount = imgProcessor->GetLabelImg(labelImgData);
	ENSURE(segCount > 0);

	delete imgProcessor;

	return segCount;	
}

void Segmenter::breakLargeSegments(Segmentation &segmentation)
{
	vector<Segment> &segments = segmentation.segments;
	ENSURE(segments.size() > 0);

	uint segCount = (uint) segments.size();
	for(uint iSeg = 0; iSeg < segCount; iSeg++)
	{
		bool breakSeq = false;

		if(segments[iSeg].size > this->params.segBreakerParams.maxSegSize)
			breakSeq = true;
		else if(segments[iSeg].size > 2 * this->params.segBreakerParams.minSegSize)
		{
			int xSpan = segments[iSeg].bounds.maxX - segments[iSeg].bounds.minX + 1;
			int ySpan = segments[iSeg].bounds.maxY - segments[iSeg].bounds.minY + 1;
			if((xSpan > 3.5f * this->params.segBreakerParams.breakGridSize) ||
			   (ySpan > 3.5f * this->params.segBreakerParams.breakGridSize))
			{
				breakSeq = true;
			}
		}

	    if(breakSeq)
		{
			SegmentBreaker segBreaker;
			segBreaker.BreakSegment(iSeg, segmentation, this->params.segBreakerParams);			
		}
	}

	segmentation.ComputeSegAvgColors();
	segmentation.CreateSegmentPoints();
}



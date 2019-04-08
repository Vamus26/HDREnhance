#include "StreamPixIO.h"
#include "Disk.h"
#include "Image.h"
#include "ImageIO.h"
#include "ImageProcessing.h"
#include <time.h>

#ifndef _USE_32BIT_TIME_T
	#error "Need to use 32 bit time to be compatible with StreamPix"
#endif

FILE* StreamPixIO::getSeqFilePtr(string path)
{
	string seqFilePattern;
	vector<string> seqFiles;

	seqFilePattern = path + "*" + STREAM_PIX_SeqExt;
	seqFiles = Disk::MatchesInPattern(seqFilePattern);	
	//check - implement lnk reader
	//if(seqFiles.size() == 0)
	//{
	//	seqFilePattern = path + "*" + STREAM_PIX_SeqLinkExt;
	//	seqFiles = Disk::MatchesInPattern(seqFilePattern);
	//}
	ENSURE(seqFiles.size() == 1);
	string seqFile = seqFiles[0];

	FILE* seqFilePtr = fopen(seqFile.c_str(), "rb");
	ENSURE(seqFilePtr != NULL);

	return seqFilePtr;
}

CSequenceHeader StreamPixIO::getSeqHeader(FILE* seqFilePtr)
{
	ENSURE(seqFilePtr != NULL);

	CSequenceHeader seqHeader;
	size_t elemsRead = fread(&seqHeader, sizeof(seqHeader), 1, seqFilePtr);
	ENSURE(elemsRead == 1);
	ENSURE(seqHeader.HeaderSize == sizeof(seqHeader));

	return seqHeader;
}

int StreamPixIO::GetFramesInSeqFile(Camera &camera)
{
	FILE *seqFilePtr = StreamPixIO::getSeqFilePtr(camera.path);
	CSequenceHeader seqHeader = getSeqHeader(seqFilePtr);
	fclose(seqFilePtr);
	return (int) seqHeader.AllocatedFrame;
}

void StreamPixIO::ExtractSeqData(Camera &camera, FrameSeq frameSeq, int frameCompression)
{
	printf("Extracting sequence data for - %s\n", camera.id.c_str());
	fflush(stdout);

	FILE *seqFilePtr = StreamPixIO::getSeqFilePtr(camera.path);
	CSequenceHeader seqHeader = getSeqHeader(seqFilePtr);

	if(frameSeq.startIndex == -1)
	{
		frameSeq.startIndex = 0;
		frameSeq.endIndex   = (int) seqHeader.AllocatedFrame - 1;
	}
	else
	{
		CHECK_FS_RANGE(frameSeq, 0, (int) seqHeader.AllocatedFrame);
	}

	//extractTimeStamps(seqHeader, seqFilePtr, frameSeq);	
	extractFrames(camera, frameSeq, frameCompression,
				  seqHeader, seqFilePtr);

	fclose(seqFilePtr);
}


void StreamPixIO::extractFrames(Camera &camera, FrameSeq frameSeq, int frameCompression,
								const CSequenceHeader &seqHeader, FILE* seqFilePtr)
{
	CHECK_FS_RANGE(frameSeq, 0, (int) seqHeader.AllocatedFrame);
	ENSURE(seqHeader.ImageInfo.ImageFormat == FORMAT_MONO_BAYER);
	ENSURE(seqHeader.ImageInfo.ImageBitDepth == 8);

	CShape frameShape(seqHeader.ImageInfo.ImageWidth, 
					  seqHeader.ImageInfo.ImageHeight, 
					  1);
	CByteImage frameData(frameShape);
	for(int iFrame = frameSeq.startIndex, outFrameIndex = 0; 
		iFrame <= frameSeq.endIndex; 
		iFrame++, outFrameIndex++)
	{
		long frameTimeOffset = seqHeader.HeaderSize + 
							   (iFrame * seqHeader.TrueImageSize);
		int result = fseek(seqFilePtr, frameTimeOffset, SEEK_SET);
		ENSURE(result == 0);

		size_t elemsRead = fread(frameData.PixelAddress(0, 0, 0), 
								 sizeof(uchar), 
								 seqHeader.ImageInfo.ImageSizeBytes,
								 seqFilePtr);
		ENSURE(elemsRead == seqHeader.ImageInfo.ImageSizeBytes);
		
		CFloatImage colorFrame = ImageProcessing::Demosiac(ImageProcessing::ByteToFloatImg(frameData));
		colorFrame = ImageProcessing::FlipVertical(colorFrame);

		ENSURE(CAMERA_ImageExt.compare(".tif") == 0);
		ImageIO::WriteFileTiff(colorFrame, 
							   camera.GetFrameName(outFrameIndex).c_str(), 
							   frameCompression);
	}
}


void StreamPixIO::extractTimeStamps(Camera &camera, FrameSeq frameSeq,
								    const CSequenceHeader &seqHeader, FILE* seqFilePtr)
{
	CHECK_FS_RANGE(frameSeq, 0, (int) seqHeader.AllocatedFrame);

	string timeStampsFN = camera.GetTimeStampsFN();
	FILE *tsFilePtr = fopen(timeStampsFN.c_str(), "w");
	ENSURE(tsFilePtr != NULL);

	for(int iFrame = frameSeq.startIndex; iFrame <= frameSeq.endIndex; iFrame++)
	{
		time_t timeStamp;
		unsigned short timeStampMS;
		extractFrameTS(iFrame, seqHeader, seqFilePtr,
					   &timeStamp, &timeStampMS);						   

		AccurateTime accurateTime = (timeStamp & 0x00000000ffffffff);
		accurateTime *= 1000;
		accurateTime += timeStampMS;	

		writeFrameTS(accurateTime, tsFilePtr);
	}	
	fclose(tsFilePtr);
}


void StreamPixIO::extractFrameTS(int iFrame, const CSequenceHeader &seqHeader, FILE* seqFilePtr,
							 	 time_t *timeStampPtr, unsigned short *timeStampMS_Ptr)
{
	long frameTimeOffset = seqHeader.HeaderSize + 
						   (iFrame * seqHeader.TrueImageSize) +
						   seqHeader.ImageInfo.ImageSizeBytes;
	int result = fseek(seqFilePtr, frameTimeOffset, SEEK_SET);
	ENSURE(result == 0);

	size_t elemsRead = fread(timeStampPtr, sizeof(time_t), 1, seqFilePtr);
	ENSURE(elemsRead == 1);

	elemsRead = fread(timeStampMS_Ptr, sizeof(unsigned short), 1, seqFilePtr);
	ENSURE(elemsRead == 1);
	ENSURE(*timeStampMS_Ptr < 1000);
}

void StreamPixIO::writeFrameTS(AccurateTime timeStamp, FILE *tsFilePtr)
{
	AccurateTime msComponent  = timeStamp % 1000;
	AccurateTime secComponent = timeStamp / 1000;

	time_t timeStampSec = (time_t) (secComponent & 0x00000000ffffffff);
	int  timeStampMS    = (int)    (msComponent  & 0x00000000ffffffff);

	struct tm *timeStats = gmtime(&timeStampSec);		

	fprintf(tsFilePtr, 
			"%lld - %02i/%02i/%04i, %02i:%02i:%02i, %03i\n",
			timeStamp,			   
			timeStats->tm_mon + 1,  timeStats->tm_mday, timeStats->tm_year + 1900,
			timeStats->tm_hour, timeStats->tm_min,  timeStats->tm_sec,
			timeStampMS);
}

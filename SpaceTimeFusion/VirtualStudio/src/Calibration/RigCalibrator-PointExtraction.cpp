#include "RigCalibrator.h"
#include "Disk.h"
#include "ImageIO.h"
#include <fstream>

void RigCalibrator::findValidFrames()
{
	printf("\tFinding frames with full point extraction...\n");
	int validFramesFound = 0;
	for(int iFrame = this->params.frameSeqToSearch.startIndex;
		iFrame <= this->params.frameSeqToSearch.endIndex;
		iFrame++)
	{
		uint iCam;
		for(iCam = 0; iCam < this->rig->cameras.size(); iCam++)
		{
			CByteImage currCamFrame;

			string camFrameName = this->rig->cameras[iCam].GetFrameName(iFrame);			
			ImageIO::ReadFile(currCamFrame, camFrameName);
			
			string calibImageName = getCalibImageName(iCam, validFramesFound);
			ImageIO::WriteFile(currCamFrame, calibImageName);
			
			if(extractPoints(calibImageName, iCam, validFramesFound,
							 this->params.maxSharpenIters) == false)
			{
				break;
			}
		}

		if(iCam == this->rig->cameras.size())
		{
			printf("\t\tFrame-%05i selected\n", iFrame);
			validFramesFound++;
			if(validFramesFound >= this->params.framesToFind)
			{
				break;
			}
		}
		else
		{
			printf("\t\tFrame-%05i rejected\n", iFrame);
		}
	}

	ENSURE(validFramesFound == this->params.framesToFind);
}

bool RigCalibrator::extractPoints(string calibImageName, int iCam, int iFrame, 
								  int maxSharpenIters)
{

	int prevIterPointCount = 0;
	int iSharpen = 0;
	do
	{
		int currIterPointCount = 0;
		if(extractPoints(calibImageName, iCam, iFrame, &currIterPointCount) == true)
		{
			return true;
		}

		if(currIterPointCount < prevIterPointCount)
		{
			return false;
		}

		if(iSharpen < maxSharpenIters)
		{
			printf("\t\t\t Sharpening - %s (%i pts found)\n", 
				   this->rig->cameras[iCam].id.c_str(),
				   currIterPointCount);

			CByteImage currCamFrame;			
			ImageIO::ReadFile(currCamFrame, calibImageName.c_str());
			ImageProcessing::DirectionalSmoothing(currCamFrame);
			ImageIO::WriteFile(currCamFrame, calibImageName.c_str());
			
			//char outCmd[1024];
			//sprintf(outCmd, "%s %s /sharpen=%i /convert=%s",
			//		getIrfanViewBinFN().c_str(),
			//		calibImageName.c_str(),
			//		sharpenAmount,
			//		calibImageName.c_str());
			//ENSURE(SilentSysCall(outCmd) == true);
		}

		prevIterPointCount = currIterPointCount;
		iSharpen++;
	} while(iSharpen <= maxSharpenIters);

	return false;
}


bool RigCalibrator::extractPoints(string calibImageName, int iCam, int iFrame, int *pointsFound)
{
	bool extractionSuccess = false;

	writeExtractPointsOptFile(calibImageName, iCam, iFrame);

	char outCmd[1024];
	sprintf(outCmd, "%s < %s > %s",
		    getExtractPtBinFN().c_str(),
		    getExtractPtOptFN().c_str(), getExtractPtOutFN().c_str());

	if(SilentSysCall(outCmd, false, true) == false)
	{
		return false;
	}

	FILE* outFilePtr = fopen(getExtractPtOutFN().c_str(), "r");
	ENSURE(outFilePtr != NULL);
	int elemScanned = fscanf(outFilePtr, "Points found %d", pointsFound);
	ENSURE(elemScanned == 1);

	if(*pointsFound >= RigCalibrator::MIN_POINTS_FOR_CALIB)
	{
		extractionSuccess = true;
	}	

	return extractionSuccess;
}

void RigCalibrator::writeExtractPointsOptFile(string calibImageName, int iCam, int iFrame)
{
	string optFileName = getExtractPtOptFN();
	ofstream outStream;
	outStream.open(optFileName.c_str());
	ENSURE(outStream.is_open());

	outStream << "fname = " << calibImageName << "\n";
	outStream << "inverse = 0\n";
	outStream << "nBits = 5\n";
	outStream << "minArea = 75\n";
	outStream << "nSquares = 4\n";
	outStream << "nCircles = 3\n";
	outStream << "wSquares = 0.6\n";
	outStream << "mirror = 0\n";
	outStream << "pointname = "  << getPointsFileName(iCam, iFrame) << "\n";
	outStream << "modelname = "  << getModelFileName(iCam, iFrame)  << "\n";

	outStream.close();
}

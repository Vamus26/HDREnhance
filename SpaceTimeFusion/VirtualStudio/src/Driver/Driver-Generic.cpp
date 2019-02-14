#include "Driver.h"
#include "Timer.h"
#include "FlowHelper.h"
#include "Timer.h"

void Driver::execGenericOp()
{
	ENSURE(this->params.genericParams.intList1.size() >= 1);
	int opCode = this->params.genericParams.intList1[0];

	switch(opCode)
	{
	case 0:
		generateLargeImages();
		break;

	case 1:
		generateCropedScaledVideo();
		break;

	case 2:
		//equalize video colors
		equalizeDataColors(true);
		break;

	case 3:
		//equalize image colors
		equalizeDataColors(false);
		break;

	case 4:
		genSmoothCamPath();
		break;


	default:
		REPORT_FAILURE("Unknown operation code: %i\n", opCode);
		break;
	}
}

void Driver::genSmoothCamPath()
{
	printf("Generating smooth camera path...\n");

	ENSURE(this->params.genericParams.floatList.size() == 0);
	ENSURE(this->params.genericParams.intList1.size() == 5);
	ENSURE(this->params.genericParams.intList2.size() == 0);
	ENSURE(this->params.genericParams.strList1.size() == 0);
	ENSURE(this->params.genericParams.strList2.size() == 0);

	int winSize         = this->params.genericParams.intList1[1];
	int winStdDev       = this->params.genericParams.intList1[2];
	int iters           = this->params.genericParams.intList1[3];
	int interFrameCount = this->params.genericParams.intList1[4];

	printf("Using [winSize, winStdDev, iters] - [%i, %i, %i]\n", winSize, winStdDev, iters);

	Rig rig;
	rig.Load(this->params.rigParams);
	rig.GenSmoothCameraPath(winSize, winStdDev, iters, interFrameCount);

	printf("\tDone.\n");
}

void Driver::generateLargeImages()
{
	printf("Generating large images...\n");
	Timer timer(true);

	ENSURE(this->params.genericParams.floatList.size() == 1);
	ENSURE(this->params.genericParams.intList1.size() == 2);
	ENSURE(this->params.genericParams.intList2.size() == 0);
	ENSURE(this->params.genericParams.strList1.size() == 0);
	ENSURE(this->params.genericParams.strList2.size() == 0);

	Rig rig;
	rig.Load(this->params.rigParams);
	string rigDirname = rig.GetRigDirName();

	ENSURE(rig.params.frameSeq.startIndex == rig.params.frameSeq.endIndex);
	int iFrame = rig.params.frameSeq.startIndex;

	float imgScale = this->params.genericParams.floatList[0];
	ENSURE(imgScale > 1.0f);

	int startCamIndex = 0;
	int endCamIndex = (int) rig.cameras.size() - 1;
	if(this->params.rigParams.targetCamIndex != -1)
	{
		startCamIndex = endCamIndex = this->params.rigParams.targetCamIndex;
		CHECK_RANGE(startCamIndex, 0, (int) rig.cameras.size() - 1);		
	}

	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{		
		CFloatImage largeImg;
		string targetImgFN = rig.cameras[iCam].GetFrameNameLarge(iFrame);

		if((this->params.overwriteResults == false) &&
		   (Disk::DoesFileExist(targetImgFN) == true))
		{
			continue;
		}

		if(rig.IsVideoFrame(iCam) == false)
		{
			string srcImgFN = rigDirname + rig.cameras[iCam].id + "_Large" + CAMERA_ImageExt;
			INSIST(Disk::DoesFileExist(srcImgFN) == true);
			ImageIO::ReadFile(largeImg, srcImgFN);

			string orgImgFN = rig.cameras[iCam].GetFrameName(iFrame);
			CShape orgImgShape = ImageIO::GetImageShape(orgImgFN);
			CShape largeImgShape = largeImg.Shape();

			ENSURE(fabs(largeImgShape.width  - (orgImgShape.width  * imgScale)) <= 1.01f);
			ENSURE(fabs(largeImgShape.height - (orgImgShape.height * imgScale)) <= 1.01f);
			ENSURE(largeImgShape.nBands == orgImgShape.nBands);
		}
		else
		{
			string srcFN = rig.cameras[iCam].GetFrameName(iFrame);
			INSIST(Disk::DoesFileExist(srcFN) == true);
			ImageIO::ReadFile(largeImg, srcFN, imgScale);
		}
		
		ImageIO::WriteFile(largeImg, targetImgFN);
		printf("."); fflush(stdout);
	}

	timer.DumpTime("\n\tDone.");
}


void Driver::generateCropedScaledVideo()
{
	printf("Generating croped scaled video...\n");
	Timer timer(true);

	checkFrameCropParams(3);
	
	Rig rig;
	rig.Load(this->params.rigParams);	

	ENSURE(rig.params.frameSeq.startIndex == rig.params.frameSeq.endIndex);
	int iFrame = rig.params.frameSeq.startIndex;

	int startCamIndex = 0;
	int endCamIndex = (int) rig.cameras.size() - 1;
	if(this->params.rigParams.targetCamIndex != -1)
	{		
		startCamIndex = endCamIndex = this->params.rigParams.targetCamIndex;
		CHECK_RANGE(startCamIndex, 0, (int) rig.cameras.size() - 1);
	}

	CShape targetShape; 
	targetShape.width  = this->params.genericParams.intList1[1];
	targetShape.height = this->params.genericParams.intList1[2];

	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{		
		if(rig.IsVideoFrame(iCam) == true)
		{
			string orgImgFN = rig.cameras[iCam].GetFrameName(iFrame);
			CFloatImage orgImg;
			ImageIO::ReadFile(orgImg, orgImgFN);
			CShape orgImgShape = orgImg.Shape();
			targetShape.nBands = orgImgShape.nBands;

			float scale;
			int cropX, cropY, cropW, cropH;
			getCamScaleCrop(iCam, 3, orgImgShape, targetShape,
				            scale, cropX, cropY, cropW, cropH);

			CFloatImage vidFrame;
			if(scale != 1.0f)
				ImageProcessing::ScaleImage(vidFrame, orgImg, scale);
			else
				vidFrame = orgImg.Clone();

			if((cropW != vidFrame.Shape().width) ||
			   (cropH != vidFrame.Shape().height))
			{
				vidFrame = ImageProcessing::CropImage(vidFrame, cropX, cropY, cropW, cropH);
			}
			else
			{
				ENSURE(cropX == 0); 
				ENSURE(cropY == 0);
			}

			string vidrFrameFN = rig.params.outDir + rig.cameras[iCam].id + "-crop" + CAMERA_ImageExt;
			ImageIO::WriteFile(vidFrame, vidrFrameFN);
			ImageIO::WriteFile(vidFrame, rig.cameras[iCam].GetFrameNameCrop(iFrame));

			Disk::DeleteFile(rig.cameras[iCam].GetFrameFlowNameCrop(iFrame));
			Disk::DeleteFile(rig.cameras[iCam].GetFrameRevFlowNameCrop(iFrame));

			printf("Cam - %s. Crop - [%i, %i, %i %i]. Scale - %f\n", 
				   rig.cameras[iCam].id.c_str(),
				   cropX, cropY, cropW, cropH, scale);
			fflush(stdout);			
		}
	}

	timer.DumpTime("\tDone.");
}


void Driver::checkFrameCropParams(int int1ListExtraParamCount)
{
	INSIST(this->params.genericParams.floatList.size() == 0);

	INSIST((int) this->params.genericParams.intList1.size() >= int1ListExtraParamCount);

	INSIST((this->params.genericParams.intList1.size() - int1ListExtraParamCount) % 5 == 0);
	INSIST(((this->params.genericParams.intList1.size() - int1ListExtraParamCount) / 5 == 0) ||
		   ((this->params.genericParams.intList1.size() - int1ListExtraParamCount) / 5 >= 2));	

	INSIST(this->params.genericParams.intList2.size() == 0);
	INSIST(this->params.genericParams.strList1.size() == 0);
	INSIST(this->params.genericParams.strList2.size() == 0);
}

void Driver::getCamScaleCrop(int iCam, int int1ListOffset, CShape camFrameShape, CShape vidFrameShape,
							 float &scale, int &cropX, int &cropY, int &cropW, int &cropH, bool invertYaxis)
{
	if((int) this->params.genericParams.intList1.size() < 5 + int1ListOffset)
	{
		scale = 1.0f;
		cropX = cropY = 0; 
		cropW = vidFrameShape.width;
		cropH = vidFrameShape.height;
		return;
	}

	int   cropIndex1 = 0;
	int   cropCam1   = this->params.genericParams.intList1[int1ListOffset + (cropIndex1 * 5) + 0];
	int   cropX1     = this->params.genericParams.intList1[int1ListOffset + (cropIndex1 * 5) + 1];
	int   cropY1     = this->params.genericParams.intList1[int1ListOffset + (cropIndex1 * 5) + 2]; 
	int   cropW1     = this->params.genericParams.intList1[int1ListOffset + (cropIndex1 * 5) + 3]; 
	int   cropH1     = this->params.genericParams.intList1[int1ListOffset + (cropIndex1 * 5) + 4]; 

	int   cropIndex2 = cropIndex1 + 1;
	int   cropCam2   = this->params.genericParams.intList1[int1ListOffset + (cropIndex2 * 5) + 0];
	int   cropX2     = this->params.genericParams.intList1[int1ListOffset + (cropIndex2 * 5) + 1];
	int   cropY2     = this->params.genericParams.intList1[int1ListOffset + (cropIndex2 * 5) + 2]; 
	int   cropW2     = this->params.genericParams.intList1[int1ListOffset + (cropIndex2 * 5) + 3]; 
	int   cropH2     = this->params.genericParams.intList1[int1ListOffset + (cropIndex2 * 5) + 4]; 

	INSIST(cropCam1 <= iCam);
	while(cropCam2 < iCam)
	{
		cropIndex1 = cropIndex2;
		cropCam1   = cropCam2;
		cropX1     = cropX2;    
		cropY1     = cropY2;    
		cropW1     = cropW2;    
		cropH1     = cropH2;

		cropIndex2 = cropIndex1 + 1;
		cropCam2   = this->params.genericParams.intList1[int1ListOffset + (cropIndex2 * 5) + 0];
		cropX2     = this->params.genericParams.intList1[int1ListOffset + (cropIndex2 * 5) + 1];
		cropY2     = this->params.genericParams.intList1[int1ListOffset + (cropIndex2 * 5) + 2];
		cropW2     = this->params.genericParams.intList1[int1ListOffset + (cropIndex2 * 5) + 3];
		cropH2     = this->params.genericParams.intList1[int1ListOffset + (cropIndex2 * 5) + 4];				
	}
	INSIST(cropCam2 > cropCam1);

	float cropWeight2 = (float) (iCam - cropCam1) / (cropCam2 - cropCam1);
	float cropWeight1 = 1.0f - cropWeight2;

	float orgCropX = ((cropWeight1 * cropX1) + (cropWeight2 * cropX2));
	float orgCropY = ((cropWeight1 * cropY1) + (cropWeight2 * cropY2));
	float orgCropW = ((cropWeight1 * cropW1) + (cropWeight2 * cropW2));
	float orgCropH = ((cropWeight1 * cropH1) + (cropWeight2 * cropH2));

	INSIST(camFrameShape.InBounds(orgCropX, orgCropY));
	INSIST(orgCropW <= camFrameShape.width);
	INSIST(orgCropH <= camFrameShape.height);
	if(orgCropY + orgCropH >= camFrameShape.height)
	{
		orgCropY = camFrameShape.height - 1 - (orgCropH - 1);
	}
	if(orgCropX + orgCropW >= camFrameShape.width)
	{
		orgCropX = camFrameShape.width - 1 - (orgCropW - 1);
	}
	INSIST(camFrameShape.InBounds(orgCropX + orgCropW - 1, orgCropY + orgCropH - 1));

	if(invertYaxis)
	{
		orgCropY = camFrameShape.height - 1 - orgCropY;
		orgCropY -= (orgCropH - 1);
	}

	// + 3 to avoid off scaling mismatch
	scale = (vidFrameShape.width + 1) / (float) orgCropW;
	ENSURE(fabs(scale - ((vidFrameShape.height + 1) / (float) orgCropH)) <= 0.01);

	cropX = NEAREST_INT(orgCropX * scale);
	cropY = NEAREST_INT(orgCropY * scale);
	cropW = vidFrameShape.width;
	cropH = vidFrameShape.height;
}


void Driver::equalizeDataColors(bool equalizeVideoColor)
{
	ENSURE(this->params.genericParams.floatList.size() == 0);
	ENSURE(this->params.genericParams.intList1.size() == 1);
	ENSURE(this->params.genericParams.intList2.size() == 0);
	ENSURE(this->params.genericParams.strList1.size() == 0);
	ENSURE(this->params.genericParams.strList2.size() == 0);

	Rig rig;
	rig.Load(this->params.rigParams);
	ENSURE(rig.params.frameSeq.startIndex == rig.params.frameSeq.endIndex);

	CFloatImage baseImg;

	bool baseImgFound = false;
	for(uint iCam = 0; iCam < rig.cameras.size(); iCam++)
	{
		if(equalizeVideoColor)
		{
			if(rig.IsVideoFrame(iCam) == false) continue;
		}
		else
		{
			if(rig.IsVideoFrame(iCam) == true) continue;
		}

		string baseImgFN = rig.cameras[iCam].GetFrameName(rig.params.frameSeq.startIndex);
		ImageIO::ReadFile(baseImg, baseImgFN);
		baseImgFound = true;
		break;
	}
	ENSURE(baseImgFound == true);

	int startCamIndex = 0;
	int endCamIndex = (int) rig.cameras.size() - 1;

	if(this->params.rigParams.targetCamIndex != -1)
	{
		CHECK_RANGE(this->params.rigParams.targetCamIndex, 0, (int) rig.cameras.size() - 1);
		startCamIndex = endCamIndex = this->params.rigParams.targetCamIndex;
	}


	Timer timer;
	printf("Equalizing %s colors...\n", (equalizeVideoColor ? "video" : "image"));
	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{
		printf("."); fflush(stdout);
		if(equalizeVideoColor)
		{
			if(rig.IsVideoFrame(iCam) == false) continue;
		}
		else
		{
			if(rig.IsVideoFrame(iCam) == true) continue;
		}

		CFloatImage targetImg;
		string targetImgFN = rig.cameras[iCam].GetFrameName(rig.params.frameSeq.startIndex);
		ImageIO::ReadFile(targetImg, targetImgFN);
		
		targetImg = ImageProcessing::TransferColor(targetImg, baseImg);
		ImageIO::WriteFile(targetImg, targetImgFN);
		ImageIO::WriteFile(targetImg, rig.params.outDir + Disk::GetBaseFileName(targetImgFN));
	}

	timer.DumpSeconds("\n\tDone.");
}

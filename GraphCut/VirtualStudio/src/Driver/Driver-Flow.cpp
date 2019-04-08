#include "Driver.h"
#include "FlowHelper.h"
#include "ImageIO.h"
#include "Disk.h"
#include "Timer.h"

void Driver::computeFlow(bool computeLargeFlow)
{
	computeBlackFlow(computeLargeFlow);
	//computeStereoFlow();
	//if(computeLargeFlow)
	//{
	//	REPORT_FAILURE("No large flow equivalent feature exists for stereo flow.");
	//}
}

void Driver::computeFlowCrop()
{
	//computeBlackFlowCrop();
	computeStereoFlowCrop();
}


void Driver::computeBlackFlow(bool computeLargeFlow)
{
	if(computeLargeFlow)
		printf("Generating large black flow...\n");
	else
		printf("Generating black flow...\n");
	Timer timer(true);

	fclose(stderr);

	Rig rig;
	rig.Load(this->params.rigParams);

	ENSURE(rig.cameras.size() > 0);
	uint startCamIndex = 0;
	uint endCamIndex   = (uint) rig.cameras.size() - 2;

	ENSURE(rig.params.frameSeq.startIndex == rig.params.frameSeq.endIndex);
	int currFrameIndex = rig.params.frameSeq.startIndex;

	if(rig.params.targetCamIndex != -1)
	{
		CHECK_RANGE(rig.params.targetCamIndex, 0, (int) rig.cameras.size() - 2);
		startCamIndex = endCamIndex = rig.params.targetCamIndex;
	}

	for(uint iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{
		if(rig.IsVideoFrame(iCam) == false) continue;
		if(rig.IsVideoFrame(iCam + 1) == false) continue;

		string flowFN    = rig.cameras[iCam + 1].GetFrameFlowName(currFrameIndex); 
		string revFlowFN = rig.cameras[iCam].GetFrameRevFlowName(currFrameIndex);

		if(computeLargeFlow)
		{
			flowFN    = rig.cameras[iCam + 1].GetFrameFlowNameLarge(currFrameIndex);
			revFlowFN = rig.cameras[iCam].GetFrameRevFlowNameLarge(currFrameIndex);
		}
		else
		{
			flowFN    = rig.cameras[iCam + 1].GetFrameFlowName(currFrameIndex);
			revFlowFN = rig.cameras[iCam].GetFrameRevFlowName(currFrameIndex);
		}

		if(this->params.overwriteResults == false)
		{
			if(Disk::DoesFileExist(flowFN) && Disk::DoesFileExist(revFlowFN))
			{
				continue;
			}
		}
		
		CFloatImage camImg;
		CFloatImage nextCamImg;

		if(computeLargeFlow)
		{
			ImageIO::ReadFile(camImg, rig.cameras[iCam].GetFrameNameLarge(currFrameIndex));
			ImageIO::ReadFile(nextCamImg, rig.cameras[iCam + 1].GetFrameNameLarge(currFrameIndex));
		}
		else
		{
			ImageIO::ReadFile(camImg, rig.cameras[iCam].GetFrameName(currFrameIndex));
			ImageIO::ReadFile(nextCamImg, rig.cameras[iCam + 1].GetFrameName(currFrameIndex));
		}
		
		ENSURE(camImg.Shape() == nextCamImg.Shape());
		
		CShape flowFieldShape(camImg.Shape().width, camImg.Shape().height, 3);

		CFloatImage flowFieldImg(flowFieldShape);
		CFloatImage nextCamFlowInput = ImageProcessing::TransferColor(camImg, nextCamImg);
		FlowHelper::ComputeFlow(nextCamFlowInput, nextCamImg, flowFieldImg);		

		CFloatImage revFlowFieldImg(flowFieldShape);
		CFloatImage currCamFlowInput = ImageProcessing::TransferColor(nextCamImg, camImg);
		FlowHelper::ComputeFlow(currCamFlowInput, camImg, revFlowFieldImg);

		//for(int y = 0; y < flowFieldShape.height; y++)
		//for(int x = 0; x < flowFieldShape.width; x++)
		//{
		//	if((x < 10) || (y < 10) ||
		//	   ((flowFieldShape.width  - x) <= 10) ||
		//	   ((flowFieldShape.height - y) <= 10))
		//	{
		//		flowFieldImg.Pixel(x, y, 0) = FLT_MAX;
		//		flowFieldImg.Pixel(x, y, 1) = FLT_MAX;
		//		flowFieldImg.Pixel(x, y, 2) = 0.0f;				

		//		revFlowFieldImg.Pixel(x, y, 0) = FLT_MAX;
		//		revFlowFieldImg.Pixel(x, y, 1) = FLT_MAX;
		//		revFlowFieldImg.Pixel(x, y, 2) = 0.0f;
		//	}
		//}

		FlowHelper::WriteFlowField(flowFN, flowFieldImg);
		FlowHelper::WriteFlowField(revFlowFN, revFlowFieldImg);

		printf("."); fflush(stdout);

		//CByteImage flowWarpMask(camImg.Shape().width, camImg.Shape().height, 1);

		//CFloatImage flowWarpImg1(camImg.Shape());				
		//FlowHelper::WarpImage(flowWarpImg1, flowWarpMask, 
		//					  nextCamFlowInput, flowFieldImg);
		//ImageIO::WriteFile(flowWarpImg1, "1.tga");
		//PRINT_DOUBLE(ImageProcessing::L2Diff(flowWarpImg1, nextCamImg, flowWarpMask, true));
		//
		//CFloatImage flowWarpImg2(camImg.Shape());
		//FlowHelper::WarpImage(flowWarpImg2, flowWarpMask, 
		//					  currCamFlowInput, revFlowFieldImg);
		//ImageIO::WriteFile(flowWarpImg2, "2.tga");
		//PRINT_DOUBLE(ImageProcessing::L2Diff(flowWarpImg2, camImg, flowWarpMask, true));
		//getchar();
	}

	timer.DumpTime("\tDone.");
}

void Driver::computeStereoFlow()
{
	printf("Generating stereo flow...\n");
	Timer timer(true);

	Rig rig;
	rig.Load(this->params.rigParams);

	ENSURE(rig.cameras.size() > 0);
	int startCamIndex = 0;
	int endCamIndex   = (int) rig.cameras.size() - 2;

	ENSURE(rig.params.frameSeq.startIndex == rig.params.frameSeq.endIndex);
	int currFrameIndex = rig.params.frameSeq.startIndex;
	this->params.srParams.iFrame = currFrameIndex;

	if(rig.params.targetCamIndex != -1)
	{
		CHECK_RANGE(rig.params.targetCamIndex, 0, (int) rig.cameras.size() - 2);
		startCamIndex = endCamIndex = rig.params.targetCamIndex;
	}

	StereoReconstruction stereoReconstruction;
	stereoReconstruction.Init(this->params.stereoParams, this->params.rigParams);

	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{
		if(rig.IsVideoFrame(iCam) == false) continue;
		if(rig.IsVideoFrame(iCam + 1) == false) continue;

		string flowFN    = rig.cameras[iCam + 1].GetFrameFlowName(currFrameIndex); 
		string revFlowFN = rig.cameras[iCam].GetFrameRevFlowName(currFrameIndex);

		if(this->params.overwriteResults == false)
		{
			if(Disk::DoesFileExist(flowFN) && Disk::DoesFileExist(revFlowFN))
			{
				continue;
			}
		}

		CFloatImage camImg;
		ImageIO::ReadFile(camImg, rig.cameras[iCam].GetFrameName(currFrameIndex));
		//ImageIO::ReadFile(camImg, StereoReconstruction::GetTargetContextNovelViewFN(&rig.cameras[iCam], 0, rig.params.outDir));
		
		CFloatImage nextCamImg;
		ImageIO::ReadFile(nextCamImg, rig.cameras[iCam + 1].GetFrameName(currFrameIndex));		
		//ImageIO::ReadFile(nextCamImg, StereoReconstruction::GetTargetContextNovelViewFN(&rig.cameras[iCam + 1], 0, rig.params.outDir));

		ENSURE(camImg.Shape() == nextCamImg.Shape());
		CShape flowFieldShape(camImg.Shape().width, camImg.Shape().height, 3);

		CFloatImage flowFieldImg(flowFieldShape); 
		stereoReconstruction.ComputeFlow(iCam, iCam + 1, flowFieldImg, params.srParams);
		FlowHelper::WriteFlowField(flowFN, flowFieldImg);

		CFloatImage revFlowFieldImg(flowFieldShape); 
		stereoReconstruction.ComputeFlow(iCam + 1, iCam, revFlowFieldImg, params.srParams);
		FlowHelper::WriteFlowField(revFlowFN, revFlowFieldImg);

		printf("."); fflush(stdout);				

		//CFloatImage nextCamFlowInput = ImageProcessing::TransferColor(camImg, nextCamImg);
		//CFloatImage currCamFlowInput = ImageProcessing::TransferColor(nextCamImg, camImg);

		//CByteImage flowWarpMask(camImg.Shape().width, camImg.Shape().height, 1);
		//CFloatImage errImg(flowWarpMask.Shape());

		//CFloatImage flowWarpImg1(camImg.Shape());		
		//FlowHelper::WarpImage(flowWarpImg1, flowWarpMask, 
		//					  nextCamFlowInput, flowFieldImg);
		//ImageIO::WriteFile(flowWarpImg1, "1.tga");
		//PRINT_DOUBLE(ImageProcessing::L2Diff(flowWarpImg1, nextCamImg, flowWarpMask, true));
		//ImageProcessing::CopyChannel(errImg, flowFieldImg, 0, 2);
		//ImageIO::WriteFile(errImg, "1-err.tga");
		//
		//CFloatImage flowWarpImg2(camImg.Shape());
		//FlowHelper::WarpImage(flowWarpImg2, flowWarpMask, 
		//					  currCamFlowInput, revFlowFieldImg);
		//ImageIO::WriteFile(flowWarpImg2, "2.tga");
		//PRINT_DOUBLE(ImageProcessing::L2Diff(flowWarpImg2, camImg, flowWarpMask, true));	
		//ImageProcessing::CopyChannel(errImg, revFlowFieldImg, 0, 2);
		//ImageIO::WriteFile(errImg, "2-err.tga");
		//getchar();
	}

	timer.DumpTime("\tDone.");
}

void Driver::computeStereoFlowCrop()
{
	printf("Generating scaled croped stereo flow...\n");
	Timer timer(true);

	checkFrameCropParams(2);

	Rig rig;
	rig.Load(this->params.rigParams);

	ENSURE(rig.cameras.size() > 0);
	int startCamIndex = 0;
	int endCamIndex   = (int) rig.cameras.size() - 2;

	ENSURE(rig.params.frameSeq.startIndex == rig.params.frameSeq.endIndex);
	int iFrame = rig.params.frameSeq.startIndex;
	this->params.srParams.iFrame = iFrame;

	if(rig.params.targetCamIndex != -1)
	{
		CHECK_RANGE(rig.params.targetCamIndex, 0, (int) rig.cameras.size() - 2);
		startCamIndex = endCamIndex = rig.params.targetCamIndex;
	}

	CShape srcShape;
	for(uint iCam = 0; iCam < rig.cameras.size(); iCam++)
	{
		if(rig.IsVideoFrame(iCam) == false) continue;

		srcShape = ImageIO::GetImageShape(rig.cameras[iCam].GetFrameName(iFrame));

		break;
	}

	CShape targetShape; 
	targetShape.width  = this->params.genericParams.intList1[0];
	targetShape.height = this->params.genericParams.intList1[1];
	targetShape.nBands = srcShape.nBands;

	StereoReconstruction stereoReconstruction;
	stereoReconstruction.Init(this->params.stereoParams, this->params.rigParams);
	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{
		int iNextCam = iCam + 1;

		if(rig.IsVideoFrame(iCam) == false) continue;
		if(rig.IsVideoFrame(iNextCam) == false) continue;

		string flowFN    = rig.cameras[iNextCam].GetFrameFlowNameCrop(iFrame); 
		string revFlowFN = rig.cameras[iCam].GetFrameRevFlowNameCrop(iFrame);

		if(this->params.overwriteResults == false)
		{
			if(Disk::DoesFileExist(flowFN) && Disk::DoesFileExist(revFlowFN))
			{
				continue;
			}
		}

		float scale;
		int cropX, cropY, cropW, cropH;
		getCamScaleCrop(iCam, 2, srcShape, targetShape,
			            scale, cropX, cropY, cropW, cropH);

		float revScale;
		int revCropX, revCropY, revCropW, revCropH;
		getCamScaleCrop(iNextCam, 2, srcShape, targetShape,
			            revScale, revCropX, revCropY, revCropW, revCropH);

		CShape flowFieldShape(targetShape.width, targetShape.height, 3);

		CFloatImage flowFieldImg(flowFieldShape); 
		stereoReconstruction.ComputeFlow(iCam, iNextCam, flowFieldImg, params.srParams,
										 scale,    CVector2(cropX, cropY),       CVector2(cropW, cropH),
										 revScale, CVector2(revCropX, revCropY), CVector2(revCropW, revCropH));

		CFloatImage revFlowFieldImg(flowFieldShape); 
		stereoReconstruction.ComputeFlow(iNextCam, iCam, revFlowFieldImg, params.srParams,
										 revScale, CVector2(revCropX, revCropY), CVector2(revCropW, revCropH),
										 scale,    CVector2(cropX, cropY),       CVector2(cropW, cropH));


		int oldTargetCamIndex = this->params.rigParams.targetCamIndex;
		this->params.rigParams.targetCamIndex = iCam;		
		computeBlackFlowCrop();
		
		this->params.rigParams.targetCamIndex = oldTargetCamIndex;

		CFloatImage blackFlowFieldImg(flowFieldShape);
		CFloatImage blackRevFlowFieldImg(flowFieldShape);
		FlowHelper::ReadFlowField(flowFN, blackFlowFieldImg);
		FlowHelper::ReadFlowField(revFlowFN, blackRevFlowFieldImg);

		uint flowAddr = 0;
		//for(int y = 0; y < flowFieldShape.height; y++)
		//for(int x = 0; x < flowFieldShape.width;  x++, flowAddr += 3)
		for(int y = 10; y < flowFieldShape.height - 10; y++)
		for(int x = 10; x < flowFieldShape.width - 10;  x++, flowAddr += 3)
		{
			if(flowFieldImg[flowAddr + 2] == 0.0)
			{
				flowFieldImg[flowAddr + 0] = blackFlowFieldImg[flowAddr + 0];
				flowFieldImg[flowAddr + 1] = blackFlowFieldImg[flowAddr + 1];
				//check
				//flowFieldImg[flowAddr + 2] = 100.0f * blackFlowFieldImg[flowAddr + 2];			
				flowFieldImg[flowAddr + 2] = 1.0f * blackFlowFieldImg[flowAddr + 2];
			}

			if(revFlowFieldImg[flowAddr + 2] == 0.0)
			{
				revFlowFieldImg[flowAddr + 0] = blackRevFlowFieldImg[flowAddr + 0];
				revFlowFieldImg[flowAddr + 1] = blackRevFlowFieldImg[flowAddr + 1];
				//check
				//revFlowFieldImg[flowAddr + 2] = 100.0f * blackRevFlowFieldImg[flowAddr + 2];
				revFlowFieldImg[flowAddr + 2] = 1.0f * blackRevFlowFieldImg[flowAddr + 2];
			}
		}

		FlowHelper::WriteFlowField(flowFN, flowFieldImg);
		FlowHelper::WriteFlowField(revFlowFN, revFlowFieldImg);

		printf("."); fflush(stdout);

		//CFloatImage camImg;
		//ImageIO::ReadFile(camImg, rig.cameras[iCam].GetFrameNameCrop(iFrame));
		////ImageIO::ReadFile(camImg, StereoReconstruction::GetTargetContextNovelViewFN(&rig.cameras[iCam], 0, rig.params.outDir) );
		//
		//CFloatImage nextCamImg;
		//ImageIO::ReadFile(nextCamImg, rig.cameras[iNextCam].GetFrameNameCrop(iFrame));		
		////ImageIO::ReadFile(nextCamImg, StereoReconstruction::GetTargetContextNovelViewFN(&rig.cameras[iNextCam], 0, rig.params.outDir) );
		//ENSURE(camImg.Shape() == nextCamImg.Shape());

		//CFloatImage nextCamFlowInput = ImageProcessing::TransferColor(camImg, nextCamImg);
		//CFloatImage currCamFlowInput = ImageProcessing::TransferColor(nextCamImg, camImg);

		//CByteImage flowWarpMask(camImg.Shape().width, camImg.Shape().height, 1);
		//CFloatImage errImg(flowWarpMask.Shape());

		//CFloatImage flowWarpImg1(camImg.Shape());		
		//FlowHelper::WarpImage(flowWarpImg1, flowWarpMask, 
		//					  nextCamFlowInput, flowFieldImg);
		//ImageIO::WriteFile(flowWarpImg1, "1.tga");
		//PRINT_DOUBLE(ImageProcessing::L2Diff(flowWarpImg1, nextCamImg, flowWarpMask, true));
		//ImageProcessing::CopyChannel(errImg, flowFieldImg, 0, 2);
		//ImageIO::WriteFile(errImg, "1-err.tga");
		//
		//
		//CFloatImage flowWarpImg2(camImg.Shape());
		//FlowHelper::WarpImage(flowWarpImg2, flowWarpMask, 
		//					  currCamFlowInput, revFlowFieldImg);
		//ImageIO::WriteFile(flowWarpImg2, "2.tga");
		//PRINT_DOUBLE(ImageProcessing::L2Diff(flowWarpImg2, camImg, flowWarpMask, true));	
		//ImageProcessing::CopyChannel(errImg, revFlowFieldImg, 0, 2);
		//ImageIO::WriteFile(errImg, "2-err.tga");

		//getchar();
	}

	timer.DumpTime("\tDone.");
}



void Driver::computeBlackFlowCrop()
{
	printf("Generating scaled croped black flow...\n");
	Timer timer(true);

	checkFrameCropParams(2);
	
	Rig rig;
	rig.Load(this->params.rigParams);	

	ENSURE(rig.params.frameSeq.startIndex == rig.params.frameSeq.endIndex);
	int iFrame = rig.params.frameSeq.startIndex;

	int startCamIndex = 0;
	int endCamIndex = (int) rig.cameras.size() - 2;
	if(rig.params.targetCamIndex != -1)
	{
		CHECK_RANGE(rig.params.targetCamIndex, 0, (int) rig.cameras.size() - 2);
		startCamIndex = endCamIndex = rig.params.targetCamIndex;
	}	

	//check
	bool useLargeFlow = false;

	CShape srcShape;
	CShape srcShapeLarge;
	float largeScale = 1.0f;

	for(uint iCam = 0; iCam < rig.cameras.size(); iCam++)
	{
		if(rig.IsVideoFrame(iCam) == false) continue;

		 srcShape = ImageIO::GetImageShape(rig.cameras[iCam].GetFrameName(iFrame));
		 if(useLargeFlow)
		 {
			 srcShapeLarge = ImageIO::GetImageShape(rig.cameras[iCam].GetFrameNameLarge(iFrame));
			 largeScale = srcShapeLarge.width / (float) srcShape.width;
			 INSIST(fabs(largeScale - srcShapeLarge.height / (float) srcShape.height) < 0.01f);
		 }
			
		 break;
	}

	CShape targetShape; 
	targetShape.width  = this->params.genericParams.intList1[0];
	targetShape.height = this->params.genericParams.intList1[1];
	targetShape.nBands = srcShape.nBands;

	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{	
		if(rig.IsVideoFrame(iCam) == false) continue;
		if(rig.IsVideoFrame(iCam + 1) == false) continue;

		CFloatImage camImgCrop;
		ImageIO::ReadFile(camImgCrop, rig.cameras[iCam].GetFrameNameCrop(iFrame));
		CFloatImage nextCamImgCrop;
		ImageIO::ReadFile(nextCamImgCrop, rig.cameras[iCam + 1].GetFrameNameCrop(iFrame));

		float scale;
		int cropX, cropY, cropW, cropH;
		getCamScaleCrop(iCam, 2, srcShape, targetShape,
			            scale, cropX, cropY, cropW, cropH);		
		ENSURE(camImgCrop.Shape().width  == cropW);
		ENSURE(camImgCrop.Shape().height == cropH);

		float revScale;
		int revCropX, revCropY, revCropW, revCropH;
		getCamScaleCrop(iCam + 1, 2, srcShape, targetShape,
			            revScale, revCropX, revCropY, revCropW, revCropH);
		ENSURE(nextCamImgCrop.Shape().width  == revCropW);
		ENSURE(nextCamImgCrop.Shape().height == revCropH);

		bool useLargeFlowForCurrCam = useLargeFlow;
		if((scale == 1.0f) && (revScale == 1.0f))
		{
			useLargeFlowForCurrCam = false;
		}

		if(useLargeFlowForCurrCam) 
			scale = scale / largeScale;

		if(useLargeFlowForCurrCam) 
			revScale = revScale / largeScale;

		string flowFN; 
		string revFlowFN;

		if(useLargeFlowForCurrCam)
		{
			flowFN = rig.cameras[iCam + 1].GetFrameFlowNameLarge(iFrame); 
			revFlowFN = rig.cameras[iCam].GetFrameRevFlowNameLarge(iFrame);
		}
		else
		{
			flowFN = rig.cameras[iCam + 1].GetFrameFlowName(iFrame); 
			revFlowFN = rig.cameras[iCam].GetFrameRevFlowName(iFrame);
		}

		CFloatImage flowFieldImg;
		FlowHelper::ReadFlowField(flowFN, flowFieldImg); 
		CFloatImage revFlowFieldImg;
		FlowHelper::ReadFlowField(revFlowFN, revFlowFieldImg); 		

		INSIST(flowFieldImg.Shape() == revFlowFieldImg.Shape());
		INSIST(flowFieldImg.Shape().nBands == 3);
		if(useLargeFlowForCurrCam)
		{
			INSIST(flowFieldImg.Shape().SameIgnoringNBands(srcShapeLarge));
		}
		else
		{
			INSIST(flowFieldImg.Shape().SameIgnoringNBands(srcShape));		
		}
	

		CFloatImage flowFieldImgCrop = ImageProcessing::ScaleImageWithoutSampling(flowFieldImg, revScale);
		flowFieldImgCrop = ImageProcessing::CropImage(flowFieldImgCrop, revCropX, revCropY, revCropW, revCropH);
		ENSURE(flowFieldImgCrop.Shape().SameIgnoringNBands(nextCamImgCrop.Shape()));

		CFloatImage revFlowFieldImgCrop = ImageProcessing::ScaleImageWithoutSampling(revFlowFieldImg, scale);
		revFlowFieldImgCrop = ImageProcessing::CropImage(revFlowFieldImgCrop, cropX, cropY, cropW, cropH);
		ENSURE(revFlowFieldImgCrop.Shape().SameIgnoringNBands(camImgCrop.Shape()));
		
		for(int iFlow = 0; iFlow <= 1; iFlow++)
		{
			CShape uvFieldShape(targetShape.width, targetShape.height, 2);
			CFloatImage uvFieldOrg(uvFieldShape);
			CFloatImage uvFieldCrop(uvFieldShape);
			
			int currCropX, currCropY, invCropX, invCropY;
			float currScale, invScale;
			CFloatImage currFlowFieldImgCrop;
			CFloatImage currCropFlowInputImg;
			CFloatImage currCropFlowTargetImg;

			if(iFlow == 0)
			{
				currCropX = revCropX; currCropY = revCropY;
				invCropX  = cropX;    invCropY  = cropY;
				currScale = revScale; invScale = scale;
				currFlowFieldImgCrop  = flowFieldImgCrop;
				currCropFlowInputImg  = camImgCrop;
				currCropFlowTargetImg = nextCamImgCrop;
			}
			else
			{
				INSIST(iFlow == 1);
				currCropX = cropX;    currCropY = cropY;
				invCropX  = revCropX; invCropY  = revCropY;				
				currScale = scale;    invScale = revScale;
				currFlowFieldImgCrop  = revFlowFieldImgCrop;
				currCropFlowInputImg  = nextCamImgCrop;
				currCropFlowTargetImg = camImgCrop;
			}
			currCropFlowInputImg = ImageProcessing::TransferColor(currCropFlowInputImg, currCropFlowTargetImg);

			uint uvFieldAddr = 0;
			for(int y = 0; y < uvFieldShape.height; y++)
			for(int x = 0; x < uvFieldShape.width;  x++, uvFieldAddr += 2)
			{
				uvFieldOrg[uvFieldAddr + 0]  = (float) x + currCropX;
				uvFieldOrg[uvFieldAddr + 1]  = (float) y + currCropY;
				uvFieldCrop[uvFieldAddr + 0] = (float) x;
				uvFieldCrop[uvFieldAddr + 1] = (float) y;
			}

			ImageProcessing::Operate(uvFieldOrg,           uvFieldOrg,           currScale,            ImageProcessing::OP_DIV, 0);
			ImageProcessing::Operate(uvFieldOrg,           uvFieldOrg,           currScale,            ImageProcessing::OP_DIV, 1);
			ImageProcessing::Operate(currFlowFieldImgCrop, uvFieldOrg,           currFlowFieldImgCrop, ImageProcessing::OP_SUB, 0);
			ImageProcessing::Operate(currFlowFieldImgCrop, uvFieldOrg,           currFlowFieldImgCrop, ImageProcessing::OP_SUB, 1);
			ImageProcessing::Operate(currFlowFieldImgCrop, currFlowFieldImgCrop, invScale,             ImageProcessing::OP_MUL, 0);
			ImageProcessing::Operate(currFlowFieldImgCrop, currFlowFieldImgCrop, invScale,             ImageProcessing::OP_MUL, 1);
			ImageProcessing::Operate(currFlowFieldImgCrop, currFlowFieldImgCrop, (float) invCropX,     ImageProcessing::OP_SUB, 0);
			ImageProcessing::Operate(currFlowFieldImgCrop, currFlowFieldImgCrop, (float) invCropY,     ImageProcessing::OP_SUB, 1);
			ImageProcessing::Operate(currFlowFieldImgCrop, uvFieldCrop,          currFlowFieldImgCrop, ImageProcessing::OP_SUB, 0);
			ImageProcessing::Operate(currFlowFieldImgCrop, uvFieldCrop,          currFlowFieldImgCrop, ImageProcessing::OP_SUB, 1);


			CFloatImage warpImgCrop(currCropFlowTargetImg.Shape());
			CByteImage warpMaskCrop(warpImgCrop.Shape().width, warpImgCrop.Shape().height, 1);			
			FlowHelper::WarpImage(warpImgCrop, warpMaskCrop, 
								  currCropFlowInputImg, currFlowFieldImgCrop);

			float maxColDist = sqrt((float) warpImgCrop.Shape().nBands);
			uint maskAddr = 0, pixelAddr = 0, flowAddr = 0;
			for(int y = 0; y < warpImgCrop.Shape().height; y++)
			for(int x = 0; x < warpImgCrop.Shape().width;  x++, maskAddr++, pixelAddr += warpImgCrop.Shape().nBands, flowAddr += 3)
			{
				if(currFlowFieldImgCrop[flowAddr + 2] > 0.0f)
				{
					currFlowFieldImgCrop[flowAddr + 2] = 0.0f;
					if(warpMaskCrop[maskAddr] == MASK_VALID)
					{
						for(int iBand = 0; iBand < warpImgCrop.Shape().nBands; iBand++)
						{
							currFlowFieldImgCrop[flowAddr + 2] += 
								SQR(warpImgCrop[pixelAddr + iBand] - 
									currCropFlowTargetImg[pixelAddr + iBand]);
						}
						currFlowFieldImgCrop[flowAddr + 2] = sqrt(currFlowFieldImgCrop[flowAddr + 2]);
						currFlowFieldImgCrop[flowAddr + 2] = 1.0f - (currFlowFieldImgCrop[flowAddr + 2] / maxColDist);
						currFlowFieldImgCrop[flowAddr + 2] = max(0.0f, min(currFlowFieldImgCrop[flowAddr + 2], 1.0f));
					}
				}				
			}

			//char filename[1024];
			//sprintf(filename, "%i.tga", iFlow + 1);
			//ImageIO::WriteFile(warpImgCrop, filename);

			//CFloatImage errChannel(warpMaskCrop.Shape());
			//ImageProcessing::CopyChannel(errChannel, currFlowFieldImgCrop, 0, 2);
			//sprintf(filename, "%i-err.tga", iFlow + 1);
			//ImageIO::WriteFile(errChannel, filename);
		}


		string flowCropFN = rig.cameras[iCam + 1].GetFrameFlowNameCrop(iFrame); 
		FlowHelper::WriteFlowField(flowCropFN, flowFieldImgCrop); 

		string revFlowCropFN = rig.cameras[iCam].GetFrameRevFlowNameCrop(iFrame);
		FlowHelper::WriteFlowField(revFlowCropFN, revFlowFieldImgCrop);

		printf("Cam - %s.\n", rig.cameras[iCam].id.c_str());
		fflush(stdout);
	}

	timer.DumpTime("\tDone.");
}
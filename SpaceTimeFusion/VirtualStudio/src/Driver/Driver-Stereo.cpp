#include "Driver.h"
#include "ImageIO.h"
#include "Disk.h"
#include "FlowHelper.h"
#include "VideoSolver.h"
#include "Timer.h"

void Driver::computeStereo()
{
	Stereo stereo;
	stereo.Init(this->params.stereoParams, this->params.rigParams);
	stereo.ComputeStereo();	
}

void Driver::backUpStereoBP_Data()
{
	Stereo stereo;
	stereo.Init(this->params.stereoParams, this->params.rigParams);
	stereo.BackUpBP_Data();	
}

void Driver::computeStereoIter()
{
	Stereo stereo;
	stereo.Init(this->params.stereoParams, this->params.rigParams);
	stereo.ComputeStereoIter();	
}

void Driver::advanceStereoIter()
{
	Stereo stereo;
	stereo.Init(this->params.stereoParams, this->params.rigParams);
	stereo.AdvanceStereoIter();	
}

/*
void Driver::computeStereoReconstruction()
{
	INSIST(this->params.srParams.reconstructUsingAnalogy == true);
	INSIST(this->params.srParams.srsCamIDs.size() == 0);

	vector<string> pendingQueue;
	vector<string> processedSet;

	string seedCamID = this->params.srParams.targetCamID;
	INSIST(seedCamID.size() > 0);
	pendingQueue.push_back(seedCamID);

	StereoReconstruction stereoReconstructor;
	stereoReconstructor.Init(this->params.stereoParams, this->params.rigParams);

	while(pendingQueue.size() > 0)
	{
		string targetCamID = pendingQueue[pendingQueue.size() - 1];
		pendingQueue.pop_back();
		processedSet.push_back(targetCamID);

		Camera *targetCam = stereoReconstructor.rig.GetCamera(targetCamID);
		INSIST(targetCam != NULL);
		INSIST(stereoReconstructor.rig.IsVideoFrame(targetCamID) == false);

		const CameraNeighs &targetCamNeighs = targetCam->cameraNeighs;
		for(int iNeighCam = 0; iNeighCam < targetCamNeighs.size(); iNeighCam++)
		{
			string neighCamID = targetCamNeighs[iNeighCam].id;			
			if(stereoReconstructor.rig.IsVideoFrame(neighCamID) == true) continue;

			bool neighCamProcessed = false;
			for(int iProcessedCam = 0; iProcessedCam < processedSet.size(); iProcessedCam++)
			{
				if(processedSet[iProcessedCam].compare(neighCamID) == 0)
				{
					neighCamProcessed = true;
					break;
				}
			}
			if(neighCamProcessed) continue;

			Camera *neighCam = stereoReconstructor.rig.GetCamera(neighCamID);
			INSIST(neighCam != NULL);
			StereoReconstruction::SR_Params neighCamSR_Params = this->params.srParams;
			neighCamSR_Params.targetCamID = neighCamID;
			
			for(uint iNeighNeighCam = 0; iNeighNeighCam < neighCam->cameraNeighs.size(); iNeighNeighCam++)
			{
				string neighNeighCamID = neighCam->cameraNeighs[iNeighNeighCam].id;
				if(stereoReconstructor.rig.IsVideoFrame(neighNeighCamID) == true) continue;

				bool neighNeighCamProcessed = false;
				for(int iProcessedCam = 0; iProcessedCam < processedSet.size(); iProcessedCam++)
				{
					if(processedSet[iProcessedCam].compare(neighNeighCamID) == 0)
					{
						neighNeighCamProcessed = true;
						break;
					}
				}

				if(neighNeighCamProcessed == true)
				{
					neighCamSR_Params.srsCamIDs.push_back(neighNeighCamID);
					neighCamSR_Params.srsCamScales.push_back(neighCamSR_Params.targetScale);
					neighCamSR_Params.srsCamFillerOnly.push_back(false);					
				}
			}

			INSIST(neighCamSR_Params.srsCamIDs.size() > 0);				
			pendingQueue.push_back(neighCamID);

			stereoReconstructor.Reconstruct(neighCamSR_Params, this->params.overwriteResults);

			CFloatImage newAnalogyImg;
			ImageIO::ReadFile(newAnalogyImg, 
							  StereoReconstruction::GetTargetContextBlendFN(neighCam, neighCamSR_Params.iFrame, stereoReconstructor.rig.params.outDir));
			ImageIO::WriteFile(newAnalogyImg, neighCam->GetFrameAnalogyName(neighCamSR_Params.iFrame));
		}
	}
}
//*/

/*
void Driver::computeStereoReconstruction()
{
	StereoReconstruction stereoReconstructor;
	stereoReconstructor.Init(this->params.stereoParams, this->params.rigParams);
	//stereoReconstructor.Reconstruct(this->params.srParams, this->params.overwriteResults);
	//stereoReconstructor.ReconstructNV(this->params.srParams, this->params.overwriteResults);
	stereoReconstructor.ReconstructMask(this->params.srParams, this->params.overwriteResults);
}
//*/


///*
void Driver::computeStereoReconstruction()
{
	ENSURE(this->params.srParams.srsCamIDs.size() == 0);
	ENSURE(this->params.srParams.srsCamScales.size() == 0);
	ENSURE(this->params.srParams.srsCamFillerOnly.size() == 0);
	ENSURE(this->params.srParams.neighCamIDs.size() == 0);
	ENSURE(this->params.srParams.neighCamScales.size() == 0);
	ENSURE(this->params.srParams.neighCamCropOrigins.size() == 0);
	ENSURE(this->params.srParams.neighCamCropExts.size() == 0);

	StereoReconstruction stereoReconstructor;
	stereoReconstructor.Init(this->params.stereoParams, this->params.rigParams);

	int startCamIndex = 0;
	int endCamIndex = (int) stereoReconstructor.rig.cameras.size() - 1;

	if(this->params.rigParams.targetCamIndex != -1)
	{
		CHECK_RANGE(this->params.rigParams.targetCamIndex, 0, (int) stereoReconstructor.rig.cameras.size() - 1);
		startCamIndex = endCamIndex = this->params.rigParams.targetCamIndex;
	}

	checkFrameCropParams(2);
	CShape targetShape; 
	targetShape.width  = this->params.genericParams.intList1[0];
	targetShape.height = this->params.genericParams.intList1[1];

	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{
		if(stereoReconstructor.rig.IsVideoFrame(iCam) == false) continue;

		StereoReconstruction::SR_Params srParams = this->params.srParams;
		srParams.iFrame = 0;
		srParams.targetCamID = stereoReconstructor.rig.cameras[iCam].id;		

		string orgImgFN = stereoReconstructor.rig.cameras[iCam].GetFrameName(srParams.iFrame);
		CShape orgImgShape = ImageIO::GetImageShape(orgImgFN);
		targetShape.nBands = orgImgShape.nBands;

		float scale;
		int cropX, cropY, cropW, cropH;
		getCamScaleCrop(iCam, 2, orgImgShape, targetShape,
			            scale, cropX, cropY, cropW, cropH);
		srParams.targetScale         = scale;
		srParams.targetCropOrigin[0] = cropX;
		srParams.targetCropOrigin[1] = cropY;
		srParams.targetCropExt[0]    = cropW;
		srParams.targetCropExt[1]    = cropH;

		const int srsLimit = 2;
		const int neighLimit = 2;
		const int nonFillerLimit = 1;
		for(int iDirc = 0; iDirc <= 1; iDirc++)
		{
			int srsFound = 0;
			int neighsFound = 0;

			int jCamStart, jCamEnd, jCamIncr;
			if(iDirc == 0)
			{
				jCamStart = iCam;
				jCamEnd   = -1;
				jCamIncr  = -1;
			}
			else
			{
				jCamStart = iCam + 1;
				jCamEnd   = (int) stereoReconstructor.rig.cameras.size();
				jCamIncr  = 1;
			}

			for(uint jCam = jCamStart; 
				(jCam != jCamEnd) && ((srsFound < srsLimit) || (neighsFound < neighLimit)); 
				jCam += jCamIncr)
			{
				if(stereoReconstructor.rig.IsVideoFrame(jCam) == false) continue;

				if((neighsFound < neighLimit) && (iCam != jCam))
				{
					string neighImgFN = stereoReconstructor.rig.cameras[jCam].GetFrameName(srParams.iFrame);
					CShape neighImgShape = ImageIO::GetImageShape(neighImgFN);
					INSIST(neighImgShape == orgImgShape);

					float scale;
					int cropX, cropY, cropW, cropH;
					getCamScaleCrop(jCam, 2, neighImgShape, targetShape,
									scale, cropX, cropY, cropW, cropH);

					srParams.neighCamIDs.push_back(stereoReconstructor.rig.cameras[jCam].id);
					srParams.neighCamScales.push_back(scale);
					srParams.neighCamCropOrigins.push_back(CVector2(cropX, cropY));
					srParams.neighCamCropExts.push_back(CVector2(cropW, cropH));

					neighsFound++;
				}

				const CameraNeighs &jCamNeighs = stereoReconstructor.rig.cameras[jCam].cameraNeighs;
				for(uint jNeigh = 0; 
					(jNeigh < jCamNeighs.size()) && (srsFound < srsLimit);
					jNeigh++)
				{
					string jNeighCamID = jCamNeighs[jNeigh].id;
					if(stereoReconstructor.rig.IsVideoFrame(jNeighCamID) == false)
					{
						srParams.srsCamIDs.push_back(jNeighCamID);
						srParams.srsCamScales.push_back(srParams.targetScale);

						if(srsFound < nonFillerLimit)
							srParams.srsCamFillerOnly.push_back(false);
						else
							srParams.srsCamFillerOnly.push_back(true);

						srsFound++;
					}
				}
			}
		}

		stereoReconstructor.Reconstruct(srParams, this->params.overwriteResults);
		//stereoReconstructor.ReconstructMask(srParams, this->params.overwriteResults);
	}
}
//*/

/*
void Driver::computeStereoReconstruction()
{
	ENSURE(this->params.srParams.srsCamIDs.size() == 0);
	ENSURE(this->params.srParams.srsCamScales.size() == 0);
	ENSURE(this->params.srParams.srsCamFillerOnly.size() == 0);
	ENSURE(this->params.srParams.neighCamIDs.size() == 0);
	ENSURE(this->params.srParams.neighCamScales.size() == 0);
	ENSURE(this->params.srParams.neighCamCropOrigins.size() == 0);
	ENSURE(this->params.srParams.neighCamCropExts.size() == 0);

	StereoReconstruction stereoReconstructor;
	stereoReconstructor.Init(this->params.stereoParams, this->params.rigParams);

	int startCamIndex = 0;
	int endCamIndex = (int) stereoReconstructor.rig.cameras.size() - 1;

	if(this->params.rigParams.targetCamIndex != -1)
	{
		CHECK_RANGE(this->params.rigParams.targetCamIndex, 0, (int) stereoReconstructor.rig.cameras.size() - 1);
		startCamIndex = endCamIndex = this->params.rigParams.targetCamIndex;
	}

	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{
		if(stereoReconstructor.rig.IsVideoFrame(iCam) == false) continue;

		StereoReconstruction::SR_Params srParams = this->params.srParams;
		srParams.iFrame = 0;
		srParams.targetCamID = stereoReconstructor.rig.cameras[iCam].id;

		PRINT_STRING(srParams.targetCamID);

		const int srsLimit = 48;		
		for(int iDirc = 0; iDirc <= 1; iDirc++)
		{
			int srsFound = 0; 

			float jCamStart, jCamIncr;
			if(iDirc == 0)
			{
				jCamStart = (float) iCam - 1;
				jCamIncr  = -1.5f;				
			}
			else
			{
				jCamStart = (float) iCam + 1;
				jCamIncr  = 1.5f;
			}

			float camCount = (float) stereoReconstructor.rig.cameras.size();
			for(float jCam = jCamStart; 
				(InRange(jCam, 0.0f, camCount - 1) && (srsFound < srsLimit)); 
				jCam += jCamIncr)
			{
				int jCamIndex = (int) NEAREST_INT(jCam);

				if(stereoReconstructor.rig.IsVideoFrame(jCamIndex) == false) continue;

				string jCamID = stereoReconstructor.rig.cameras[jCamIndex].id;
				srParams.srsCamIDs.push_back(jCamID);
				srParams.srsCamScales.push_back(srParams.targetScale);
				srParams.srsCamFillerOnly.push_back(false);
				srsFound++;

				PRINT_STRING(jCamID);
			}
		}

		stereoReconstructor.ReconstructNV(srParams, this->params.overwriteResults);
	}
}
//*/


/*
void Driver::computeStereoReconstruction()
{
	ENSURE(this->params.srParams.srsCamIDs.size() == 0);
	ENSURE(this->params.srParams.srsCamScales.size() == 0);
	ENSURE(this->params.srParams.srsCamFillerOnly.size() == 0);
	ENSURE(this->params.srParams.neighCamIDs.size() == 0);
	ENSURE(this->params.srParams.neighCamScales.size() == 0);
	ENSURE(this->params.srParams.neighCamCropOrigins.size() == 0);
	ENSURE(this->params.srParams.neighCamCropExts.size() == 0);

	StereoReconstruction stereoReconstructor;
	stereoReconstructor.Init(this->params.stereoParams, this->params.rigParams);

	int startCamIndex = 0;
	int endCamIndex = (int) stereoReconstructor.rig.cameras.size() - 1;

	if(this->params.rigParams.targetCamIndex != -1)
	{
		CHECK_RANGE(this->params.rigParams.targetCamIndex, 0, (int) stereoReconstructor.rig.cameras.size() - 1);
		startCamIndex = endCamIndex = this->params.rigParams.targetCamIndex;
	}

	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{
		if(stereoReconstructor.rig.IsVideoFrame(iCam) == false) continue;

		StereoReconstruction::SR_Params srParams = this->params.srParams;
		srParams.iFrame = 0;
		srParams.targetCamID = stereoReconstructor.rig.cameras[iCam].id;

		PRINT_STRING(srParams.targetCamID);

		int srsFound = 0;

		//srParams.srsCamIDs.push_back("IMG_0001");
		//srParams.srsCamScales.push_back(srParams.targetScale);
		//srParams.srsCamFillerOnly.push_back(false);

		//srParams.srsCamIDs.push_back("IMG_0002");
		//srParams.srsCamScales.push_back(srParams.targetScale);
		//srParams.srsCamFillerOnly.push_back(false);

		//srParams.srsCamIDs.push_back("IMG_0003");
		//srParams.srsCamScales.push_back(srParams.targetScale);
		//srParams.srsCamFillerOnly.push_back(false);

		//srParams.srsCamIDs.push_back("IMG_0004");
		//srParams.srsCamScales.push_back(srParams.targetScale);
		//srParams.srsCamFillerOnly.push_back(false);

		//srParams.srsCamIDs.push_back("IMG_0005");
		//srParams.srsCamScales.push_back(srParams.targetScale);
		//srParams.srsCamFillerOnly.push_back(false);

		//srParams.srsCamIDs.push_back("IMG_0006");
		//srParams.srsCamScales.push_back(srParams.targetScale);
		//srParams.srsCamFillerOnly.push_back(false);

		srParams.srsCamIDs.push_back("IMG_0005");
		srParams.srsCamScales.push_back(srParams.targetScale);
		srParams.srsCamFillerOnly.push_back(false);

		srParams.srsCamIDs.push_back("IMG_0006");
		srParams.srsCamScales.push_back(srParams.targetScale);
		srParams.srsCamFillerOnly.push_back(false);

		srParams.srsCamIDs.push_back("IMG_0009");
		srParams.srsCamScales.push_back(srParams.targetScale);
		srParams.srsCamFillerOnly.push_back(false);

		srParams.srsCamIDs.push_back("IMG_0010");
		srParams.srsCamScales.push_back(srParams.targetScale);
		srParams.srsCamFillerOnly.push_back(false);

		srParams.srsCamIDs.push_back("IMG_0013");
		srParams.srsCamScales.push_back(srParams.targetScale);
		srParams.srsCamFillerOnly.push_back(false);

		srsFound++;

		stereoReconstructor.ReconstructNV(srParams, this->params.overwriteResults);
	}
}
//*/

/*
void Driver::computeStereoReconstruction()
{
	ENSURE(this->params.srParams.srsCamIDs.size() == 0);
	ENSURE(this->params.srParams.srsCamScales.size() == 0);
	ENSURE(this->params.srParams.srsCamFillerOnly.size() == 0);
	ENSURE(this->params.srParams.neighCamIDs.size() == 0);
	ENSURE(this->params.srParams.neighCamScales.size() == 0);
	ENSURE(this->params.srParams.neighCamCropOrigins.size() == 0);
	ENSURE(this->params.srParams.neighCamCropExts.size() == 0);

	StereoReconstruction stereoReconstructor;
	stereoReconstructor.Init(this->params.stereoParams, this->params.rigParams);

	int startCamIndex = 0;
	int endCamIndex = (int) stereoReconstructor.rig.cameras.size() - 1;
	if(this->params.rigParams.targetCamIndex != -1)
	{
		CHECK_RANGE(this->params.rigParams.targetCamIndex, 0, (int) stereoReconstructor.rig.cameras.size() - 1);
		startCamIndex = endCamIndex = this->params.rigParams.targetCamIndex;
	}

	INSIST(stereoReconstructor.rig.cameras.size() == 336);

	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{
		INSIST(iCam <= 167);

		if(stereoReconstructor.rig.IsVideoFrame(iCam) == false) continue;

		StereoReconstruction::SR_Params srParams = this->params.srParams;
		srParams.iFrame = 0;
		srParams.targetCamID = stereoReconstructor.rig.cameras[iCam].id;

		PRINT_STRING(srParams.targetCamID);

		int srsFound = 0;
		int minCamIndex = max(iCam + 168 - 15, 168);
		int maxCamIndex = min(iCam + 168 + 15, 335);
		for(int iNeigh = minCamIndex; iNeigh <= maxCamIndex; iNeigh++)
		{
			string vidID = stereoReconstructor.rig.cameras[iNeigh].id;
			srParams.srsCamIDs.push_back(vidID);
			srParams.srsCamScales.push_back(srParams.targetScale);
			srParams.srsCamFillerOnly.push_back(false);
			srsFound++;
		}		

		stereoReconstructor.ReconstructNV(srParams, this->params.overwriteResults);
	}
}
*/

void Driver::reconstructVideoFNF()
{
	Rig rig;
	rig.Load(this->params.rigParams);

	int startCamIndex = 0;
	int endCamIndex = (int) rig.cameras.size() - 1;
	if(this->params.rigParams.targetCamIndex != -1)
	{
		CHECK_RANGE(this->params.rigParams.targetCamIndex, 0, (int) rig.cameras.size() - 1);
		startCamIndex = endCamIndex = this->params.rigParams.targetCamIndex;
	}

	printf("Computing flash no-flash image..\n"); 
	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{
		printf("."); fflush(stdout); 
		if(rig.IsVideoFrame(iCam) == false) continue;

		string noFlashFN = rig.cameras[iCam].GetFrameName(rig.params.frameSeq.startIndex);
		CFloatImage noFlashImg;
		ImageIO::ReadFile(noFlashImg, noFlashFN);
	
		string flashFN = VideoSolver::GetResultCC_Name(Disk::GetFileNameID(noFlashFN), this->params.vsParams);
		CFloatImage flashImg;
		ImageIO::ReadFile(flashImg, flashFN);

		ImageIO::WriteFile(noFlashImg, "1.tga");
		ImageIO::WriteFile(flashImg, "2.tga");


		CFloatImage flashBase = ImageProcessing::BilateralFilter(flashImg, 
																 this->params.vsParams.cbfParams.detailParams.spaceD, 
																 this->params.vsParams.cbfParams.detailParams.sigD, 
																 this->params.vsParams.cbfParams.detailParams.winHalfWidth);
		CFloatImage detailImg = flashImg.Clone();
		ImageProcessing::Operate(flashBase, flashBase, 0.02f, ImageProcessing::OP_ADD);
		ImageProcessing::Operate(detailImg, detailImg, 0.02f, ImageProcessing::OP_ADD);
		ImageProcessing::Operate(detailImg, detailImg, flashBase, ImageProcessing::OP_DIV);

		CFloatImage result = ImageProcessing::CrossBilateralFilter(noFlashImg, flashImg, detailImg,
																   this->params.vsParams.cbfParams.flashNoFlashParams.spaceD, 
																   this->params.vsParams.cbfParams.flashNoFlashParams.sigD, 
																   this->params.vsParams.cbfParams.flashNoFlashParams.winHalfWidth);

		string resultFN = VideoSolver::GetResultFNF_Name(Disk::GetFileNameID(noFlashFN), this->params.vsParams);
		PRINT_STRING(resultFN);
		//ImageIO::WriteFile(result, resultFN);

		ImageIO::WriteFile(result, "3.tga");
	}	

	printf("\tDone\n");
}

void Driver::reconstructVideo()
{	
	Timer timer;
	VideoSolver::VideoSolverParams vsOptions = this->params.vsParams;

	Rig rig;
	rig.Load(this->params.rigParams);

	ENSURE(rig.params.frameSeq.startIndex == rig.params.frameSeq.endIndex);

	//vsOptions.videoSolverFrameLimit = 30;
	//vsOptions.videoSolverMaxIters = 1;
	//for(int iCam = 0; iCam < 0 + vsOptions.videoSolverFrameLimit; iCam++)
	for(int iCam = 0; iCam < (int) rig.cameras.size(); iCam++)
	{
		if(rig.IsVideoFrame(iCam) == false) continue;

		const Camera *currCam = &rig.cameras[iCam];

		string camTemporalOrgFN = currCam->GetFrameName(rig.params.frameSeq.startIndex);
		string camTemporalCropFN = currCam->GetFrameNameCrop(rig.params.frameSeq.startIndex);
		vsOptions.temporalImageNames.push_back(Disk::GetFileNameID(camTemporalOrgFN));

		string camSpatialFN = StereoReconstruction::GetTargetContextNovelViewFN(currCam, 
																				rig.params.frameSeq.startIndex,
																				rig.params.outDir);

		//string camSpatialFN = StereoReconstruction::GetTargetContextBlendFN(currCam, 
		//																	rig.params.frameSeq.startIndex,
		//																	rig.params.outDir);

		//string camSpatialFN = StereoReconstruction::GetTargetContextRegFN(currCam, 
		//																  rig.params.frameSeq.startIndex,
		//																  rig.params.outDir);

		vsOptions.spatialImageNames.push_back(Disk::GetFileNameID(camSpatialFN));

		if(vsOptions.boostTempGradsUsingMask)
		{
			string camMaskFN = StereoReconstruction::GetTargetContextMaskFN(currCam, 
																			rig.params.frameSeq.startIndex,
																			rig.params.outDir);
			vsOptions.frameMaskNames.push_back(Disk::GetFileNameID(camMaskFN));
		}
	}

	if(this->params.appOP == OptFileParser::OP_RECONSTRUCT_VIDEO_VS)
	{
		VideoSolver solver;
		solver.ComputeVideoStats(vsOptions);
		timer.DumpTime("  Done computing video color stats.");
	}
	else if(this->params.appOP == OptFileParser::OP_RECONSTRUCT_VIDEO_CR)
	{
		VideoSolver solver;
		solver.CombineResults(vsOptions);
		timer.DumpTime("  Done combining results.");
	}
	else if(this->params.appOP == OptFileParser::OP_RECONSTRUCT_VIDEO_CC)
	{
		VideoSolver solver;
		solver.ColorCorrect(vsOptions);
		timer.DumpTime("  Done color correcting results.");
	}
	else
	{
		ENSURE(this->params.appOP == OptFileParser::OP_RECONSTRUCT_VIDEO);

		uint frameCount = (uint) vsOptions.spatialImageNames.size();

		ENSURE(frameCount == vsOptions.temporalImageNames.size());
		ENSURE(frameCount > 0);

		if(vsOptions.videoSolverFrameLimit <= 0)
		{
			vsOptions.videoSolverFrameLimit = frameCount;
		}

		for(int iIter = 0; iIter < vsOptions.videoSolverMaxIters; iIter++)
		{
			for(uint iFrame = 0; iFrame < frameCount; /* iFrame incremented at the bottom*/)
			{
				int partitionSize = min((int)(frameCount - iFrame), vsOptions.videoSolverFrameLimit);
				if((iFrame == 0) && (iIter % 2 == 1))
				{
					if(vsOptions.videoSolverFrameLimit < (int) frameCount)
						partitionSize = vsOptions.videoSolverFrameLimit / 2;
				}

				printf("Solving frames: %i - %i\n", iFrame, iFrame + partitionSize - 1);

				VideoSolver::VideoSolverParams partitionOptions = vsOptions;
				partitionOptions.spatialImageNames.clear();
				partitionOptions.temporalImageNames.clear();
				for(int i = 0; i < partitionSize; i++)
				{
					partitionOptions.spatialImageNames.push_back(vsOptions.spatialImageNames[i + iFrame]);
					partitionOptions.temporalImageNames.push_back(vsOptions.temporalImageNames[i + iFrame]);
				}

				if(iFrame > 0)
				{
					partitionOptions.startConstraintFrame    = vsOptions.temporalImageNames[iFrame - 1];
					partitionOptions.useStartConstraintFrame = true;
					if(params.vsParams.boostTempGradsUsingMask)
					{
						partitionOptions.startConstraintMask = vsOptions.frameMaskNames[iFrame - 1];
					}
				}

				if((iIter > 0) || (params.vsParams.initUsingExistingSol))
				{
					partitionOptions.initUsingExistingSol = true;
					if(iFrame + partitionSize < frameCount - 1)
					{
						partitionOptions.endConstraintFrame    = vsOptions.temporalImageNames[iFrame + partitionSize];
						partitionOptions.useEndConstraintFrame = true;
						if(params.vsParams.boostTempGradsUsingMask)
						{
							partitionOptions.endConstraintMask = vsOptions.frameMaskNames[iFrame + partitionSize];
						}
					}
				}
				else
				{
					ENSURE(partitionOptions.initUsingExistingSol == false);
					ENSURE(partitionOptions.useEndConstraintFrame == false);
				}

				partitionOptions.colorCorrect = false;

				VideoSolver solver;
				solver.Solve(partitionOptions);
				printf("\n");

				iFrame += partitionSize;
			}
		}

		if(vsOptions.channel == -1)
		{
			VideoSolver solver;
			solver.CombineResults(vsOptions);
		}

		if(vsOptions.colorCorrect == true)
		{
			ENSURE(vsOptions.channel == -1);
			VideoSolver solver;
			solver.ColorCorrect(vsOptions);
		}

		timer.DumpSeconds("  Done reconstructing video..");
	}
}




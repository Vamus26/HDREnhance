#include "Stereo.h"
#include "Disk.h"
#include "Segmenter.h"
#include "ImageProcessing.h"
#include "ImageIO.h"
#include "Timer.h"

//load and unload only when necessary
//smooth proj belief
//bayer pattern bug

Stereo::Stereo()
{
	this->currView         = NULL;
	this->camSegmentations = NULL;
	this->scenePlaneCount  = 0;
}

Stereo::~Stereo()
{
	ENSURE(this->camSegmentations == NULL);
}

void Stereo::ComputeStereoIter()
{
	ENSURE(this->stereoParams.processInMem == false);

	FrameSeq frameSeg = this->rig.params.frameSeq;
	ENSURE(frameSeg.startIndex == frameSeg.endIndex);
	CHECK_RANGE(frameSeg.startIndex, 0, this->rig.frameCount - 1);

	CHECK_RANGE(this->rig.params.targetCamIndex, 0, (int) this->rig.cameras.size() - 1);

	loadFrameSegmentations(frameSeg.startIndex);

	//bug
	if(this->rig.cameras[this->rig.params.targetCamIndex].cameraNeighs.size() == 0) { unloadFrameSegmentations(frameSeg.startIndex); return; }
	
	loadCamFrameContext(this->rig.params.targetCamIndex, frameSeg.startIndex, 0);
	//visScenePlaneProjections(0);
	processCurrCamFrame();
	unloadCurrCamFrameContext();

	unloadFrameSegmentations(frameSeg.startIndex);
}


void Stereo::BackUpBP_Data()
{
	printf("Backing up BP data...\n");

	FrameSeq frameSeg = this->rig.params.frameSeq;
	INSIST(frameSeg.startIndex == frameSeg.endIndex);
	for(uint iCam = 0; iCam < this->rig.cameras.size(); iCam++)
	{
		printf("."); fflush(stdout);
		this->rig.cameras[iCam].BackUpBP_Data(frameSeg.startIndex);
	}

	printf("\n");
}

void Stereo::AdvanceStereoIter()
{
	ENSURE(this->stereoParams.processInMem == false);

	FrameSeq frameSeg = this->rig.params.frameSeq;
	ENSURE(frameSeg.startIndex == frameSeg.endIndex);
	CHECK_RANGE(frameSeg.startIndex, 0, this->rig.frameCount - 1);
	
	ENSURE(this->rig.params.targetCamIndex == -1);

	loadFrameSegmentations(frameSeg.startIndex);

	updatePrevIterDynamicData(frameSeg.startIndex);

	unloadFrameSegmentations(frameSeg.startIndex);
}


void Stereo::ComputeStereo()
{
	ENSURE(this->rig.params.targetCamIndex == -1);

	printf("Computing Stereo...\n");
	FrameSeq frameSeg = this->rig.params.frameSeq;
	for(int iFrame = frameSeg.startIndex; iFrame <= frameSeg.endIndex; iFrame++)
	{
		printf("Computing segmentation for frame - %i\n", iFrame);
		loadFrameSegmentations(iFrame);

		//visSFM_Geo();

		printf("Computing stereo for frame - %i\n", iFrame);
		computeFrameStereo(iFrame);
	
		unloadFrameSegmentations(iFrame);
	}
}

void Stereo::computeFrameStereo(int iFrame)
{
	for(int iIter = 0; iIter < this->stereoParams.bpIters; iIter++)
	{
		int startCamIndex = 0;

		for(uint iCam = startCamIndex; iCam < this->rig.cameras.size(); iCam++)
		{
			//bug
			if(this->rig.cameras[iCam].cameraNeighs.size() == 0) continue;

			//bug
			//loadCamFrameContext(iCam, iFrame, iIter);
			loadCamFrameContext(iCam, iFrame, iIter, true);
			visScenePlaneProjections(0);

			processCurrCamFrame();
			unloadCurrCamFrameContext();
		}

		updatePrevIterDynamicData(iFrame);
	}
}

void Stereo::updatePrevIterDynamicData(int iFrame)
{
	bool currIterDynamicDataComputed = true;
	for(uint iCam = 0; iCam < this->rig.cameras.size(); iCam++)
	{
		if((this->stereoParams.onlyProcessPhotos == false) ||
		   (this->rig.IsVideoFrame(iCam) == false))
		{
			if(Segmentation::CurrIterDynamicDataExists(this->rig.cameras[iCam], iFrame) == false)
			{
				currIterDynamicDataComputed = false;
				break;
			}
		}
	}

	if(currIterDynamicDataComputed == true)
	{
		for(uint iCam = 0; iCam < this->rig.cameras.size(); iCam++)
		{
			if((this->stereoParams.onlyProcessPhotos == false) ||
			   (this->rig.IsVideoFrame(iCam) == false))
			{
				Segmentation::UpdatePrevIterDynamicData(this->rig.cameras[iCam], iFrame);
			}
		}
	}
}

void Stereo::processCurrCamFrame()
{
	//if(this->currView->iIter == 0)
	//{		
	//	initBP_data();
	//	writeDepthImages();
	//	return;
	//}

	bool neighViewsSegmented = true;
	for(uint iView = 0; iView < this->neighViews.size(); iView++)
	{
		if(this->neighViews[iView].seg->segments.size() == 0)
		{
			neighViewsSegmented = false;
			break;
		}
	}

	if(neighViewsSegmented == false)
	{
		ENSURE(this->currView->seg->fullyComputed == false);
		printf("Skipping static data computation for cam-%s. "
			   "Neigh view segmentation incomplete.\n",
				this->currView->cam.id.c_str());
	}
	else
	{
		if(this->currView->seg->fullyComputed == false)
		{
			computeCurrViewStaticData();
			initBP_data();
			writeDepthImages();
		}
		else
		{
			bool neighViewsFullyComputed = true;
			for(uint iView = 0; iView < this->neighViews.size(); iView++)
			{
				if(this->neighViews[iView].seg->fullyComputed == false)
				{
					neighViewsFullyComputed = false;
					break;
				}
			}

			if(neighViewsFullyComputed)
			{
				if((this->stereoParams.onlyProcessPhotos) &&
				   (this->rig.IsVideoFrame(this->currView->iCam)))
				{				
					printf("Skipping BP iter for cam-%s. "
						   "Processing only photographs.\n",
						   this->currView->cam.id.c_str());
				}
				else
				{
					bp_iteration();
					writeDepthImages();
				}
			}
			else
			{
				printf("Skipping BP iter for cam-%s. "
					   "Neigh view computation incomplete.\n",
					   this->currView->cam.id.c_str());
			}
		}
	}	
}

CShortImage Stereo::GetCurrViewPlaneIndexImage()
{
	const vector<Segment> &currViewSegs = this->currView->seg->segments;
	const CIntImage &currViewLabels     = this->currView->seg->labelImg;

	CShape maskShape = currViewLabels.Shape();
	CShortImage planeIndexImg(maskShape);

	uint nodeAddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	for(int x = 0; x < maskShape.width;  x++, nodeAddr++)
	{
		int nodeLabel = currViewLabels[nodeAddr];
		const Segment &nodeSeg = currViewSegs[nodeLabel];

		int iPlane = nodeSeg.segData->iLabel;
		CHECK_RANGE(iPlane, -1, this->currView->cam.planeCount - 1);
		CHECK_RANGE(iPlane, SHRT_MIN, SHRT_MAX);
		planeIndexImg[nodeAddr] = (short) iPlane;
	}

	return planeIndexImg;
}



CFloatImage Stereo::GetDepthsVis(const CShortImage &planeIndexImg, 
								 const Camera &cam,
								 bool usePureDepths)
{
	CShape maskShape = planeIndexImg.Shape();
	ENSURE(maskShape.nBands == 1);

	CFloatImage depthVisImg(maskShape.width, maskShape.height, 3);
	depthVisImg.ClearPixels();

	double depthRange = cam.maxDepth - cam.minDepth;
	INSIST(depthRange > 0.0);

	uint pixelAddr = 0;
	uint nodeAddr  = 0;
	for(int y = 0; y < maskShape.height; y++)
	for(int x = 0; x < maskShape.width;  x++, nodeAddr++, pixelAddr += 3)
	{
		short iPlane = planeIndexImg[nodeAddr];
		if(iPlane == SEGMENT_OccDepPlaneIndex)
			depthVisImg[pixelAddr + 0] = 1.0f;
		else
		{
			CHECK_RANGE(iPlane, 0, cam.planeCount - 1);			

			if(usePureDepths)
			{
				CVector2 uv(x, y);
				CVector3 worldXYZ;
				cam.cameraMats.projUVtoXYZ(uv, cam.planes[iPlane], worldXYZ);

				CHECK_RANGE(worldXYZ[2], cam.minDepth, cam.maxDepth);
				float depVisVal = (float) (1.0 - ((worldXYZ[2] - cam.minDepth) / depthRange));

				depthVisImg[pixelAddr + 0] = depVisVal;
				depthVisImg[pixelAddr + 1] = depVisVal;
				depthVisImg[pixelAddr + 2] = depVisVal;
			}
			else
			{
				depthVisImg[pixelAddr + 0] = (float) cam.planes[iPlane].col[0];
				depthVisImg[pixelAddr + 1] = (float) cam.planes[iPlane].col[1];
				depthVisImg[pixelAddr + 2] = (float) cam.planes[iPlane].col[2];
			}
		}
	}

	return depthVisImg;
}

CFloatImage Stereo::GetDepthsVis(const CShortImage &planeIndexImg, bool usePureDepths)
{
	return GetDepthsVis(planeIndexImg, this->currView->cam, usePureDepths);
}

CFloatImage Stereo::GetCurrViewDepthsVis()
{
	CShortImage planeIndexImg = GetCurrViewPlaneIndexImage();
	return GetDepthsVis(planeIndexImg);
}

void Stereo::writeDepthImages()
{
	CShortImage planeIndexImg = GetCurrViewPlaneIndexImage();
	CFloatImage depthVisImg   = GetDepthsVis(planeIndexImg, true);

	string depthVisFN = this->currView->cam.GetFrameDepsVisName(this->currView->iFrame);
	ImageIO::WriteFile(depthVisImg, depthVisFN);

	string depthIndexFN = this->currView->cam.GetFrameDepsName(this->currView->iFrame);
	ImageIO::WriteRaw(planeIndexImg, depthIndexFN);
}


void Stereo::Step()
{
	static bool doneWithStereo = false;

	if(doneWithStereo == true)
	{
		printf("Done computing stereo.");
		return;
	}

	int iCam, iIter, iFrame;
	if(this->currView == NULL)
	{
		iCam = 0;
		iIter = 0;
		iFrame = this->rig.params.frameSeq.startIndex;		
		loadFrameSegmentations(iFrame);		
	}
	else
	{
		iFrame = this->currView->iFrame;
		iCam   = this->currView->iCam;
		iIter  = this->currView->iIter;

		unloadCurrCamFrameContext();

		if(iCam == (int) this->rig.cameras.size() - 1)
		{
			if(iIter == this->stereoParams.bpIters - 1)
			{
				updatePrevIterDynamicData(iFrame);

				if(iFrame == this->rig.params.frameSeq.endIndex)
				{
					doneWithStereo = true;
					printf("Done computing stereo.");
					return;
				}
				else
				{
					unloadFrameSegmentations(iFrame); 
					iIter = 0;
					iCam = 0;
					iFrame++;					
					loadFrameSegmentations(iFrame);					
				}
			}
			else
			{
				iIter++;
				iCam = 0;
			}
		}
		else
		{
			iCam++;
		}		
	}

	loadCamFrameContext(iCam, iFrame, iIter);
	
	//processCurrCamFrame();
}


void Stereo::CleanUpStereo()
{
	ENSURE(this->stereoParams.processInMem == false);

	FrameSeq frameSeg = this->rig.params.frameSeq;
	ENSURE(frameSeg.startIndex == frameSeg.endIndex);
	CHECK_RANGE(frameSeg.startIndex, 0, this->rig.frameCount - 1);

	CHECK_RANGE(this->rig.params.targetCamIndex, 0, (int) this->rig.cameras.size() - 1);

	loadFrameSegmentations(frameSeg.startIndex);

	//bug
	if(this->rig.cameras[this->rig.params.targetCamIndex].cameraNeighs.size() == 0) { unloadFrameSegmentations(frameSeg.startIndex); return; }
	
	loadCamFrameContext(this->rig.params.targetCamIndex, frameSeg.startIndex, 0);
	
	vector<Segment> &currSegs = this->currView->seg->segments;
	for(vector<Segment>::iterator currSeg = currSegs.begin();
		currSeg != currSegs.end();
		currSeg++)
	{	
		if(((currSeg->bounds.maxX - currSeg->bounds.minX) > 45) &&
		   ((currSeg->bounds.maxY - currSeg->bounds.minY) > 45))
		{
			continue;
		}

		int iCurrSegLabel = currSeg->segData->iLabel;
		INSIST(iCurrSegLabel >= this->scenePlaneCount);

		if(iCurrSegLabel >= 130) continue;
		if((currSeg->point[0] > 379) && (currSeg->point[0] < 641) && (currSeg->point[1] > 474)) continue;
		//if(currSeg->point[0] > 984) continue;

		int neighCount = currSeg->spatialNeighs.neighCount;
		for(int iNeigh = 0; iNeigh < neighCount; iNeigh++)
		{
			int neighID = currSeg->spatialNeighs.neighID[iNeigh];
			const Segment &neighSeg = currSegs[neighID];
			
			int iNeighSegLabel = neighSeg.segData->iLabel;
			INSIST(iNeighSegLabel >= this->scenePlaneCount);

			if((iNeighSegLabel - iCurrSegLabel) > 15)
			{
				float temp = currSeg->segData->belief[iNeighSegLabel];
				currSeg->segData->belief[iNeighSegLabel] = currSeg->segData->belief[iCurrSegLabel] + 0.000001f;
				currSeg->segData->belief[iCurrSegLabel] = temp;

				currSeg->segData->iLabel = iNeighSegLabel;
				break;
			}
		}
	}

	//CShortImage planeIndexImg = GetCurrViewPlaneIndexImage();
	//CFloatImage depthVis = GetDepthsVis(planeIndexImg);
	//ImageIO::WriteFile(depthVis, "1.tga");
	//exit(0);

	writeDepthImages();	
	unloadCurrCamFrameContext();
	Segmentation::UpdatePrevIterDynamicData(this->rig.cameras[this->rig.params.targetCamIndex], 0);
	unloadFrameSegmentations(frameSeg.startIndex);
}

#include "Stereo.h"
#include "ImageProcessing.h"
#include "ImageIO.h"

Segmentation* Stereo::loadSegmentation(int iCam, int iFrame, bool loadBeliefDataOnly, bool createSegmentationIfAbsent)
{
	CHECK_RANGE(iCam,   0, (int) this->rig.cameras.size() - 1);
	CHECK_RANGE(iFrame, 0, this->rig.frameCount - 1);

	//bug
	if(this->camSegmentations[iCam] != NULL)
		return this->camSegmentations[iCam];
	
	ENSURE(this->camSegmentations[iCam] == NULL);
	this->camSegmentations[iCam] = new Segmentation();
	ENSURE(this->camSegmentations[iCam] != NULL);

	const Camera &cam = this->rig.cameras[iCam];
	printf("Loading cam-%s, frame-%05i context\n", cam.id.c_str(), iFrame);
	fflush(stdout);

	if(this->camSegmentations[iCam]->Load(cam, iFrame, loadBeliefDataOnly) == false)
	{
		if(createSegmentationIfAbsent)
		{
			Segmenter segmenter;
			segmenter.CreateSegmentation(cam, iFrame, 										 
										 *this->camSegmentations[iCam], 
										 this->stereoParams.segmenterParams);
		}
		else
		{
			return this->camSegmentations[iCam];
		}
	}
	else
	{
		this->camSegmentations[iCam]->CheckSegmentationConsistency(
			this->stereoParams.segmenterParams.segBreakerParams.minSegSize,
			this->stereoParams.segmenterParams.segBreakerParams.maxSegSize);
	}

	int viewCount = (int) cam.cameraNeighs.size();
	ENSURE(viewCount > 0);
	if(this->camSegmentations[iCam]->fullyComputed == false)
	{
		ENSURE(this->camSegmentations[iCam]->viewCount  == 0);
		ENSURE(this->camSegmentations[iCam]->planeCount == 0);
		ENSURE(this->camSegmentations[iCam]->scenePlaneCount == 0);
		if(loadBeliefDataOnly == false)
			this->camSegmentations[iCam]->AllocSegmentBuffers(viewCount, cam.planeCount, cam.scenePlaneCount);
	}
	else
	{
		ENSURE(this->camSegmentations[iCam]->planeCount      == cam.planeCount);
		ENSURE(this->camSegmentations[iCam]->scenePlaneCount == cam.scenePlaneCount);
		ENSURE(this->camSegmentations[iCam]->viewCount       == viewCount);
	}

	return this->camSegmentations[iCam];
}

void Stereo::unloadSegmentation(int iCam, int iFrame, bool saveDynamicData)
{
	CHECK_RANGE(iCam,   0, (int) this->rig.cameras.size() - 1);
	CHECK_RANGE(iFrame, 0, this->rig.frameCount - 1);

	if(this->camSegmentations[iCam] != NULL)
	{
		Camera &cam = this->rig.cameras[iCam];
		printf("Unloading cam-%s, frame-%05i context\n", cam.id.c_str(), iFrame); fflush(stdout);		
		if(saveDynamicData)
		{
			this->camSegmentations[iCam]->SaveSegmentsDynamicData(cam, iFrame);
		}
		delete this->camSegmentations[iCam];
		this->camSegmentations[iCam] = NULL;
	}
}


void Stereo::loadFrameSegmentations(int iFrame)
{
	CHECK_RANGE(iFrame, 0, this->rig.frameCount - 1);
	ENSURE(this->camSegmentations == NULL);

	int camCount = (int) this->rig.cameras.size();
	this->camSegmentations = new Segmentation*[camCount];
	ENSURE(this->camSegmentations != NULL);

	for(int iCam = 0; iCam < camCount; iCam++)
	{
		this->camSegmentations[iCam] = NULL;

		if(this->stereoParams.processInMem == true)
			loadSegmentation(iCam, iFrame, false, true);
	}
}

void Stereo::unloadFrameSegmentations(int iFrame)
{
	CHECK_RANGE(iFrame, 0, this->rig.frameCount - 1);

	ENSURE(this->camSegmentations != NULL);
	int camCount = (int) this->rig.cameras.size();
	for(int iCam = 0; iCam < camCount; iCam++)
	{
		unloadSegmentation(iCam, iFrame, true);
	}

	delete [] this->camSegmentations;
	this->camSegmentations = NULL;
}

void Stereo::loadCamFrameContext(int iCam, int iFrame, int iIter, bool createNeighSegmentationsIfAbsent)
{
	ENSURE(this->currView == NULL);
	ENSURE(this->neighViews.size() == 0);
	ENSURE(this->rig.cameras.size() > 1);

	Camera *currCam = &this->rig.cameras[iCam];
	CameraMats refCamMats = currCam->cameraMats;
	Segmentation *currCamSegmentation = NULL;

	if(this->stereoParams.processInMem)
		currCamSegmentation = this->camSegmentations[iCam];		
	else
		currCamSegmentation = loadSegmentation(iCam, iFrame, false, true);		

	this->currView = new View(iFrame, iCam, iIter, 
							  currCam, currCamSegmentation);	
	this->currView->Recenter(refCamMats);

	CFloatImage &currViewImg = this->currView->smoothImg;

	const CameraNeighs &currCamNeighs = currCam->cameraNeighs;
	for(int iNeighCam = 0; iNeighCam < (int) currCamNeighs.size(); iNeighCam++)
	{
		int neighCamIndex = -1;
		Camera *neighCam = this->rig.GetCamera(currCamNeighs[iNeighCam].id, neighCamIndex);
		ENSURE(neighCam != NULL);

		Segmentation *neighCamSegmentation = NULL;
		if(this->stereoParams.processInMem)
			neighCamSegmentation = this->camSegmentations[neighCamIndex];
		else
			neighCamSegmentation = loadSegmentation(neighCamIndex, iFrame, true, createNeighSegmentationsIfAbsent);

		View neighView(iFrame, neighCamIndex, iIter, 
			           neighCam, neighCamSegmentation);

		if(neighCamSegmentation->segments.size() > 0) //neigh segmentation computed
		{
			neighView.smoothImg = ImageProcessing::TransferColor(neighView.smoothImg, currViewImg);
		}

		neighView.Recenter(refCamMats);
		neighView.ComputeAvgDepthDist(this->currView);

		this->neighViews.push_back(neighView);
	}
}

void Stereo::unloadCurrCamFrameContext()
{
	if(this->stereoParams.processInMem == false)
	{
		int currViewCamID = this->currView->iCam;
		if(this->camSegmentations[currViewCamID]->fullyComputed == true)
		{
			unloadSegmentation(currViewCamID, this->currView->iFrame, true);
		}
		else
		{
			unloadSegmentation(currViewCamID, this->currView->iFrame, false);
			for(uint iView = 0; iView < this->neighViews.size(); iView++)
			{
				int neighViewCamID = this->neighViews[iView].iCam;
				ENSURE(this->camSegmentations[neighViewCamID]->fullyComputed == false);
			}
		}
	}

	delete this->currView;
	this->currView = NULL;

	for(uint iView = 0; iView < this->neighViews.size(); iView++)
	{
		if(this->stereoParams.processInMem == false)
		{
			unloadSegmentation(this->neighViews[iView].iCam,
							   this->neighViews[iView].iFrame,
							   false);
		}
	}

	this->neighViews.clear();
}

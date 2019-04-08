#include "Flow.h"
#include "ImageProcessing.h"

Segmentation* Flow::loadSegmentation(int iCam, int iFrame, Segmentation **segmentations)
{
	CHECK_RANGE(iCam,   0, (int) this->rig.cameras.size() - 1);
	CHECK_RANGE(iFrame, 0, this->rig.frameCount - 1);
	
	ENSURE(segmentations[iCam] == NULL);
	segmentations[iCam] = new Segmentation();
	ENSURE(segmentations[iCam] != NULL);

	const Camera &cam = this->rig.cameras[iCam];
	printf("Loading cam-%s, frame-%05i context\n", cam.id.c_str(), iFrame);
	Segmenter segmenter;
	segmenter.CreateSegmentation(cam, iFrame, 
							     *segmentations[iCam], 
								 this->stereoParams.segmenterParams);

	int viewCount = (int) cam.cameraNeighs.size();
	ENSURE(viewCount > 0);
	ENSURE(segmentations[iCam]->fullyComputed == true);
	ENSURE(segmentations[iCam]->depthCount == this->depthCount);
	ENSURE(segmentations[iCam]->viewCount == viewCount);

	return segmentations[iCam];
}

void Flow::unloadSegmentation(int iCam, int iFrame, Segmentation **segmentations)
{
	CHECK_RANGE(iCam, 0, (int) this->rig.cameras.size() - 1);
	CHECK_RANGE(iFrame, 0, this->rig.frameCount - 1);

	if(segmentations[iCam] != NULL)
	{
		Camera &cam = this->rig.cameras[iCam];
		printf("Unloading cam-%s, frame-%05i context\n", cam.id.c_str(), iFrame);
		delete segmentations[iCam];
		segmentations[iCam] = NULL;
	}
}


void Flow::loadFlowSegmentations(int iCurrFrame, int iNextFrame)
{
	ENSURE(iCurrFrame < iNextFrame);
	CHECK_RANGE(iCurrFrame, 0, this->rig.frameCount - 1);
	CHECK_RANGE(iNextFrame, 0, this->rig.frameCount - 1);

	ENSURE(this->currSegmentations == NULL);
	ENSURE(this->nextSegmentations == NULL);
	int camCount = (int) this->rig.cameras.size();
	this->currSegmentations = new Segmentation*[camCount];
	this->nextSegmentations = new Segmentation*[camCount];
	ENSURE(this->currSegmentations != NULL);
	ENSURE(this->nextSegmentations != NULL);

	for(int iCam = 0; iCam < camCount; iCam++)
	{
		this->currSegmentations[iCam] = NULL;
		this->nextSegmentations[iCam] = NULL;
		if(this->flowParams.processInMem == true)
		{
			loadSegmentation(iCam, iCurrFrame, this->currSegmentations, true);
			loadSegmentation(iCam, iNextFrame, this->nextSegmentations, true);
		}
	}
}

void Flow::unloadFlowSegmentations(int iCurrFrame, int iNextFrame)
{
	ENSURE(iCurrFrame < iNextFrame);
	CHECK_RANGE(iCurrFrame, 0, this->rig.frameCount - 1);
	CHECK_RANGE(iNextFrame, 0, this->rig.frameCount - 1);

	ENSURE(this->currSegmentations != NULL);
	ENSURE(this->nextSegmentations != NULL);

	int camCount = (int) this->rig.cameras.size();
	for(int iCam = 0; iCam < camCount; iCam++)
	{
		unloadSegmentation(iCam, iCurrFrame, this->currSegmentations);
		unloadSegmentation(iCam, iNextFrame, this->nextSegmentations);
	}

	delete this->currSegmentations;
	delete this->nextSegmentations;
	this->currSegmentations = NULL;
	this->nextSegmentations = NULL;
}

void Flow::loadCamFlowContext(int iCam, int iCurrFrame, int iNextFrame)
{
	ENSURE(this->currView == NULL);
	ENSURE(this->nextView == NULL);
	ENSURE(this->currViewNeighs.size() == 0);
	ENSURE(this->nextViewNeighs.size() == 0);

	Camera *cam = &this->rig.cameras[iCam];
	const CameraNeighs &camNeighs = cam->cameraNeighs;

	Segmentation *currViewSegmentation = NULL;
	Segmentation *nextViewSegmentation = NULL;

	if(this->flowParams.processInMem)
	{
		currViewSegmentation = this->currSegmentations[iCam];
		nextViewSegmentation = this->nextSegmentations[iCam];
	}
	else
	{
		currViewSegmentation = loadSegmentation(iCam, iCurrFrame, this->currSegmentations);
		nextViewSegmentation = loadSegmentation(iCam, iNextFrame, this->nextSegmentations);
	}

	this->currView = new View(iCurrFrame, iCam, 0, 0.0f,
							  cam, currViewSegmentation);

	this->nextView = new View(iNextFrame, iCam, 0, 0.0f,
							  cam, nextViewSegmentation);

	CFloatImage refHistImg = this->currView->smoothImg;
	this->nextView->smoothImg = ImageProcessing::TransferColor(this->nextView->smoothImg, 
															   refHistImg);

	for(int iNeighView = 0; iNeighView < (int) camNeighs.size(); iNeighView++)
	{
		int neighCamIndex = -1;
		Camera *neighCam = this->rig.GetCamera(camNeighs[iNeighView].id, neighCamIndex);
		ENSURE(neighCam != NULL);

		Segmentation *currNeighSegmentation = NULL;
		Segmentation *nextNeighSegmentation = NULL;

		if(this->flowParams.processInMem)
		{
			currNeighSegmentation = this->currSegmentations[neighCamIndex];
			nextNeighSegmentation = this->nextSegmentations[neighCamIndex];
		}
		else
		{
			currNeighSegmentation = loadSegmentation(neighCamIndex, iCurrFrame, this->currSegmentations);
			nextNeighSegmentation = loadSegmentation(neighCamIndex, iNextFrame, this->nextSegmentations);
		}

		View currNeighView(iCurrFrame, neighCamIndex, 0, camNeighs[iNeighView].dist,
			               neighCam, currNeighSegmentation);
		currNeighView.smoothImg = ImageProcessing::TransferColor(currNeighView.smoothImg, refHistImg);
		this->currViewNeighs.push_back(currNeighView);

		View nextNeighView(iNextFrame, neighCamIndex, 0, camNeighs[iNeighView].dist,
			               neighCam, nextNeighSegmentation);
		nextNeighView.smoothImg = ImageProcessing::TransferColor(nextNeighView.smoothImg, refHistImg);
		this->nextViewNeighs.push_back(nextNeighView);
	}
}



void Flow::unloadCurrCamFlowContext()
{
	ENSURE(this->currView->iCam == this->nextView->iCam);
	ENSURE(this->currView->iFrame < this->nextView->iFrame);
	if(this->flowParams.processInMem == false)
	{
		unloadSegmentation(this->currView->iCam, this->currView->iFrame, this->currSegmentations);
		unloadSegmentation(this->nextView->iCam, this->nextView->iFrame, this->nextSegmentations);
	}
	delete this->currView;
	delete this->nextView;
	this->currView = NULL;
	this->nextView = NULL;

	ENSURE(this->currViewNeighs.size() == this->nextViewNeighs.size());
	for(uint iNeighView = 0; iNeighView < this->currViewNeighs.size(); iNeighView++)
	{
		ENSURE(this->currViewNeighs[iNeighView].iCam   == this->nextViewNeighs[iNeighView].iCam);
		ENSURE(this->currViewNeighs[iNeighView].iFrame < this->nextViewNeighs[iNeighView].iFrame);
	}
	for(uint iNeighView = 0; iNeighView < this->currViewNeighs.size(); iNeighView++)
	{
		if(this->stereoParams.processInMem == false)
		{
			unloadSegmentation(this->currViewNeighs[iNeighView].iCam,
							   this->currViewNeighs[iNeighView].iFrame,
							   this->currSegmentations);

			unloadSegmentation(this->nextViewNeighs[iNeighView].iCam,
							   this->nextViewNeighs[iNeighView].iFrame,
							   this->nextSegmentations);
		}
	}

	this->currViewNeighs.clear();
	this->nextViewNeighs.clear();
}
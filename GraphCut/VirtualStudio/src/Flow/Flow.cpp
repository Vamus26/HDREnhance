#include "Flow.h"
#include "Timer.h"

Flow::Flow()
{
	this->currSegmentations = this->nextSegmentations = NULL;
	this->currView = this->nextView = NULL;
}

Flow::~Flow()
{
	ENSURE(this->currSegmentations == NULL);
	ENSURE(this->nextSegmentations == NULL);
}


void Flow::Init(FlowParams fParams, 
				Stereo::StereoParams sParams,
				Rig::RigParams rigParams)
{
	this->flowParams = fParams;
	this->stereoParams = sParams;
	this->rig.Load(rigParams);

	ENSURE(this->flowParams.searchWinSize > 0);

	//recenter rig with respect to reference cam
	string refCamID = this->stereoParams.refCamID;
	ENSURE(refCamID.size() > 0);
	this->rig.RecenterCameras(refCamID);

	string depthPlanesFN = Stereo::GetFrontoPlanesFN(this->rig, this->stereoParams);
	Stereo::ReadDepthPlanesFile(this->depthPlanes, depthPlanesFN);
	this->depthCount = (int) this->depthPlanes.size();
	ENSURE(this->depthCount > 0);
}


void Flow::ComputeFlow()
{
	printf("Computing Flow...\n");
	FrameSeq frameSeg = this->rig.params.frameSeq;
	ENSURE((frameSeg.endIndex - frameSeg.startIndex) >= 1);

	for(int iFrame = frameSeg.startIndex; iFrame <= frameSeg.endIndex - 1; iFrame++)
	{		
		int iCurrFrame = iFrame;
		int iNextFrame = iFrame + 1;

		printf("Computing flow between from frame-%05i to frame-%05i\n", iCurrFrame, iNextFrame);

		loadFlowSegmentations(iCurrFrame, iNextFrame);
		computeFlow(iCurrFrame, iNextFrame);	
		unloadFlowSegmentations(iCurrFrame, iNextFrame);
	}
}

void Flow::computeFlow(int iCurrFrame, int iNextFrame)
{
	for(uint iCam = 0; iCam < this->rig.cameras.size(); iCam++)
	{
		loadCamFlowContext(iCam, iCurrFrame, iNextFrame);
		printf("\t Processing cam-%s\n", this->rig.cameras[iCam].id.c_str());
		processCurrCamFlow();
		unloadCurrCamFlowContext();
	}
}
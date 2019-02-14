#ifndef __CAM_FRAME_CONTEXT_H__
#define __CAM_FRAME_CONTEXT_H__
#include "Camera.h"
#include "Segmentation.h"

class View
{
public:
	int iFrame;
	int iCam;
	int iIter;
	Camera cam;
	Segmentation *seg;
	CFloatImage smoothImg;

	float avgFrontoPlaneDist;

	View(int frameIndex, int cameraIndex, int iteration,
	     Camera *camera, Segmentation *segmentation);	

	void ComputeAvgDepthDist(const View *neighView);

	void Recenter(CameraMats refMats);
};



#endif

#include "CamFrameContext.h"
#include "ImageIO.h"


View::View(int frameIndex, int cameraIndex, int iteration,
		   Camera *camera, Segmentation *segmentation)
{
	this->iFrame = frameIndex;
	this->iCam   = cameraIndex;
	this->iIter  = iteration;
	this->cam    = *camera;
	this->seg    = segmentation;

	this->avgFrontoPlaneDist = 0.0f;

	this->smoothImg = segmentation->smoothImg.Clone();
}

void View::ComputeAvgDepthDist(const View *neighView)
{
	const vector<double> neighFrontoPlanes = neighView->cam.frontoPlanes;
	ENSURE(neighFrontoPlanes.size() > 1);

	CShape leftImgShape = neighView->smoothImg.Shape();
	const CameraMats &leftCamMats = neighView->cam.cameraMats;
	const CameraMats &rightCamMats = this->cam.cameraMats;

	CVector3 leftUVZ(leftImgShape.width  / 2.0, 
					 leftImgShape.height / 2.0, 
					 neighFrontoPlanes[0]);

	CVector3 worldXYZ;
	leftCamMats.projUVZtoXYZ(leftUVZ, worldXYZ);
	
	CVector2 rightUV;	
	rightCamMats.projXYZtoUV(worldXYZ, rightUV);	

	this->avgFrontoPlaneDist = 0.0f;
	for(uint iFrontoPlane = 1; iFrontoPlane < neighFrontoPlanes.size(); iFrontoPlane++)
	{
		CVector2 prevRighUV = rightUV;
		leftUVZ[2] = neighFrontoPlanes[iFrontoPlane];

		leftCamMats.projUVZtoXYZ(leftUVZ, worldXYZ);
		rightCamMats.projXYZtoUV(worldXYZ, rightUV);

		this->avgFrontoPlaneDist += (float) rightUV.Dist(prevRighUV);
	}

	this->avgFrontoPlaneDist /= (neighFrontoPlanes.size() - 1);
}


void View::Recenter(CameraMats refMats)
{
	this->cam.Recenter(refMats);
}
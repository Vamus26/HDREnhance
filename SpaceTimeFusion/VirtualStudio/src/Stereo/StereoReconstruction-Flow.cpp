#include "StereoReconstruction.h"
#include "FlowHelper.h"

void StereoReconstruction::ComputeFlow(int srcCamIndex, int targetCamIndex, 
									   CFloatImage &flowFieldImg, SR_Params reconstructionParams)
{
	ComputeFlow(srcCamIndex, targetCamIndex, 
				flowFieldImg, reconstructionParams,
				1.0f, CVector2::Null(), CVector2::Null(),
				1.0f, CVector2::Null(), CVector2::Null());
}


void StereoReconstruction::ComputeFlow(int srcCamIndex, int targetCamIndex, 
										 CFloatImage &flowFieldImg, SR_Params reconstructionParams,
										 float srcScale,    CVector2 srcCropOrigin, CVector2 srcCropExt,
										 float targetScale, CVector2 targetCropOrigin, CVector2 targetCropExt)
{
	this->srParams = reconstructionParams;

	loadFrameSegmentations(this->srParams.iFrame);

	INSIST(this->srParams.depthDisConMaskErWidth <= 5);
	
	SRS_Context targetSRS = loadSRS_Context(this->rig.cameras[targetCamIndex].id, true,
											true, false,
											CameraMats(), false,
											targetScale, targetCropOrigin, targetCropExt);
	this->refCamMats = targetSRS.sourceCam.cameraMats;
	targetSRS.sourceCam.Recenter(this->refCamMats);

	SRS_Context srcSRS = loadSRS_Context(this->rig.cameras[srcCamIndex].id, true,
										 true, false,
										 this->refCamMats, true, 
										 srcScale, srcCropOrigin, srcCropExt);

	srcSRS.sourceImg   = ImageProcessing::TransferColor(srcSRS.sourceImg, targetSRS.sourceImg,
													    srcSRS.sourceMask, true,
														targetSRS.sourceMask, true);

	CShape srcShape    = srcSRS.sourceImg.Shape();
	CShape targetShape = targetSRS.sourceImg.Shape();
	float maxColDist   = sqrt((float) srcShape.nBands);
	ENSURE(srcShape.SameIgnoringNBands(flowFieldImg.Shape()));
	ENSURE(flowFieldImg.Shape().nBands == 3);
	ENSURE(srcShape.nBands == targetShape.nBands);

	const CameraMats &srcCMs    = srcSRS.sourceCam.cameraMats;
	const CameraMats &targetCMs = targetSRS.sourceCam.cameraMats;

	ImageProcessing::Operate(flowFieldImg, flowFieldImg, FlowHelper_INVALID_FLOW, ImageProcessing::OP_SET, 0);
	ImageProcessing::Operate(flowFieldImg, flowFieldImg, FlowHelper_INVALID_FLOW, ImageProcessing::OP_SET, 1);
	ImageProcessing::Operate(flowFieldImg, flowFieldImg, 0.0f, ImageProcessing::OP_SET, 2);

	CFloatImage warpImg(targetShape);
	warpImg.ClearPixels();

	uint maskAddr  = 0;
	uint pixelAddr = 0;
	uint flowFieldAddr = 0;
	for(int v = 0; v < targetShape.height; v++)
	for(int u = 0; u < targetShape.width;  u++, maskAddr++, pixelAddr += targetShape.nBands, flowFieldAddr += 3)
	{
		short targetPlaneIndex = targetSRS.sourcePlaneMap[maskAddr];				
		if(targetPlaneIndex == SEGMENT_OccDepPlaneIndex) continue;
		CHECK_RANGE(targetPlaneIndex, 0, targetSRS.sourceCam.planeCount - 1);
		const Plane &targetPlane = targetSRS.sourceCam.planes[targetPlaneIndex];

		CVector2 targetUV((double) u, (double) v);
		CVector3 targetWorldXYZ;
		targetCMs.projUVtoXYZ(targetUV, targetPlane, targetWorldXYZ);
		CVector2 srsUV;
		srcCMs.projXYZtoUV(targetWorldXYZ, srsUV);

		if(srcShape.InBounds(srsUV[0], srsUV[1]))
		{
			int srsU          = NEAREST_INT(srsUV[0]);
			int srsV          = NEAREST_INT(srsUV[1]);
			uint srsMaskAddr  = srcShape.NodeIndex(srsU, srsV);

			if(srcSRS.sourceMask[srsMaskAddr] == MASK_INVALID) continue;			

			int srsPlaneIndex = srcSRS.sourcePlaneMap[srsMaskAddr];
			CHECK_RANGE(srsPlaneIndex, SEGMENT_OccDepPlaneIndex, srcSRS.sourceCam.planeCount - 1);

			bool useSRS = true;
			if((srsPlaneIndex != targetPlaneIndex) ||
			   (srsPlaneIndex >= this->scenePlaneCount))
			{
				int targetFrontoPlaneIndex = getFrontoPlaneIndex(u, v, targetSRS, targetPlaneIndex, false);
				int srsFrontoPlaneIndex    = getFrontoPlaneIndex(srsUV[0], srsUV[1], srcSRS, srsPlaneIndex, 
																 true, targetSRS);
				if((targetFrontoPlaneIndex == SEGMENT_OccDepPlaneIndex) || 
	               (srsFrontoPlaneIndex == SEGMENT_OccDepPlaneIndex)    ||
				   (abs(srsFrontoPlaneIndex - targetFrontoPlaneIndex) > this->srParams.depthMatchThres))
				{
					useSRS = false;
				}
			}

			if(useSRS == true)
			{
				flowFieldImg[flowFieldAddr + 0] = (float) u - (float) srsUV[0];
				flowFieldImg[flowFieldAddr + 1] = (float) v - (float) srsUV[1];
				flowFieldImg[flowFieldAddr + 2] = 0.0f;

				ImageProcessing::MitchellInterlopation(warpImg, pixelAddr, srcSRS.sourceImg, (float) srsUV[0], (float) srsUV[1]);
				for(int iBand = 0; iBand < srcShape.nBands; iBand++)
				{
					float channelDiff = targetSRS.sourceImg[pixelAddr + iBand] - 
						                warpImg[pixelAddr + iBand];
					flowFieldImg[flowFieldAddr + 2] += (channelDiff * channelDiff);
				}
				flowFieldImg[flowFieldAddr + 2] = sqrt(flowFieldImg[flowFieldAddr + 2]);
				flowFieldImg[flowFieldAddr + 2] = 1.0f - (flowFieldImg[flowFieldAddr + 2] / maxColDist);
				flowFieldImg[flowFieldAddr + 2] = max(0.0f, min(flowFieldImg[flowFieldAddr + 2], 1.0f));
			}
		}
	}

	unloadSRS_Context(targetSRS);
	unloadSRS_Context(srcSRS);
	unloadFrameSegmentations(this->srParams.iFrame);
}

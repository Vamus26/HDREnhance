#include "StereoReconstruction.h"

StereoReconstruction::SR_Params StereoReconstruction::SR_Params::Default()
{
	StereoReconstruction::SR_Params params;

	params.iFrame = -1;
	params.pbParams = PiecesBlender::PiecesBlenderParams::Default();
	params.bsParams = BlendSolver::BlendSolverParams::Default();

	params.targetScale = 1.0f;
	params.targetCropOrigin = params.targetCropExt = CVector2(0.0, 0.0);

	params.depthMatchThres = 20;
	params.depthDisConMaskThres = 40;
	params.depthDisConMaskErWidth = 5;

	params.reconstructUsingAnalogy = false;
	params.useAnalogyMask = false;

	params.mattingRad = 3;

	return params;
}

void StereoReconstruction::SR_Params::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sStereo Reconstruction params:\n", prefixPtr);
	printf("%s  iFrame: %i\n", prefixPtr, iFrame);
	printf("%s  Target camera id: %s\n", prefixPtr, targetCamID.c_str());
	printf("%s  Target camera scale: %f\n", prefixPtr, this->targetScale);
	printf("%s  Target camera crop: [%lf, %lf, %lf, %lf]\n", prefixPtr, 
															this->targetCropOrigin[0], this->targetCropOrigin[1],
															this->targetCropExt[0], this->targetCropExt[1]);
	printf("%s  SRS camera ids:\n", prefixPtr);
	ENSURE(this->srsCamIDs.size() == this->srsCamScales.size());
	for(uint iSRS = 0; iSRS < this->srsCamIDs.size(); iSRS++)
	{
		printf("%s  \t%s %f\n", prefixPtr, this->srsCamIDs[iSRS].c_str(), this->srsCamScales[iSRS]);
	}
	
	printf("%s  depthMatchThres:         %i\n", prefixPtr, depthMatchThres);
	printf("%s  depthDisConMaskThres:    %i\n", prefixPtr, depthDisConMaskThres);
	printf("%s  depthDisConMaskErWidth:  %i\n", prefixPtr, depthDisConMaskErWidth);
	printf("%s  reconstructUsingAnalogy: %i\n", prefixPtr, reconstructUsingAnalogy);	
	printf("%s  useAnalogyMask:          %i\n", prefixPtr, useAnalogyMask);	
	ENSURE((reconstructUsingAnalogy == true) || (useAnalogyMask == false));

	printf("%s  mattingRad: %i\n", prefixPtr, mattingRad);	
	
	printf("\n");
	this->pbParams.Dump(prefix + "  ");
	printf("\n");
	this->bsParams.Dump(prefix + "  ");
	printf("\n");
}

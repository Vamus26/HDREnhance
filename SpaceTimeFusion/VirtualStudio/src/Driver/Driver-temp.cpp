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

	int startCamIndex = 1;
	int endCamIndex = (int) stereoReconstructor.rig.cameras.size() - 1;

	if(this->params.rigParams.targetCamIndex != -1)
	{
		CHECK_RANGE(this->params.rigParams.targetCamIndex, 1, (int) stereoReconstructor.rig.cameras.size() - 1);
		startCamIndex = endCamIndex = this->params.rigParams.targetCamIndex;
	}

	for(int iCam = startCamIndex; iCam <= endCamIndex; iCam++)
	{
		ENSURE(stereoReconstructor.rig.IsVideoFrame(iCam) == true);

		StereoReconstruction::SR_Params srParams = this->params.srParams;
		srParams.iFrame = 0;
		srParams.targetCamID = stereoReconstructor.rig.cameras[iCam].id;

		srParams.srsCamIDs.push_back(stereoReconstructor.rig.cameras[0].id);
		srParams.srsCamScales.push_back(1.0f);
		srParams.srsCamFillerOnly.push_back(false);

		ENSURE(srParams.useAnalogyMask == true);
		ENSURE(srParams.reconstructUsingAnalogy == true);

		stereoReconstructor.Reconstruct(srParams, this->params.overwriteResults);
		//stereoReconstructor.ReconstructMask(srParams, this->params.overwriteResults);
	}
}
//*/

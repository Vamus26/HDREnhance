#include "Driver.h"

Driver::Driver(OptFileParser::OptFileParams driverParams) : params(driverParams)
{

}

void Driver::Start()
{
	srand(this->params.seed); 

	switch(this->params.appOP)
	{
		case OptFileParser::OP_EXTRACT_SEQ_DATA:
			extractSeqData();
			break;		

		case OptFileParser::OP_CALIBRATE_RIG_CAMERAS:
			calibrateRigCams();
			break;

		case OptFileParser::OP_OUTPUT_RIG_FRAME:
			outputRigFrame();
			break;

		case OptFileParser::OP_GENERATE_RIG_FMATRICES:
			outputRigFMatrices();
			break;

		case OptFileParser::OP_COMPUTE_STEREO:
			computeStereo();
			break;

		case OptFileParser::OP_BACK_UP_STEREO_BP:
			backUpStereoBP_Data();
			break;

		case OptFileParser::OP_COMPUTE_STEREO_ITER:
			computeStereoIter();
			break;

		case OptFileParser::OP_ADVANCE_STEREO_ITER:
			advanceStereoIter();
			break;

		case OptFileParser::OP_COMPUTE_STEREO_RECONSTRUCTION:
			computeStereoReconstruction();
			break;

		case OptFileParser::OP_COMPUTE_FLOW:
			computeFlow(false);
			break;			

		case OptFileParser::OP_COMPUTE_FLOW_LARGE:
			computeFlow(true);
			break;

		case OptFileParser::OP_COMPUTE_FLOW_CROP:
			computeFlowCrop();
			break;

		case OptFileParser::OP_SETUP_STATIC_SCENE:
			setupStaticScene();
			break;

		case OptFileParser::OP_FEA_COMBINE:
			combineFeatures();
			break;

		case OptFileParser::OP_FEA_RENDER:
			renderFeatures();
			break;

		case OptFileParser::OP_FEA_MATCH:
			matchFeatures();
			break;

		case OptFileParser::OP_FEA_RENDER_MATCHES:
			renderFeatureMatches();
			break;

		case OptFileParser::OP_FEA_COMPUTE_FM:
			computeFeaFM();
			break;

		case OptFileParser::OP_FEA_RENDER_FM:
			renderFM_Inliers();
			break;

		case OptFileParser::OP_RECONSTRUCT_VIDEO_FNF:
			reconstructVideoFNF();
			break;

		case OptFileParser::OP_RECONSTRUCT_VIDEO:
		case OptFileParser::OP_RECONSTRUCT_VIDEO_CR:
		case OptFileParser::OP_RECONSTRUCT_VIDEO_VS:
		case OptFileParser::OP_RECONSTRUCT_VIDEO_CC:
			reconstructVideo();
			break;

		case OptFileParser::OP_GENERIC:
			execGenericOp();
			break;

		case OptFileParser::OP_DEFAULT:
		default:
			REPORT_FAILURE("Unknown app operation - %i\n", this->params.appOP);
			break;
	}
}

void Driver::outputRigFrame()
{
	Rig rig;
	rig.Load(this->params.rigParams);
	ENSURE(this->params.rigParams.frameSeq.startIndex == this->params.rigParams.frameSeq.endIndex);
	rig.OutputFrame(this->params.rigParams.frameSeq.startIndex);
}

void Driver::outputRigFMatrices()
{
	Rig rig;
	rig.Load(this->params.rigParams);
	rig.OutputFMatrices();
}

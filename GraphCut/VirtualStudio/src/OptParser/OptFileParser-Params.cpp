#include "OptFileParser.h"

OptFileParser::OptFileParams OptFileParser::OptFileParams::Default()
{
	OptFileParser::OptFileParams params;
	params.seed = (int) time(NULL);
	params.appOP = OptFileParser::OP_DEFAULT;
	params.overwriteResults = true;

	params.rigParams = Rig::RigParams::Default();
	params.rigCalibratorParams = RigCalibrator::RigCalibratorParams::Default();
	params.stereoParams = Stereo::StereoParams::Default();
	params.srParams = StereoReconstruction::SR_Params::Default();
	params.ssSetupParams = SS_Setup::SS_SetupParams::Default();
	params.feaMatcherParams = FeatureMatcher::FeatureMatcherParams::Default();
	params.mfmParams = MotionFromMatches::MotionFromMatchesParams::Default();
	params.vsParams = VideoSolver::VideoSolverParams::Default();
	
	return params;
}

void OptFileParser::OptFileParams::Dump()
{
	printf("Global Options:\n");
	printf("  Seed: %u\n", seed);
	printf("  overwriteResults: %i\n", overwriteResults);
	printf("  App OP: ");
	switch(appOP)
	{
		
	case OP_OUTPUT_RIG_FRAME:
		printf("OP_OUTPUT_RIG_FRAME\n");
		this->rigParams.Dump("  ");
		break;

	case OP_EXTRACT_SEQ_DATA:
		printf("OP_EXTRACT_SEQ_DATA\n");
		this->rigParams.Dump("  ");
		break;

	case OP_CALIBRATE_RIG_CAMERAS:
		printf("OP_CALIBRATE_RIG_CAMERAS\n");
		this->rigParams.Dump("  ");
		this->rigCalibratorParams.Dump("  ");
		break;

	case OP_GENERATE_RIG_FMATRICES:
		printf("OP_GENERATE_RIG_FMATRICES\n");
		this->rigParams.Dump("  ");
		break;

	case OP_COMPUTE_STEREO_ITER:
		printf("OP_COMPUTE_STEREO_ITER\n");
		this->rigParams.Dump("  ");
		this->stereoParams.Dump("  ");
		break;

	case OP_ADVANCE_STEREO_ITER:
		printf("OP_ADVANCE_STEREO_ITER\n");
		this->rigParams.Dump("  ");
		this->stereoParams.Dump("  ");
		break;

	case OP_COMPUTE_STEREO:
		printf("OP_COMPUTE_STEREO\n");
		this->rigParams.Dump("  ");
		this->stereoParams.Dump("  ");
		break;

	case OP_BACK_UP_STEREO_BP:
		printf("OP_BACK_UP_STEREO_BP\n");
		this->rigParams.Dump("  ");
		break;

	case OP_COMPUTE_STEREO_RECONSTRUCTION:
		printf("OP_COMPUTE_STEREO_RECONSTRUCTION\n");
		this->rigParams.Dump("  ");
		this->stereoParams.Dump("  ");
		this->srParams.Dump("  ");
		break;

	case OP_FEA_COMBINE:
		printf("OP_FEA_COMBINE\n");
		this->genericParams.Dump("  ");
		break;

	case OP_FEA_RENDER:
		printf("OP_FEA_RENDER\n");
		this->genericParams.Dump("  ");
		break;

	case OP_FEA_MATCH:
		printf("OP_FEA_MATCH\n");
		this->genericParams.Dump("  ");
		this->feaMatcherParams.Dump("  ");
		break;

	case OP_FEA_RENDER_MATCHES:
		printf("OP_FEA_RENDER_MATCHES\n");
		this->genericParams.Dump("  ");
		this->feaMatcherParams.Dump("  ");
		break;

	case OP_FEA_RENDER_FM:
		printf("OP_FEA_RENDER_FM\n");
		this->genericParams.Dump("  ");
		this->feaMatcherParams.Dump("  ");
		this->mfmParams.Dump("  ");
		break;

	case OP_FEA_COMPUTE_FM:
		printf("OP_FEA_COMPUTE_FM\n");
		this->genericParams.Dump("  ");
		this->feaMatcherParams.Dump("  ");
		this->mfmParams.Dump("  ");
		break;	

	case OP_SETUP_STATIC_SCENE:
		printf("OP_SETUP_STATIC_SCENE\n");
		this->ssSetupParams.Dump("  ");
		this->rigCalibratorParams.Dump("  ");
		break;

	case OP_COMPUTE_FLOW:
		printf("OP_COMPUTE_FLOW\n");
		this->rigParams.Dump("  ");
		break;

	case OP_COMPUTE_FLOW_LARGE:
		printf("OP_COMPUTE_FLOW_LARGE\n");
		this->rigParams.Dump("  ");
		break;

	case OP_COMPUTE_FLOW_CROP:
		printf("OP_COMPUTE_FLOW_CROP\n");
		this->rigParams.Dump("  ");
		break;

	case OP_GENERIC:
		printf("OP_GENERIC\n");
		this->genericParams.Dump("  ");
		break;

	case OP_RECONSTRUCT_VIDEO:
		printf("OP_RECONSTRUCT_VIDEO\n");
		this->rigParams.Dump("  ");
		this->vsParams.Dump("  ");
		break;

	case OP_RECONSTRUCT_VIDEO_CC:
		printf("OP_RECONSTRUCT_VIDEO_CC\n");		
		this->rigParams.Dump("  ");
		this->vsParams.Dump("  ");
		break;

	case OP_RECONSTRUCT_VIDEO_CR:
		printf("OP_RECONSTRUCT_VIDEO_CR\n");		
		this->rigParams.Dump("  ");
		this->vsParams.Dump("  ");
		break;

	case OP_RECONSTRUCT_VIDEO_VS:
		printf("OP_RECONSTRUCT_VIDEO_VS\n");
		this->rigParams.Dump("  ");
		this->vsParams.Dump("  ");
		break;

	case OP_RECONSTRUCT_VIDEO_FNF:
		printf("OP_RECONSTRUCT_VIDEO_FNF\n");
		this->rigParams.Dump("  ");
		this->vsParams.Dump("  ");
		break;		

	case OP_DEFAULT:
	default:
		REPORT_FAILURE("Unknown app operation - %i\n", appOP);
		break;
	}
	printf("\n");

	fflush(stdout);
}
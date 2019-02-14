#include "tiff.h"

const string CAMERA_ColorDirName     = "color"  + OS_PATH_DELIMIT_STR;
const string CAMERA_SegDirName       = "seg"    + OS_PATH_DELIMIT_STR;
const string CAMERA_DepthsDirName    = "depths" + OS_PATH_DELIMIT_STR;
const string CAMERA_FlowDirName      = "flow"   + OS_PATH_DELIMIT_STR;

const string CAMERA_ImageExt              = ".tif";
const string CAMERA_RawExt                = ".raw";
//const string CAMERA_AnalogySuffix         = "_Mask1";
const string CAMERA_AnalogySuffix         = "_Analogy";
const string CAMERA_MaskSuffix		      = "_Mask";
const string CAMERA_AltSuffix		      = "_Alt";
const string CAMERA_LargeSuffix           = "_Large";
const string CAMERA_CropSuffix            = "_Crop";
const string CAMERA_FramePrefix           = "vid-";
const string CAMERA_ReconPrefix           = "recon-";
const string CAMERA_FlowPrefix            = "flow-";
const string CAMERA_RevFlowPrefix         = "revflow-";
const string CAMERA_FrameSmoothPrefix     = "smooth-";
const string CAMERA_FrameLabelPrefix      = "label-";
const string CAMERA_FrameLabelEdgesPrefix = "labelEdges-";
const string CAMERA_FrameSegPrefix        = "seg-";
const string CAMERA_FrameMsgesPrefix      = "msg-";
const string CAMERA_FramePrevMsgesPrefix  = "prev-msg-";
const string CAMERA_FrameBeliefPrefix     = "belief-";
const string CAMERA_FramePrevBeliefPrefix = "prev-belief-";
const string CAMERA_FrameDepthsPrefix     = "deps-";
const string CAMERA_FrameFlowDC_Prefix    = "flowDC-";
const string CAMERA_TimeStampsFN          = "TimeStamps.txt";
const string CAMERA_CameraMatsFN          = "CamMats.txt";
const string CAMERA_CameraViewPtsFN       = "CamViewPts.txt";
const string CAMERA_CameraNeighsFN        = "CamNeighs.txt";
const string CAMERA_CameraScenePlanesFN   = "ScenePlanes.txt";
const string CAMERA_CameraFrontoPlanesFN  = "FrontoPlanes.txt";

//check
//const int CAMERA_ImageCompression = COMPRESSION_JPEG;
const int CAMERA_ImageCompression = COMPRESSION_NONE;

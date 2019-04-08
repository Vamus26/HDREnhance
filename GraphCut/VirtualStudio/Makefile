OPT-GCC			= -O3 -funroll-loops --fast-math -mtune=pentium4 -mfpmath=sse -msse2 -J4 

OPT-ICC			= -fast -axN

GCC				= g++

ICC				= icpc

EXE1 	  		= vs

DEBUG-EXE1      = vs-debug

LIBS            = -lm               \
				  -lpng             \
				  -lmkl_lapack      \
				  -lguide           \
				  -lpthread         \
				  -ltiff            \
				  -lmkl_ia32        \
				  -llaspack         \
				  -lminpack		  		  
				  

LIBPATH			=  -Llibs/mkl_8.1/linux \
                   -Lsrc/LasPack/lib

CC				= $(GCC)

LFLAGS			= #-ipo	

INCLUDE			=	-Isrc/ImageLib/                      \
					-Isrc/General/                       \
					-Isrc/Disk/                          \
					-Isrc/MeanShiftSeg/prompt            \
					-Isrc/MeanShiftSeg/edge              \
					-Isrc/MeanShiftSeg/segm              \
					-Isrc/Camera/                        \
					-Isrc/Segmentation/                  \
					-Isrc/Rig/                           \
					-Isrc/Stereo/                        \
					-Isrc/Stereo/ScenePoints             \
					-Isrc/Renderer/                      \
					-Isrc/Driver/                        \
					-Isrc/OptParser/                     \
					-Isrc/MotionModel/                   \
					-Isrc/MotionModel/fmatrix            \
					-Isrc/FeatureLib/                    \
					-Isrc/Calibration/                   \
					-Isrc/Stereo/GC_Stitch/              \
					-Isrc/BlackFlow/                     \
					-Isrc/GCFlow/maxflow/                \
					-Isrc/GCFlow/maxflow/adjacency_list/ \
					-Isrc/LasPack/                       \
					-Isrc/Stereo/PoisonBlending/         \
					-Ilibs/mkl_8.1/include
					

CFLAGS 			= $(OPT-GCC) -MMD $(INCLUDE)					

CXX_OBJECTS		= src/MeanShiftSeg/prompt/parser.o                  \
				  src/MeanShiftSeg/prompt/libppm.o                  \
				  src/MeanShiftSeg/prompt/globalFnc.o               \
				  src/MeanShiftSeg/prompt/edison.o                  \
				                                                    \
		          src/MeanShiftSeg/edge/BgEdge.o                    \
				  src/MeanShiftSeg/edge/BgEdgeDetect.o              \
				  src/MeanShiftSeg/edge/BgEdgeList.o                \
				  src/MeanShiftSeg/edge/BgImage.o                   \
				  src/MeanShiftSeg/edge/BgGlobalFc.o                \
				                                                    \
		          src/MeanShiftSeg/segm/ms.o                        \
				  src/MeanShiftSeg/segm/msImageProcessor.o          \
				  src/MeanShiftSeg/segm/msSysPrompt.o               \
				  src/MeanShiftSeg/segm/RAList.o                    \
				  src/MeanShiftSeg/segm/rlist.o                     \
				                                                    \
                  src/BlackFlow/FlowHelper.o                        \
				                                                    \
                  src/GCFlow/maxflow/adjacency_list/graph.o         \
                  src/GCFlow/maxflow/adjacency_list/maxflow.o       \
				                                                    \
				  src/Segmentation/Segmenter.o                      \
				  src/Segmentation/Segmenter-Params.o               \
				  src/Segmentation/SegmentBreaker.o                 \
		          src/Segmentation/SegmentBreaker-Params.o          \
				  src/Segmentation/Segmentation.o                   \
				  src/Segmentation/Segmentation-Debug.o             \
				  src/Segmentation/Segmentation-IO.o                \
		          src/Segmentation/Segment.o                        \
				  src/Segmentation/Segment-IO.o                     \
				  src/Segmentation/Segment-Debug.o                  \
				  src/Segmentation/Segment-SegData.o                \
				  src/Segmentation/Segment-SpatialNeighs.o          \
				  src/Segmentation/Segment-ViewNeighs.o             \
				                                                    \
		          src/Camera/Camera.o                               \
				  src/Camera/Camera-Accessors.o                     \
				  src/Camera/Camera-Checks.o                        \
				  src/Camera/Camera-Planes.o                        \
				  src/Camera/Camera-FileNames.o                     \
				  src/Camera/CameraMats.o                           \
				  src/Camera/CameraMats-Checks.o                    \
				  src/Camera/CameraMats-IO.o                        \
                  src/Camera/CameraMats-Proj.o                      \
				                                                    \
		          src/Rig/Rig.o                                     \
				  src/Rig/Rig-Checks.o                              \
				  src/Rig/Rig-CameraPath.o                          \
				  src/Rig/Rig-Planes.o                              \
				  src/Rig/Rig-FileNames.o                           \
				  src/Rig/Rig-Debug.o		                        \
				  src/Rig/Rig-Params.o                              \
															        \
		          src/Calibration/RigCalibrator.o                   \
				  src/Calibration/RigCalibrator-CamMatrices.o       \
				  src/Calibration/RigCalibrator-FileNames.o         \
				  src/Calibration/RigCalibrator-Params.o            \
				  src/Calibration/RigCalibrator-NoahLarry.o         \
				  src/Calibration/RigCalibrator-PointExtraction.o   \
				                                                    \
				  src/Rig/SS_Setup.o                                \
				  src/Rig/SS_Setup-Params.o                         \
				                                                    \
		          src/Stereo/CamFrameContext.o                      \
				  src/Stereo/Stereo.o                               \
				  src/Stereo/Stereo-BP.o                            \
				  src/Stereo/Stereo-BP-Init.o                       \
				  src/Stereo/Stereo-Debug.o                         \
				  src/Stereo/Stereo-FileNames.o                     \
				  src/Stereo/Stereo-Init.o                          \
				  src/Stereo/Stereo-Load.o                          \
				  src/Stereo/Stereo-Params.o                        \
				  src/Stereo/Stereo-StaticData.o                    \
				  src/Stereo/StereoReconstruction-FileNames.o       \
				  src/Stereo/StereoReconstruction-Params.o          \
				  src/Stereo/StereoReconstruction-IO.o              \
				  src/Stereo/StereoReconstruction-NVI.o             \
				  src/Stereo/StereoReconstruction-Flow.o            \
				  src/Stereo/StereoReconstruction-Matte.o           \
				  src/Stereo/StereoReconstruction.o                 \
				                                                    \
				  src/Stereo/GC_Stitch/RegistrationDefs.o           \
				  src/Stereo/GC_Stitch/PiecesBlender.o              \
				  src/Stereo/GC_Stitch/CCLabeler.o                  \
				                                                    \
				  src/Stereo/PoisonBlending/BlendSolver.o           \
				  src/Stereo/PoisonBlending/SparseSolver.o          \
				  src/Stereo/PoisonBlending/VideoSolver.o           \
				  src/Stereo/PoisonBlending/VideoSolver-FileNames.o \
				  src/Stereo/PoisonBlending/VideoSolver-Color.o     \
				                                                    \
		          src/Stereo/ScenePoints/PointCloud.o               \
		          src/Stereo/ScenePoints/ScenePoints.o              \
		          src/Stereo/ScenePoints/PCloudSegmenter.o          \
		          src/Stereo/ScenePoints/PlaneFromPoints.o          \
		          src/Stereo/ScenePoints/Plane.o                    \
																	\
  	              src/ImageLib/Image.o                              \
				  src/ImageLib/ImageIO.o                            \
				  src/ImageLib/ImageIO-Tiff.o                       \
				  src/ImageLib/RefCntMem.o                          \
				  src/ImageLib/ImageProcessing.o                    \
				  src/ImageLib/ImageProcessing-Demosiac.o           \
				  src/ImageLib/ImageProcessing-DirSmooth.o          \
				  src/ImageLib/ImageProcessing-DHW.o                \
				                                                    \
				  src/Disk/Disk.o                                   \
                                                                    \
				  src/Renderer/Draw.o                               \
                                                                    \
				  src/OptParser/OptFileParser.o                     \
				  src/OptParser/OptFileParser-Params.o              \
				  src/Driver/Driver-Preprocess.o                    \
				  src/Driver/Driver-Stereo.o                        \
				  src/Driver/Driver-Flow.o                          \
				  src/Driver/Driver-Features.o                      \
				  src/Driver/Driver-Features-Debug.o                \
				  src/Driver/Driver-Generic.o                       \
				  src/Driver/Driver.o                               \
                                                                    \
				  src/MotionModel/horn.o                            \
				  src/MotionModel/Motion.o                          \
				  src/MotionModel/MotionFromMatches.o               \
				  src/MotionModel/PiecewiseMotion.o                 \
                                                                    \
				  src/FeatureLib/FeatureMatcher.o                   \
				  src/FeatureLib/Features.o                         \
				  src/FeatureLib/FeatureRenderer.o                  \
                                                                    \
                  src/General/Timer.o                               \
			      src/General/Debug.o                               \
			      src/General/ColorHelper.o                         \
			      src/General/Definitions.o                         \
			      src/General/MathHelper.o                          \
			      src/General/Transform-4x4.o                       \
			      src/General/Transform-2x2.o                       \
			      src/main.o
			      

C_OBJECTS		= src/MotionModel/fmatrix/matrix.o                  \
                  src/MotionModel/fmatrix/vector.o                  \
                  src/MotionModel/fmatrix/fmatrix.o                 \
				  src/MotionModel/fmatrix/triangulate.o             \
                                                                    \
                  src/BlackFlow/derivatives.o                       \
				  src/BlackFlow/disconts.o                          \
				  src/BlackFlow/filter.o                            \
				  src/BlackFlow/image_stats.o                       \
				  src/BlackFlow/image-io.o                          \
				  src/BlackFlow/pyramid.o                           \
				  src/BlackFlow/pyramid-sor.o                       \
				  src/BlackFlow/sor.o                               \
				  src/BlackFlow/utils.o                             \
				  src/BlackFlow/warp.o                              \
				  src/BlackFlow/warp-back.o

                  
                  
                  
CXX_SRC_FILES	= src/MeanShiftSeg/prompt/parser.cpp                  \
				  src/MeanShiftSeg/prompt/libppm.cpp                  \
				  src/MeanShiftSeg/prompt/globalFnc.cpp               \
				  src/MeanShiftSeg/prompt/edison.cpp                  \
				                                                      \
		          src/MeanShiftSeg/edge/BgEdge.cpp                    \
				  src/MeanShiftSeg/edge/BgEdgeDetect.cpp              \
				  src/MeanShiftSeg/edge/BgEdgeList.cpp                \
				  src/MeanShiftSeg/edge/BgImage.cpp                   \
				  src/MeanShiftSeg/edge/BgGlobalFc.cpp                \
				                                                      \
		          src/MeanShiftSeg/segm/ms.cpp                        \
				  src/MeanShiftSeg/segm/msImageProcessor.cpp          \
				  src/MeanShiftSeg/segm/msSysPrompt.cpp               \
				  src/MeanShiftSeg/segm/RAList.cpp                    \
				  src/MeanShiftSeg/segm/rlist.cpp                     \
                                                                      \
                  src/BlackFlow/FlowHelper.cpp                        \
				                                                      \
                  src/GCFlow/maxflow/adjacency_list/graph.o           \
                  src/GCFlow/maxflow/adjacency_list/maxflow.o         \
				                                                      \
				  src/Segmentation/Segmenter.cpp                      \
				  src/Segmentation/Segmenter-Params.cpp               \
				  src/Segmentation/SegmentBreaker.cpp                 \
		          src/Segmentation/SegmentBreaker-Params.cpp          \
				  src/Segmentation/Segmentation.cpp                   \
				  src/Segmentation/Segmentation-Debug.cpp             \
				  src/Segmentation/Segmentation-IO.cpp                \
		          src/Segmentation/Segment.cpp                        \
				  src/Segmentation/Segment-IO.cpp                     \
				  src/Segmentation/Segment-Debug.cpp                  \
				  src/Segmentation/Segment-SegData.cpp                \
				  src/Segmentation/Segment-SpatialNeighs.cpp          \
				  src/Segmentation/Segment-ViewNeighs.cpp             \
				                                                      \
		          src/Camera/Camera.cpp                               \
				  src/Camera/Camera-Accessors.cpp                     \
				  src/Camera/Camera-Checks.cpp                        \
				  src/Camera/Camera-Planes.cpp						  \
				  src/Camera/Camera-FileNames.cpp                     \
				  src/Camera/CameraMats.cpp                           \
				  src/Camera/CameraMats-Checks.cpp                    \
				  src/Camera/CameraMats-IO.cpp                        \
                  src/Camera/CameraMats-Proj.cpp                      \
				                                                      \
		          src/Rig/Rig.cpp                                     \
				  src/Rig/Rig-Checks.cpp                              \
				  src/Rig/Rig-CameraPath.cpp                          \
				  src/Rig/Rig-Planes.cpp                              \
				  src/Rig/Rig-FileNames.cpp                           \
				  src/Rig/Rig-Debug.cpp                               \
				  src/Rig/Rig-Params.cpp                              \
															          \
		          src/Calibration/RigCalibrator.cpp                   \
				  src/Calibration/RigCalibrator-CamMatrices.cpp       \
				  src/Calibration/RigCalibrator-FileNames.cpp         \
				  src/Calibration/RigCalibrator-Params.cpp            \
				  src/Calibration/RigCalibrator-NoahLarry.cpp         \
				  src/Calibration/RigCalibrator-PointExtraction.cpp   \
				                                                      \
				  src/Rig/SS_Setup.cpp                                \
				  src/Rig/SS_Setup-Params.cpp                         \
				                                                      \
		          src/Stereo/CamFrameContext.cpp                      \
				  src/Stereo/Stereo.cpp                               \
				  src/Stereo/Stereo-BP.cpp                            \
				  src/Stereo/Stereo-BP-Init.cpp                       \
				  src/Stereo/Stereo-Debug.cpp                         \
				  src/Stereo/Stereo-FileNames.cpp                     \
				  src/Stereo/Stereo-Init.cpp                          \
				  src/Stereo/Stereo-Load.cpp                          \
				  src/Stereo/Stereo-Params.cpp                        \
				  src/Stereo/Stereo-StaticData.cpp                    \
				  src/Stereo/StereoReconstruction-FileNames.cpp       \
				  src/Stereo/StereoReconstruction-Params.cpp          \
				  src/Stereo/StereoReconstruction-IO.cpp              \
				  src/Stereo/StereoReconstruction-NVI.cpp             \
				  src/Stereo/StereoReconstruction-Flow.cpp            \
				  src/Stereo/StereoReconstruction-Matte.o             \
				  src/Stereo/StereoReconstruction.cpp                 \
				                                                      \
				  src/Stereo/GC_Stitch/RegistrationDefs.cpp           \
				  src/Stereo/GC_Stitch/PiecesBlender.cpp              \
				  src/Stereo/GC_Stitch/CCLabeler.cpp                  \
				                                                      \
				  src/Stereo/PoisonBlending/BlendSolver.cpp           \
				  src/Stereo/PoisonBlending/SparseSolver.cpp          \
				  src/Stereo/PoisonBlending/VideoSolver.cpp           \
				  src/Stereo/PoisonBlending/VideoSolver-FileNames.cpp \
				  src/Stereo/PoisonBlending/VideoSolver-Color.cpp     \
				                                                      \
		          src/Stereo/ScenePoints/PointCloud.cpp               \
		          src/Stereo/ScenePoints/ScenePoints.cpp              \
		          src/Stereo/ScenePoints/PCloudSegmenter.cpp          \
		          src/Stereo/ScenePoints/PlaneFromPoints.cpp          \
		          src/Stereo/ScenePoints/Plane.cpp                    \
																	  \
  	              src/ImageLib/Image.cpp                              \
				  src/ImageLib/ImageIO.cpp                            \
				  src/ImageLib/ImageIO-Tiff.cpp                       \
				  src/ImageLib/RefCntMem.cpp                          \
				  src/ImageLib/ImageProcessing.cpp                    \
				  src/ImageLib/ImageProcessing-Demosiac.cpp           \
				  src/ImageLib/ImageProcessing-DirSmooth.cpp          \
				  src/ImageLib/ImageProcessing-DHW.cpp                \
				                                                      \
				  src/Disk/Disk.cpp                                   \
                                                                      \
				  src/Renderer/Draw.cpp                               \
                                                                      \
				  src/OptParser/OptFileParser.cpp                     \
				  src/OptParser/OptFileParser-Params.cpp              \
				  src/Driver/Driver-Preprocess.cpp                    \
				  src/Driver/Driver-Stereo.cpp                        \
				  src/Driver/Driver-Flow.cpp                          \
				  src/Driver/Driver-Features.cpp                      \
				  src/Driver/Driver-Features-Debug.cpp                \
				  src/Driver/Driver-Generic.cpp                       \
				  src/Driver/Driver.cpp                               \
                                                                      \
				  src/MotionModel/horn.cpp                            \
				  src/MotionModel/Motion.cpp                          \
				  src/MotionModel/MotionFromMatches.cpp               \
				  src/MotionModel/PiecewiseMotion.cpp                 \
                                                                      \
				  src/FeatureLib/FeatureMatcher.cpp                   \
				  src/FeatureLib/Features.cpp                         \
				  src/FeatureLib/FeatureRenderer.cpp                  \
                                                                      \
                  src/General/Timer.cpp                               \
			      src/General/Debug.cpp                               \
			      src/General/ColorHelper.cpp                         \
			      src/General/Definitions.cpp                         \
			      src/General/MathHelper.cpp                          \
			      src/General/Transform-4x4.cpp                       \
			      src/General/Transform-2x2.cpp                       \
			      src/main.cpp                                        

C_SRC_FILES		= src/MotionModel/fmatrix/matrix.c                  \
                  src/MotionModel/fmatrix/fmatrix.c                 \
                  src/MotionModel/fmatrix/vector.c                  \
                  src/MotionModel/fmatrix/triangulate.c             \
                                                                    \
                  src/BlackFlow/derivatives.c                       \
				  src/BlackFlow/disconts.c                          \
				  src/BlackFlow/filter.c                            \
				  src/BlackFlow/image_stats.c                       \
				  src/BlackFlow/image-io.c                          \
				  src/BlackFlow/pyramid.c                           \
				  src/BlackFlow/pyramid-sor.c                       \
				  src/BlackFlow/sor.c                               \
				  src/BlackFlow/utils.c                             \
				  src/BlackFlow/warp.c                              \
				  src/BlackFlow/warp-back.c
                  
                  
LOCAL_SCRATCH_DIR = /scratch/pro/vs

default: $(EXE1)

# ------------------ Dependencies
.DELETE_ON_ERROR:
-include $(CXX_OBJECTS:.o=.d)

# ------------------ Compilation rules

# $(CXX_OBJECTS):
# 	touch $@
#	$(CC) -c $(CFLAGS) -o $@ $(@:.o=.cpp)  

.cpp.o:
	$(GCC) -c $(CFLAGS) -o $@ $(@:.o=.cpp)

# $(C_OBJECTS):
#	touch $@
#	gcc -c $(CFLAGS) -o $@ $(@:.o=.c)

.c.o:
	gcc -c $(CFLAGS) -o $@ $(@:.o=.c)

$(EXE1): $(CXX_OBJECTS) $(C_OBJECTS)
	rm -f $(EXE1)
	mkdir -p $(LOCAL_SCRATCH_DIR)
	$(CC)  -o $(LOCAL_SCRATCH_DIR)/$(EXE1) $(CXX_OBJECTS) $(C_OBJECTS) $(LFLAGS) $(LIBPATH) $(LIBS)
	mv $(LOCAL_SCRATCH_DIR)/$(EXE1) $(EXE1) 
   
$(DEBUG-EXE1): $(CXX_OBJECTS) $(C_OBJECTS)
	rm -f $(DEBUG-EXE1)
	mkdir -p $(LOCAL_SCRATCH_DIR)
	$(CC)  -ggdb -o $(LOCAL_SCRATCH_DIR)/$(DEBUG-EXE1) $(CXX_OBJECTS) $(C_OBJECTS) $(LFLAGS) $(LIBPATH) $(LIBS) 
	mv $(LOCAL_SCRATCH_DIR)/$(DEBUG-EXE1) $(DEBUG-EXE1) 

clean:
	find . -iname '*.o' -or -iname '*.d' | xargs rm -fv
	rm -fv $(EXE1) $(DEBUG-EXE1)

ba_helper: $(EXE1)
	cp $(EXE1) /projects/grail54/pro/PIR/data/bin/ba_helper
	
depend: $(CXX_SRC_FILES) $(C_SRC_FILES)
	makedepend $CFLAGS $(CXX_SRC_FILES) $(C_SRC_FILES)

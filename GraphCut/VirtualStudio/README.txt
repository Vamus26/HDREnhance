This is a code dump for the "Using Photographs to Enhance Videos of a Static Scene" project.

This code is being released under the GNU General Public License (GPL).

This code dump contains the code for our multiview stereo algorithm, our renderer (graphcut stitching + spacetime fusion), and the various apps shown in the demo video.

This code dump does not contain Noah Snavely's 'Structure From Motion (SFM)' code which is used to figure out the camera parameters for the video frames and the photographs.

This code dump compiles on Windows and Linux.

This code dump contains a lot of unused code. If you open the project in visual studio the unused code is neatly stacked under the "Unused" folder.


DISCLAIMER:

This code dump is not meant to be a usable demo. For one is it missing Noah's SFM code. And it is undocumented research quality code which I dont have a lot of time to support.

This code dump might be useful for:
- A reference for implementing the algorithms described in the paper.
- Some of the components like the graphcut based stitcher, spacetime fusion and the multiview stereo algorithms could be salvaged from this code, in that order of difficulty.

Before you delve in, know the limitations of the code/project:
- It assumes the scene is static - that is no objects in the scene are moving. The camera is allowed to move and the scene lighting may change.
- In it's current unoptimized state the code takes about 5 minutes to process a single 800x600 resolution video frame (from start to finish). We use a server frame to run various portions of the pipeline in parallel.

Before you delve in, know the alternatives to this project, which have their own pros and cons:
- VideoTrace: Rapid interactive scene modelling from video
  http://www.acvt.com.au/research/videotrace/

- Unwrap Mosaics: A new representation for video editing
  http://research.microsoft.com/unwrap/


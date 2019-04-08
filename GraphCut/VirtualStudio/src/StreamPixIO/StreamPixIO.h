#ifndef __STREAM_PIX_IO_H__
#define __STREAM_PIX_IO_H__

#include "Definitions.h"
#include "StreamPixDefs.h"
#include "Camera.h"

class StreamPixIO
{
public:
	static void ExtractSeqData(Camera &camera, FrameSeq frameSeq, int frameCompression);

	static int GetFramesInSeqFile(Camera &camera);

private:
	static FILE *getSeqFilePtr(string path);

	static CSequenceHeader getSeqHeader(FILE* seqFilePtr);

	static void extractFrames(Camera &camera, FrameSeq frameSeq, int frameCompression,
							  const CSequenceHeader &seqHeader, FILE* seqFilePtr);

	static void extractTimeStamps(Camera &camera, FrameSeq frameSeq,
								  const CSequenceHeader &seqHeader, FILE* seqFilePtr);

	static void extractFrameTS(int iFrame, const CSequenceHeader &seqHeader, FILE* seqFilePtr,
							   time_t *timeStampPtr, unsigned short *timeStampMS_Ptr);

	static void writeFrameTS(AccurateTime timeStamp, FILE *tsFilePtr);
};

#endif
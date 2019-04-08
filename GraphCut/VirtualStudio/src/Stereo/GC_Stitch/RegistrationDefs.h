#ifndef __REGISTRATION_DEFS_H__
#define __REGISTRATION_DEFS_H__

#include "Definitions.h"
#include "FeatureDefs.h"
#include "Image.h"
#include "ImageIO.h"
#include "Debug.h"
#include <fstream>
#include "ImageProcessing.h"
#include "GC_Stitch_Defs.h"

typedef std::pair<uint, uint> Coord;

class RegisteredPiece
{
public:
	int id;
	CFloatImage   image;
	CByteImage    mask;
	vector<Coord> registeredPts;
	CFloatImage   filteredImage;
	CFloatImage   sobelImageHE;
	CFloatImage   sobelImageVE;
	CFloatImage   sobelImagePDE;
	CFloatImage   sobelImageNDE;
	CFloatImage   colDistSqrImg;
	CFloatImage   disparityMap;
	CShortImage   planeMap;

	RegisteredPiece()
	{
		this->id = -1;
	}

	void ComputeEdgeImages();
	void ComputeColDistSqrImage(const CFloatImage &targetImg, int windowSize, 
								const CByteImage &targetMask = CByteImage(), bool useTargetMask = false);
};

class RegisteredPieces : public vector<RegisteredPiece>
{
private:
	
public:
	static void WriteRegData(const char *fileName, const RegisteredPiece &piece);
	static void ReadRegData(const char *fileName, RegisteredPiece &piece);

	static void WriteRegisteredPieces(RegisteredPieces &registeredPieces, const char *path, bool writeFast = false, int iStartPiece = 0, bool borrowTag = false);
	static void WriteRegisteredPieces(RegisteredPieces &registeredPieces, string path, bool writeFast = false, int iStartPiece = 0, bool borrowTag = false)
	{
		WriteRegisteredPieces(registeredPieces, path.c_str(), writeFast, iStartPiece, borrowTag);
	}

	static void AppendRegisteredPieces(RegisteredPieces &registeredPieces, const char *path, bool writeFast = false);
	static void AppendRegisteredPieces(RegisteredPieces &registeredPieces, string path, bool writeFast = false)
	{
		AppendRegisteredPieces(registeredPieces, path.c_str(), writeFast);
	}

	static RegisteredPieces ReadRegisteredPieces(const CFloatImage &targetImage, int dataCostWinSize, const char *path, bool readFast = false);
	static RegisteredPieces ReadRegisteredPieces(const CFloatImage &targetImage, int dataCostWinSize, string path,      bool readFast = false)
	{
		return ReadRegisteredPieces(targetImage, dataCostWinSize, path.c_str(), readFast);
	}

	static CByteImage ReadRegPiecesSpaceMask(const char *path, bool readFast = false);
	static CByteImage ReadRegPiecesSpaceMask(string path, bool readFast = false)
	{
		return ReadRegPiecesSpaceMask(path.c_str(), readFast);
	}	
};

#endif

#ifndef __IMAGE_PROCESSSING__
#define __IMAGE_PROCESSSING__

#pragma warning(disable : 4239) 

#include "Image.h"
#include "Debug.h"
#include "Transform-3x3.h"
#include "ColorHelper.h"

class ImageProcessing
{	
	//CHECK - Make the sobel kernel images static
	static CFloatImage getSobelHE()
	{
		CShape sobelKernelShape(3, 3, 1);

		CFloatImage sKernel(sobelKernelShape);
		sKernel.origin[0] = -1;
		sKernel.origin[1] = -1;

		sKernel.Pixel(0, 0, 0) = -0.25f;
		sKernel.Pixel(0, 1, 0) =  0;
		sKernel.Pixel(0, 2, 0) =  0.25f;
		sKernel.Pixel(1, 0, 0) = -0.5f;
		sKernel.Pixel(1, 1, 0) =  0;
		sKernel.Pixel(1, 2, 0) =  0.5f;
		sKernel.Pixel(2, 0, 0) = -0.25f;
		sKernel.Pixel(2, 1, 0) =  0;
		sKernel.Pixel(2, 2, 0) =  0.25f;

		return sKernel;
	}

	static CFloatImage getSobelDiagP()
	{
		CShape sobelKernelShape(3, 3, 1);

		CFloatImage sKernel(sobelKernelShape);
		sKernel.origin[0] = -1;
		sKernel.origin[1] = -1;

		sKernel.Pixel(0, 0, 0) = -0.50f;
		sKernel.Pixel(1, 0, 0) = -0.25f;
		sKernel.Pixel(2, 0, 0) =  0;
		sKernel.Pixel(0, 1, 0) = -0.25f;
		sKernel.Pixel(1, 1, 0) =  0;
		sKernel.Pixel(2, 1, 0) =  0.25f;
		sKernel.Pixel(0, 2, 0) =  0;
		sKernel.Pixel(1, 2, 0) =  0.25f;
		sKernel.Pixel(2, 2, 0) =  0.50f;

		return sKernel;
	}


	static CFloatImage getSobelDiagN()
	{
		CShape sobelKernelShape(3, 3, 1);

		CFloatImage sKernel(sobelKernelShape);
		sKernel.origin[0] = -1;
		sKernel.origin[1] = -1;

		sKernel.Pixel(0, 0, 0) =  0;
		sKernel.Pixel(1, 0, 0) = -0.25f;
		sKernel.Pixel(2, 0, 0) = -0.50f;
		sKernel.Pixel(0, 1, 0) =  0.25f;
		sKernel.Pixel(1, 1, 0) =  0;
		sKernel.Pixel(2, 1, 0) = -0.25f;
		sKernel.Pixel(0, 2, 0) =  0.50f;
		sKernel.Pixel(1, 2, 0) =  0.25f;
		sKernel.Pixel(2, 2, 0) =  0;

		return sKernel;
	}

	static CFloatImage getSobelVE()
	{
		CShape sobelKernelShape(3, 3, 1);

		CFloatImage sKernel(sobelKernelShape);
		sKernel.origin[0] = -1;
		sKernel.origin[1] = -1;

		sKernel.Pixel(0, 0, 0) =  0.25f;
		sKernel.Pixel(1, 0, 0) =  0;
		sKernel.Pixel(2, 0, 0) = -0.25f;
		sKernel.Pixel(0, 1, 0) =  0.50f;
		sKernel.Pixel(1, 1, 0) =  0;
		sKernel.Pixel(2, 1, 0) = -0.50f;
		sKernel.Pixel(0, 2, 0) =  0.25f;
		sKernel.Pixel(1, 2, 0) =  0;
		sKernel.Pixel(2, 2, 0) = -0.25f;

		return sKernel;
	}

public:
	typedef enum
	{
		OP_ADD,
		OP_POW,
		OP_SUB,
		OP_MUL,
		OP_DIV,
		OP_ABS,
		OP_SET
	} ImageOperation;


	static CFloatImage GetCompositeImage(const CFloatImage &orgImg, 
									     const CFloatImage &imgToComposite, 
										 const CFloatImage &compositeMask);

	template <class T>
	static void ComputeMedian(const vector< CImageOf<T> > &srcImgs, const vector<CByteImage> &srcMasks, 
							  CImageOf<T> &medianImg, CByteImage &medianMask, int maxSamples = INT_MAX);

	template <class T>
	static CImageOf<T> GetMaskedImage(const CImageOf<T> &img, const CByteImage &imgMask);

	static CByteImage GetInvertedMask(const CByteImage &mask);

	static CByteImage  FloatToByteImg(const CFloatImage &floatImg);
	static CFloatImage ByteToFloatImg(const CByteImage &byteImg);

	static void GetEdgeImgs(const CFloatImage &image, CFloatImage &horzEdges, CFloatImage &vertEdges);

	static void DirectionalSmoothing(CByteImage &image);

	template <class T>
	static CImageOf<T> FlipVertical(CImageOf<T> &img);

	static CFloatImage GetDistImage(const CByteImage &mask);

	template <class T>
	static CImageOf<T> CropImage(const CImageOf<T> &srcImg, int x, int y, int width, int height);

	template <class T>
	static CImageOf<T> CropImage(const CImageOf<T> &srcImg, const CByteImage &srcMask, bool &cropViolatesMask, 
							     int srcX, int srcY, int cropWidth, int cropHeight);

	template <class T>
	static CImageOf<T> CropImageUsingReflection(const CImageOf<T> &srcImg, int x, int y, int width, int height);

	static void ScaleImage(CFloatImage &dst, CFloatImage &src, float scale, bool blurBeforeScaling = true, float gaussianStdDev = 0.5f);

	template <class T>
	static CImageOf<T> ScaleImageWithoutSampling(const CImageOf<T> &src, float scale);

	static uint TransformImage(CFloatImage &dst, CByteImage &mask,
							   CTransform3x3 homography, CFloatImage &source, 
							   const CByteImage &sourceMask = CByteImage(), bool useSourceMask = false);

	static float BilinearIterpolation(const CFloatImage &img, float x, float y, int band, bool reflectCoords = true);

	static void BilinearIterpolation(CFloatImage &dstImg, uint dstPixelAddr, const CFloatImage &srcImg, float x, float y, bool reflectCoords = true);

	template <class T>
	static float MitchellInterlopation(CImageOf<T>  &img, float x, float y, int band, const CByteImage &mask = CByteImage(), bool useMask = false);

	template <class T>
	static void MitchellInterlopation(CImageOf<T>  &dstImg, uint dstPixelAddr, CImageOf<T>  &srcImg, float x, float y);

	static void RemapMeanAndDev(CFloatImage &src, int band, float newMean, float newStdDev, const CByteImage &mask = CByteImage(), bool useMask = false);

	static void Rescale(CFloatImage &image, float newMin, float newMax);

	static void GetMinAndMax(CFloatImage &src, int band, float &min, float &max, const CByteImage &mask = CByteImage(), bool useMask = false);

	static float GetMean(CFloatImage &src, int band, const CByteImage &mask = CByteImage(), bool useMask = false);

	static void GetMeanAndDev(CFloatImage &src, int band, float &mean, float &stdDev, const CByteImage &mask = CByteImage(), bool useMask = false);

	static CFloatImage TransferColor(const CFloatImage &target, const CFloatImage &src, 									   
									   CByteImage targetMask = CByteImage(), bool useTargetMask = false,
									   CByteImage srcMask = CByteImage(),    bool useSrcMask = false);

	static CFloatImage RemoveHue(CFloatImage &src);

	static CByteImage getNullMask(const CFloatImage &img, const CByteImage &mask = CByteImage(), bool useMask = false);

	static void Convolution(CFloatImage &dst, CFloatImage &src, CFloatImage &kernel, const CByteImage &mask = CByteImage(), bool useMask = false);	

	static void GetImageSobel(CFloatImage &dst, CFloatImage &img, CFloatImage &kernel, const CByteImage &mask = CByteImage(), bool useMask = false);

	static void GetImagesSobel(CFloatImage &hEdges, CFloatImage &vEdges, 
		                       CFloatImage &img, const CByteImage &mask = CByteImage(), bool useMask = false, int erosionWidth = 0);

	static void GetImagesSobel(CFloatImage &hEdges, CFloatImage &vEdges, 
							   CFloatImage &pdEdges, CFloatImage &ndEdges, 
							   CFloatImage &img, const CByteImage &mask, bool useMask = false, int erosionWidth = 0);

	template <class T>
	static CImageOf<T> ErodeImage(const CImageOf<T> &mask, int erosionWidth, T erodeVal);

	static CFloatImage GetFilteredImage(CFloatImage &image,     CByteImage &imageMask,  bool useImgMask, 
										CFloatImage &sourceImg, CByteImage &sourceMask, bool useSrcMask,
										ImageFilter filterType);

	static CFloatImage GetLabelsImage(CImageOf<short> &labels);
	
	static CFloatImage GetSourcesImage(CImageOf<short> &labels, int sourceCount, 
									   const vector<CVector3> &srcColors = vector<CVector3>(), bool useSrcColors = false);	

	static double L2Diff(CFloatImage &image1, CFloatImage &image2, const CByteImage &mask = CByteImage(), bool useMask = false);

	static CFloatImage GaussianBlur(CFloatImage &src, int kernelSize, float stdDev, const CByteImage &mask = CByteImage(), bool useMask = false);
		
	static void PrintImageStats(CFloatImage &src, const CByteImage &mask = CByteImage(), bool useMask = false);

	static CFloatImage GetContrastImage(CFloatImage &src, const CByteImage &mask = CByteImage(), bool useMask = false);

	static CFloatImage CreateCompositeFromDisparity(vector<CFloatImage> &sourceImages,
													CImageOf<short> &disparitySources,
												    CFloatImage &compositeDisparity);

	static CFloatImage CreateCompositeFromDisparity(CFloatImage &sourceImage,
													CFloatImage &compositeDisparity);

	template <class T>
	static void CopyChannel(CImageOf<T> &dst, CImageOf<T> &src, 
						    int dstChannel, int srcChannel);

	template <class T>
	static void Operate(CImageOf<T> &dst, const CImageOf<T> &image1, T val, ImageOperation op, int band = -1, const CByteImage &mask = CByteImage(), bool useMask = false);

	template <class T>
	static void Operate(CImageOf<T> &dst, const CImageOf<T> &image1, const CImageOf<T> &image2, ImageOperation op, int band = -1, const CByteImage &mask = CByteImage(), bool useMask = false);

	static CFloatImage BilateralFilter(CFloatImage &img, float spaceD, float sigD, int halfWidth);

	static CFloatImage CrossBilateralFilter(CFloatImage &noFlash, 
											 CFloatImage &flash, 
											 CFloatImage &detailImg, 
											 float spaceD, float sigD, int halfWidth);

public: //ImageProcessing-Demosiac.cpp
	static CFloatImage Demosiac(CFloatImage &bayerImg);

private: //ImageProcessing-Demosiac.cpp
	static int getBayerChannel(int x, int y);
	static void setGreenKernel(CFloatImage &gKernel);
	static void setRB_Kernel1(CFloatImage &rbKernel1);
	static void setRB_Kernel2(CFloatImage &rbKernel2);
	static void setRB_Kernel3(CFloatImage &rbKernel3);

	static float getBayerRespose(CFloatImage &bayerImg, 
								 CFloatImage &kernel, 
								 int x, int y);


public: //ImageProcessing-DHW.cpp
	static CIntImage MatchImageHist(const CIntImage &targetImg, 
									const CIntImage &srcImg,
									int minChannelVal, int maxChannelVal,
									const CByteImage &targetMask = CByteImage(), bool useTargetMask = false,
									const CByteImage &srcMask    = CByteImage(), bool useSrcMask    = false);


	static int* GetImageHist(const CIntImage &image,
							  int minChannelVal, int maxChannelVal,
							  const CByteImage &imageMask = CByteImage(), bool useImageMask = false);

	static float* GetNormalizedImageHist(const CIntImage &image,
										  int minChannelVal, int maxChannelVal,
   										  const CByteImage &imageMask = CByteImage(), bool useImageMask = false);


	static float* GetCumlImageHist(const float *imgHist, int channelRange);


	static int* GetHistMappingUsingDHW(float const *targetCulmHist, 
										float const *srcCulmHist,
										int targetChannelRange,
										int srcChannelRange,
										int maxTargetHistCompression = 29,
										int maxSrcHistCompression = 29);

private: //ImageProcessing-DHW.cpp


};

#include "ImageProcessing.inl"

#endif



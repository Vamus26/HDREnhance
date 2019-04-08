#include "FlowHelper.h"
#include "ImageProcessing.h"
#include <float.h>
#include "ImageIO.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "robust.h"

void FlowHelper::WriteFlowField(const char *fname,
							   CFloatImage &flowField)
{
	CShape fieldShape = flowField.Shape();	
	ENSURE(fieldShape.depth	== 1);
	ENSURE(fieldShape.nBands == 3);

	uint frameSize    = fieldShape.width * fieldShape.height * fieldShape.nBands;
	float *tempBuffer = new float[frameSize]; 	
	ENSURE(tempBuffer != NULL);

	for(uint iElem = 0; iElem < frameSize; iElem++)
	{
		tempBuffer[iElem] = flowField[iElem];
	}

	FILE *filePtr = fopen(fname, "wb");
	if(filePtr == NULL)
	{
		REPORT_FAILURE("Error writing flow file - %s\n", fname);
	}
	if((fwrite(&fieldShape.width,  sizeof(fieldShape.width),  1, filePtr) != 1) ||
	   (fwrite(&fieldShape.height, sizeof(fieldShape.height), 1, filePtr) != 1) ||
	   (fwrite(&fieldShape.nBands, sizeof(fieldShape.nBands), 1, filePtr) != 1))
	{
		REPORT_FAILURE("Error writing flow file");
	}
	if(fwrite(tempBuffer, sizeof(*tempBuffer), frameSize, filePtr) != frameSize)
	{
		REPORT_FAILURE("Error writing flow file");
	}
	fclose(filePtr);

	delete tempBuffer;

}

void FlowHelper::ReadFlowField(const char *fname,
							   CFloatImage &flowField,
							   float scale)
{
	FILE *filePtr = fopen(fname, "rb");
	if(filePtr == NULL)
	{
		REPORT_FAILURE("Couldn't open file - %s\n", fname);
	}

	CShape fieldShape;
	fieldShape.depth  = 1;
	if((fread(&fieldShape.width,  sizeof(fieldShape.width),  1, filePtr) != 1) ||
	   (fread(&fieldShape.height, sizeof(fieldShape.height), 1, filePtr) != 1) ||
	   (fread(&fieldShape.nBands, sizeof(fieldShape.nBands), 1, filePtr) != 1))
	{
		REPORT_FAILURE("Error reading flow file");
	}
	ENSURE(fieldShape.nBands == 3);

	uint frameSize    = fieldShape.width * fieldShape.height * fieldShape.nBands;
	float *tempBuffer = new float[frameSize]; 
	ENSURE(tempBuffer != NULL);

	if(fread(tempBuffer, sizeof(*tempBuffer), frameSize, filePtr) != frameSize)
	{
		REPORT_FAILURE("Error reading flow file - %s", fname);
	}
	fclose(filePtr);

	flowField.ReAllocate(fieldShape);
	for (uint iElem = 0; iElem < frameSize; iElem++)
	{
		flowField[iElem] = tempBuffer[iElem];
	}

	delete tempBuffer;

	if(scale != 1.0f)
	{
		ImageProcessing::Operate(flowField, flowField, scale, ImageProcessing::OP_MUL);
		CFloatImage flowFieldCopy = flowField.Clone();
		ImageProcessing::ScaleImage(flowField, flowFieldCopy, scale);
	}
}


extern "C"
{
	void warp_image(float *image, float *warped, float *u, float *v, int nx, int ny);

	void pyramid_sor(float *image1, float *image2, int max_level, int min_level, int iters, float omega, float *u, float *v, 
				     float *prev_u, float *prev_v, float *du, float *dv, 
					float l1, float l2, float l3, float *s1, float *s2, float *s3, int nx, int ny,
					float *Ix, float *Iy, float *It, float *err, float *u_scale, float *v_scale, int filter);

	void data_disconts(float *image1, float *image2, float *u, float *v, float *s1, int nx, int ny, float *warp, float *It, float *discont);

	void spatial_disconts(float *u, float *v, float *s2, int nx, int ny, float *discont);

	void save_pgm(char *path, float *image, int nx, int ny);

	void save_pgm_no_scale(char *path, float *image, int nx, int ny);
}


void FlowHelper::copyFrameIntensity(float *buff, CFloatImage &img)
{
	CShape imgShape = img.Shape();
	ENSURE((imgShape.nBands == 1) || (imgShape.nBands == 3));

	uint pixelAddr = 0;
	uint nodeAddr  = 0;
	for (int y = 0; y < imgShape.height; y++)
	{
		for(int x = 0; x < imgShape.width; x++, pixelAddr += imgShape.nBands, nodeAddr++)
		{
            if(imgShape.nBands == 1)
			{
				buff[nodeAddr] = img[pixelAddr];				
			}
			else
			{
				buff[nodeAddr] = ((0.299f * img[pixelAddr + 0]) + 
					              (0.587f * img[pixelAddr + 1]) + 
								  (0.114f * img[pixelAddr + 2])) * 255.0f;
			}

			if(buff[nodeAddr] > 255.0f) buff[nodeAddr] = 255.0f;
			if(buff[nodeAddr] <   0.0f) buff[nodeAddr] = 0.0f;
		}
	}
}


void FlowHelper::setScalarField(float *field, CFloatImage &img)
{
	CShape imgShape = img.Shape();
	ENSURE(imgShape.nBands == 1);

	uint nodeAddr  = 0;
	for (int y = 0; y < imgShape.height; y++)
	{
		for(int x = 0; x < imgShape.width; x++, nodeAddr++)
		{
			img[nodeAddr] = field[nodeAddr];
		}
	}
}


void FlowHelper::setFlowField(float *u, float *v, CFloatImage &errImg, CFloatImage &flowField)
{
	CShape fieldShape = flowField.Shape();
	ENSURE(fieldShape.nBands == 3);

	uint fieldAddr = 0;
	uint nodeAddr  = 0;
	for (int y = 0; y < fieldShape.height; y++)
	{
		for(int x = 0; x < fieldShape.width; x++, fieldAddr += fieldShape.nBands, nodeAddr++)
		{
			flowField[fieldAddr + 0] = u[nodeAddr];
			flowField[fieldAddr + 1] = v[nodeAddr];
			flowField[fieldAddr + 2] = errImg[nodeAddr];
		}
	}
}

void FlowHelper::ComputeFlow(CFloatImage &prevFrame, 
							 CFloatImage &currFrame,
							 CFloatImage &flowField)
{	
	CShape imgShape = prevFrame.Shape();
	ENSURE(imgShape == currFrame.Shape());
	ENSURE(imgShape.SameIgnoringNBands(flowField.Shape()));
	ENSURE(imgShape.depth == 1);
	ENSURE(flowField.Shape().nBands == 3);

	float lambda1 = 1.0f, lambda2 = 1.0f, lambda3 = 0.0f, l3 = 0.05f;
	float s1Orig = 1.0f, s2Orig = 1.0f, s3Orig = 1.0f;
	float s1 = s1Orig, s2 = s2Orig, s3 = s3Orig;
	float s1_end = 1.0f, s2_end = 1.0f, s3_end = 1.0f;
	float factor = 0.75f;
	float s1_rate, s2_rate, s3_rate;
	float omega = 1.95f;
	int index, from, to, im, by=1, max_level, min_level;
	int iters = 5, nx=128, ny=128, filter=1, skip=0;
	int stages = 1, st;
	float s1_factor, s2_factor;

	unsigned char *uOut, *vOut;
	float *dx, *dy, *dt, *u, *v;
	float *u_scale, *v_scale, *du, *dv;
	float *prev_u, *prev_v, *prev_du, *prev_dv;
	float *prev, *curr, *lap1, *lap2, *err;
	float *discont, *outliers, *sigma1, *sigma2, *sigma3, *temp;
	float maxAmp;
	int fdOutu, fdOutv, fdOutdiscont, fdOuttemp;
	int sizeInput, imageSize, sizeDeriv, sizeOutput;
	int start, n, i, j, i0, ix, iy, ixf, iyf, itmp, icount;

	max_level = 7;
	min_level = 1;

	lambda1 = 10.0f;
	lambda2 = 10.0f;
	lambda3 = 0.0f;
	l3      = 0.0f;

	s1Orig = 10.0f / ROOT2;
	s2Orig = 10.0f / ROOT2;
	s1_end = 10.0f / ROOT2;
	s2_end = 0.05f / ROOT2;

	nx = imgShape.width;
	ny = imgShape.height;

	iters = 20;
	omega = 1.995f;

	factor = 0.5;
	filter = 0;
	stages = 5; 

	sizeDeriv  = nx * ny * sizeof( float );
	sizeOutput = nx * ny * sizeof( char );

	if ((err = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((discont = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((outliers = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((sigma1 = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((sigma2 = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((sigma3 = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((prev = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((curr = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((u = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((v = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((du = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((dv = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((prev_u = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((prev_v = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((prev_du = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((prev_dv = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((u_scale = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((v_scale = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((dx = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((dy = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);
	if ((dt = (float *) malloc((size_t) sizeDeriv)) == NULL)
		assert(false);

	
	s1 = s1Orig;
	s2 = s2Orig;
	s3 = s3Orig;

	memset(u, 0, sizeDeriv);
	memset(v, 0, sizeDeriv);
	memset(du, 0, sizeDeriv);
	memset(dv, 0, sizeDeriv);
	memset(prev_du, 0, sizeDeriv);
	memset(prev_dv, 0, sizeDeriv);
	memset(outliers, 0, sizeDeriv);
	memset(prev_u, 0, sizeDeriv);
	memset(prev_v, 0, sizeDeriv);

	for(i=0;i<ny;i++)
	{
		for(j=0;j<nx;j++)
		{
			index = (i * nx) + j;
			sigma1[index] = s1;
			sigma2[index] = s2;
			sigma3[index] = s3;
		}
	}

	copyFrameIntensity(prev, prevFrame);
	copyFrameIntensity(curr, currFrame);

	s1_factor = pow((s1_end/s1), (1.0f/ (float)(stages - 1)));
	s2_factor = pow((s2_end/s2), (1.0f/ (float)(stages - 1)));
	for(st=0; st<stages; st++) 
	{	
		/* update flow */
		pyramid_sor(prev, curr, max_level, min_level, iters, omega, u, v, 
			prev_u, prev_v, du, dv, lambda1, lambda2,
			lambda3, sigma1, sigma2, sigma3, nx, ny, dx, dy, dt, err,
			u_scale, v_scale, filter);

		//data_disconts(prev, curr, u, v, sigma1, nx, ny, err, dt, outliers);
		//spatial_disconts(u, v, sigma2, nx, ny, discont);
		//warp_image(prev, err, u, v, nx, ny);

	
		s1 = max((s1 * s1_factor), s1_end);
		s2 = max((s2 * s2_factor), s2_end);
	
		for(i=0;i<ny;i++)
		{
			for(j=0;j<nx;j++)
			{
				index = (i*nx) +j;
				sigma1[index] = s1;
				sigma2[index] = s2;
			}
		}

		memcpy(prev_u, u, sizeDeriv);
		memcpy(prev_v, v, sizeDeriv);
	}

	warp_image(prev, err, u, v, nx, ny);
	data_disconts(prev, curr, u, v, sigma1, nx, ny, err, dt, outliers);
	
	CShape maskShape = flowField.Shape();
	maskShape.nBands = 1;
	CFloatImage errImg(maskShape);
	int nodeCount = maskShape.width * maskShape.height;
	for(int iNode = 0; iNode < nodeCount; iNode++)
	{
		errImg[iNode] = 1.0f - abs((err[iNode] - curr[iNode]) / 255.0f);
		errImg[iNode] *= outliers[iNode];
		errImg[iNode] = max(0.0f, errImg[iNode]);
		errImg[iNode] = min(1.0f, errImg[iNode]);
	}

	//CFloatImage errImage(maskShape);
	//setScalarField(err, errImage);
	//ImageProcessing::Operate(errImage, errImage, 255.0f, ImageProcessing::OP_DIV);
	//ImageIO::WriteFile(errImage, "1.tga"); 

	//CFloatImage currImage(maskShape);
	//setScalarField(curr, currImage);
	//ImageProcessing::Operate(currImage, currImage, 255.0f, ImageProcessing::OP_DIV);
	//ImageIO::WriteFile(currImage, "2.tga"); 

	//ImageIO::WriteFile(errImg, "3.tga");

	//exit(0);

	setFlowField(u, v, errImg, flowField);

	delete err;
	delete discont;
	delete outliers;
	delete sigma1;
	delete sigma2;
	delete sigma3;
	delete prev;
	delete curr;
	delete u;
	delete v;
	delete du;
	delete dv;
	delete prev_u;
	delete prev_v;
	delete prev_du;
	delete prev_dv;
	delete u_scale;
	delete v_scale;
	delete dx;
	delete dy;
	delete dt;
}

#include "mex.h"
#include "project.h"
#include "Image.h"
#include "OpticalFlow.h"
#include <iostream>

using namespace std;

// void LoadImage(DImage& image,const mxArray* matrix)
// {
// 	if(mxIsClass(matrix,"uint8"))
// 	{
// 		image.LoadMatlabImage<unsigned char>(matrix);
// 		return;
// 	}
// 	if(mxIsClass(matrix,"int8"))
// 	{
// 		image.LoadMatlabImage<char>(matrix);
// 		return;
// 	}
// 	if(mxIsClass(matrix,"int32"))
// 	{
// 		image.LoadMatlabImage<int>(matrix);
// 		return;
// 	}
// 	if(mxIsClass(matrix,"uint32"))
// 	{
// 		image.LoadMatlabImage<unsigned int>(matrix);
// 		return;
// 	}
// 	if(mxIsClass(matrix,"int16"))
// 	{
// 		image.LoadMatlabImage<short int>(matrix);
// 		return;
// 	}
// 	if(mxIsClass(matrix,"uint16"))
// 	{
// 		image.LoadMatlabImage<unsigned short int>(matrix);
// 		return;
// 	}
// 	if(mxIsClass(matrix,"double"))
// 	{
// 		image.LoadMatlabImage<double>(matrix);
// 		return;
// 	}
// 	mexErrMsgTxt("Unknown type of the image!");
// }

void mexFunction(mwSize nlhs, mxArray *plhs[], mwSize nrhs, const mxArray *prhs[])
{
	DImage Im1,Im2, Im3;
    Im1.LoadMatlabImage(prhs[0]);
    Im2.LoadMatlabImage(prhs[1]);
	Im3.LoadMatlabImage(prhs[2]);

	if(Im1.matchDimension(Im2)==false)
		mexErrMsgTxt("The two images don't match!");
	
	// get the parameters
	double alpha= 1;
	double ratio=0.5;
	mwSize minWidth= 40;
	mwSize nOuterFPIterations = 3;
	mwSize nInnerFPIterations = 1;
	mwSize nSORIterations= 20;
	if(nrhs>2)
	{
		mwSize nDims=mxGetNumberOfDimensions(prhs[3]);
		const mwSize *dims=mxGetDimensions(prhs[3]);
		double* para=(double *)mxGetData(prhs[3]);
		mwSize npara=dims[0]*dims[1];
		if(npara>0)
			alpha=para[0];
		if(npara>1)
			ratio=para[1];
		if(npara>2)
			minWidth=para[2];
		if(npara>3)
			nOuterFPIterations=para[3];
		if(npara>4)
			nInnerFPIterations=para[4];
		if(npara>5)
			nSORIterations = para[5];
	}

	DImage vx,vy,warpI2,warpI3;
	OpticalFlow::Coarse2FineFlow(vx,vy,warpI2,Im1,Im2,alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations,warpI3, Im3);

	// output the parameters
	vx.OutputToMatlab(plhs[0]);
	vy.OutputToMatlab(plhs[1]);
	if (nlhs > 2) {
		warpI2.OutputToMatlab(plhs[2]);
		warpI3.OutputToMatlab(plhs[3]);
	}
}
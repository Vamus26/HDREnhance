/* file: mkl_vml_functions.h */
/*
//                             INTEL CONFIDENTIAL
//  Copyright(C) 2006 Intel Corporation. All Rights Reserved.
//  The source code contained  or  described herein and all documents related to
//  the source code ("Material") are owned by Intel Corporation or its suppliers
//  or licensors.  Title to the  Material remains with  Intel Corporation or its
//  suppliers and licensors. The Material contains trade secrets and proprietary
//  and  confidential  information of  Intel or its suppliers and licensors. The
//  Material  is  protected  by  worldwide  copyright  and trade secret laws and
//  treaty  provisions. No part of the Material may be used, copied, reproduced,
//  modified, published, uploaded, posted, transmitted, distributed or disclosed
//  in any way without Intel's prior express written permission.
//  No license  under any  patent, copyright, trade secret or other intellectual
//  property right is granted to or conferred upon you by disclosure or delivery
//  of the Materials,  either expressly, by implication, inducement, estoppel or
//  otherwise.  Any  license  under  such  intellectual property  rights must be
//  express and approved by Intel in writing.
*/
/*
//++
//  User-level VML function declarations
//--
*/

#ifndef __MKL_VML_FUNCTIONS_H__
#define __MKL_VML_FUNCTIONS_H__

#include "mkl_vml_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
//++
//  EXTERNAL API MACROS.
//  Used to construct VML function declaration. Change them if you are going to
//  provide different API for VML functions.
//--
*/
#if defined(MKL_VML_STDCALL)
  #define _Vml_Api(rtype,name,arg) extern rtype __stdcall name    arg;
  #define _vml_api(rtype,name,arg) extern rtype __stdcall name    arg;
  #define _VML_API(rtype,name,arg) extern rtype __stdcall name    arg;
#else /* MKL_VML_CDECL */
  #define _Vml_Api(rtype,name,arg) extern rtype __cdecl   name    arg;
  #define _vml_api(rtype,name,arg) extern rtype __cdecl   name    arg;
  #define _VML_API(rtype,name,arg) extern rtype __cdecl   name    arg;
#endif

/*
//++
//  VML ELEMENTARY FUNCTION DECLARATIONS.
//--
*/
/* Reciprocal: r[i] = 1.0 / a[i] */
_VML_API(void,VSINV,(int *n, const float  a[], float  r[]))
_VML_API(void,VDINV,(int *n, const double a[], double r[]))
_vml_api(void,vsinv,(int *n, const float  a[], float  r[]))
_vml_api(void,vdinv,(int *n, const double a[], double r[]))
_Vml_Api(void,vsInv,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdInv,(int n,  const double a[], double r[]))

/* Square Root: r[i] = a[i]^0.5 */
_VML_API(void,VSSQRT,(int *n, const float  a[], float  r[]))
_VML_API(void,VDSQRT,(int *n, const double a[], double r[]))
_vml_api(void,vssqrt,(int *n, const float  a[], float  r[]))
_vml_api(void,vdsqrt,(int *n, const double a[], double r[]))
_Vml_Api(void,vsSqrt,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdSqrt,(int n,  const double a[], double r[]))

/* Reciprocal Square Root: r[i] = 1/a[i]^0.5 */
_VML_API(void,VSINVSQRT,(int *n, const float  a[], float  r[]))
_VML_API(void,VDINVSQRT,(int *n, const double a[], double r[]))
_vml_api(void,vsinvsqrt,(int *n, const float  a[], float  r[]))
_vml_api(void,vdinvsqrt,(int *n, const double a[], double r[]))
_Vml_Api(void,vsInvSqrt,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdInvSqrt,(int n,  const double a[], double r[]))

/* Cube Root: r[i] = a[i]^(1/3) */
_VML_API(void,VSCBRT,(int *n, const float  a[], float  r[]))
_VML_API(void,VDCBRT,(int *n, const double a[], double r[]))
_vml_api(void,vscbrt,(int *n, const float  a[], float  r[]))
_vml_api(void,vdcbrt,(int *n, const double a[], double r[]))
_Vml_Api(void,vsCbrt,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdCbrt,(int n,  const double a[], double r[]))

/* Reciprocal Cube Root: r[i] = 1/a[i]^(1/3) */
_VML_API(void,VSINVCBRT,(int *n, const float  a[], float  r[]))
_VML_API(void,VDINVCBRT,(int *n, const double a[], double r[]))
_vml_api(void,vsinvcbrt,(int *n, const float  a[], float  r[]))
_vml_api(void,vdinvcbrt,(int *n, const double a[], double r[]))
_Vml_Api(void,vsInvCbrt,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdInvCbrt,(int n,  const double a[], double r[]))

/* Exponential Function: r[i] = e^a[i] */
_VML_API(void,VSEXP,(int *n, const float  a[], float  r[]))
_VML_API(void,VDEXP,(int *n, const double a[], double r[]))
_vml_api(void,vsexp,(int *n, const float  a[], float  r[]))
_vml_api(void,vdexp,(int *n, const double a[], double r[]))
_Vml_Api(void,vsExp,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdExp,(int n,  const double a[], double r[]))

/* Logarithm (base e): r[i] = ln(a[i]) */
_VML_API(void,VSLN,(int *n, const float  a[], float  r[]))
_VML_API(void,VDLN,(int *n, const double a[], double r[]))
_vml_api(void,vsln,(int *n, const float  a[], float  r[]))
_vml_api(void,vdln,(int *n, const double a[], double r[]))
_Vml_Api(void,vsLn,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdLn,(int n,  const double a[], double r[]))

/* Logarithm (base 10): r[i] = lg(a[i]) */
_VML_API(void,VSLOG10,(int *n, const float  a[], float  r[]))
_VML_API(void,VDLOG10,(int *n, const double a[], double r[]))
_vml_api(void,vslog10,(int *n, const float  a[], float  r[]))
_vml_api(void,vdlog10,(int *n, const double a[], double r[]))
_Vml_Api(void,vsLog10,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdLog10,(int n,  const double a[], double r[]))

/* Sine: r[i] = sin(a[i]) */
_VML_API(void,VSSIN,(int *n, const float  a[], float  r[]))
_VML_API(void,VDSIN,(int *n, const double a[], double r[]))
_vml_api(void,vssin,(int *n, const float  a[], float  r[]))
_vml_api(void,vdsin,(int *n, const double a[], double r[]))
_Vml_Api(void,vsSin,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdSin,(int n,  const double a[], double r[]))

/* Cosine: r[i] = cos(a[i]) */
_VML_API(void,VSCOS,(int *n, const float  a[], float  r[]))
_VML_API(void,VDCOS,(int *n, const double a[], double r[]))
_vml_api(void,vscos,(int *n, const float  a[], float  r[]))
_vml_api(void,vdcos,(int *n, const double a[], double r[]))
_Vml_Api(void,vsCos,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdCos,(int n,  const double a[], double r[]))

/* Tangent: r[i] = tan(a[i]) */
_VML_API(void,VSTAN,(int *n, const float  a[], float  r[]))
_VML_API(void,VDTAN,(int *n, const double a[], double r[]))
_vml_api(void,vstan,(int *n, const float  a[], float  r[]))
_vml_api(void,vdtan,(int *n, const double a[], double r[]))
_Vml_Api(void,vsTan,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdTan,(int n,  const double a[], double r[]))

/* Hyperbolic Sine: r[i] = sh(a[i]) */
_VML_API(void,VSSINH,(int *n, const float  a[], float  r[]))
_VML_API(void,VDSINH,(int *n, const double a[], double r[]))
_vml_api(void,vssinh,(int *n, const float  a[], float  r[]))
_vml_api(void,vdsinh,(int *n, const double a[], double r[]))
_Vml_Api(void,vsSinh,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdSinh,(int n,  const double a[], double r[]))

/* Hyperbolic Cosine: r[i] = ch(a[i]) */
_VML_API(void,VSCOSH,(int *n, const float  a[], float  r[]))
_VML_API(void,VDCOSH,(int *n, const double a[], double r[]))
_vml_api(void,vscosh,(int *n, const float  a[], float  r[]))
_vml_api(void,vdcosh,(int *n, const double a[], double r[]))
_Vml_Api(void,vsCosh,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdCosh,(int n,  const double a[], double r[]))

/* Hyperbolic Tangent: r[i] = th(a[i]) */
_VML_API(void,VSTANH,(int *n, const float  a[], float  r[]))
_VML_API(void,VDTANH,(int *n, const double a[], double r[]))
_vml_api(void,vstanh,(int *n, const float  a[], float  r[]))
_vml_api(void,vdtanh,(int *n, const double a[], double r[]))
_Vml_Api(void,vsTanh,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdTanh,(int n,  const double a[], double r[]))

/* Arc Cosine: r[i] = arccos(a[i]) */
_VML_API(void,VSACOS,(int *n, const float  a[], float  r[]))
_VML_API(void,VDACOS,(int *n, const double a[], double r[]))
_vml_api(void,vsacos,(int *n, const float  a[], float  r[]))
_vml_api(void,vdacos,(int *n, const double a[], double r[]))
_Vml_Api(void,vsAcos,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdAcos,(int n,  const double a[], double r[]))

/* Arc Sine: r[i] = arcsin(a[i]) */
_VML_API(void,VSASIN,(int *n, const float  a[], float  r[]))
_VML_API(void,VDASIN,(int *n, const double a[], double r[]))
_vml_api(void,vsasin,(int *n, const float  a[], float  r[]))
_vml_api(void,vdasin,(int *n, const double a[], double r[]))
_Vml_Api(void,vsAsin,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdAsin,(int n,  const double a[], double r[]))

/* Arc Tangent: r[i] = arctan(a[i]) */
_VML_API(void,VSATAN,(int *n, const float  a[], float  r[]))
_VML_API(void,VDATAN,(int *n, const double a[], double r[]))
_vml_api(void,vsatan,(int *n, const float  a[], float  r[]))
_vml_api(void,vdatan,(int *n, const double a[], double r[]))
_Vml_Api(void,vsAtan,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdAtan,(int n,  const double a[], double r[]))

/* Hyperbolic Arc Cosine: r[i] = arcch(a[i]) */
_VML_API(void,VSACOSH,(int *n, const float  a[], float  r[]))
_VML_API(void,VDACOSH,(int *n, const double a[], double r[]))
_vml_api(void,vsacosh,(int *n, const float  a[], float  r[]))
_vml_api(void,vdacosh,(int *n, const double a[], double r[]))
_Vml_Api(void,vsAcosh,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdAcosh,(int n,  const double a[], double r[]))

/* Hyperbolic Arc Sine: r[i] = arcsh(a[i]) */
_VML_API(void,VSASINH,(int *n, const float  a[], float  r[]))
_VML_API(void,VDASINH,(int *n, const double a[], double r[]))
_vml_api(void,vsasinh,(int *n, const float  a[], float  r[]))
_vml_api(void,vdasinh,(int *n, const double a[], double r[]))
_Vml_Api(void,vsAsinh,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdAsinh,(int n,  const double a[], double r[]))

/* Hyperbolic Arc Tangent: r[i] = arcth(a[i]) */
_VML_API(void,VSATANH,(int *n, const float  a[], float  r[]))
_VML_API(void,VDATANH,(int *n, const double a[], double r[]))
_vml_api(void,vsatanh,(int *n, const float  a[], float  r[]))
_vml_api(void,vdatanh,(int *n, const double a[], double r[]))
_Vml_Api(void,vsAtanh,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdAtanh,(int n,  const double a[], double r[]))

/*  Error Function: r[i] = erf(a[i]) */
_VML_API(void,VSERF,(int *n, const float  a[], float  r[]))
_VML_API(void,VDERF,(int *n, const double a[], double r[]))
_vml_api(void,vserf,(int *n, const float  a[], float  r[]))
_vml_api(void,vderf,(int *n, const double a[], double r[]))
_Vml_Api(void,vsErf,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdErf,(int n,  const double a[], double r[]))

/*  Complementary Error Function: r[i] = 1 - erf(a[i]) */
_VML_API(void,VSERFC,(int *n, const float  a[], float  r[]))
_VML_API(void,VDERFC,(int *n, const double a[], double r[]))
_vml_api(void,vserfc,(int *n, const float  a[], float  r[]))
_vml_api(void,vderfc,(int *n, const double a[], double r[]))
_Vml_Api(void,vsErfc,(int n,  const float  a[], float  r[]))
_Vml_Api(void,vdErfc,(int n,  const double a[], double r[]))

/* Arc Tangent of a/b: r[i] = arctan(a[i]/b[i]) */
_VML_API(void,VSATAN2,(int *n, const float  a[], const float  b[], \
  float  r[]))
_VML_API(void,VDATAN2,(int *n, const double a[], const double b[], \
  double r[]))
_vml_api(void,vsatan2,(int *n, const float  a[], const float  b[], \
  float  r[]))
_vml_api(void,vdatan2,(int *n, const double a[], const double b[], \
  double r[]))
_Vml_Api(void,vsAtan2,(int n,  const float  a[], const float  b[], \
  float  r[]))
_Vml_Api(void,vdAtan2,(int n,  const double a[], const double b[], \
  double r[]))

/* Division: r[i] = a[i] / b[i] */
_VML_API(void,VSDIV,(int *n, const float  a[], const float  b[], float  r[]))
_VML_API(void,VDDIV,(int *n, const double a[], const double b[], double r[]))
_vml_api(void,vsdiv,(int *n, const float  a[], const float  b[], float  r[]))
_vml_api(void,vddiv,(int *n, const double a[], const double b[], double r[]))
_Vml_Api(void,vsDiv,(int n,  const float  a[], const float  b[], float  r[]))
_Vml_Api(void,vdDiv,(int n,  const double a[], const double b[], double r[]))

/* Power Function: r[i] = a[i]^b[i] */
_VML_API(void,VSPOW,(int *n, const float  a[], const float  b[], float  r[]))
_VML_API(void,VDPOW,(int *n, const double a[], const double b[], double r[]))
_vml_api(void,vspow,(int *n, const float  a[], const float  b[], float  r[]))
_vml_api(void,vdpow,(int *n, const double a[], const double b[], double r[]))
_Vml_Api(void,vsPow,(int n,  const float  a[], const float  b[], float  r[]))
_Vml_Api(void,vdPow,(int n,  const double a[], const double b[], double r[]))

/* Power Function with Fixed Degree: r[i] = a[i]^b */
_VML_API(void,VSPOWX,(int *n, const float  a[], const float  *b, float  r[]))
_VML_API(void,VDPOWX,(int *n, const double a[], const double *b, double r[]))
_vml_api(void,vspowx,(int *n, const float  a[], const float  *b, float  r[]))
_vml_api(void,vdpowx,(int *n, const double a[], const double *b, double r[]))
_Vml_Api(void,vsPowx,(int n,  const float  a[], const float  b, float  r[]))
_Vml_Api(void,vdPowx,(int n,  const double a[], const double b, double r[]))

/* Sine & Cosine: r1[i] = sin(a[i]), r2[i]=cos(a[i]) */
_VML_API(void,VSSINCOS,(int *n, const float  a[], float  r1[], float  r2[]))
_VML_API(void,VDSINCOS,(int *n, const double a[], double r1[], double r2[]))
_vml_api(void,vssincos,(int *n, const float  a[], float  r1[], float  r2[]))
_vml_api(void,vdsincos,(int *n, const double a[], double r1[], double r2[]))
_Vml_Api(void,vsSinCos,(int n,  const float  a[], float  r1[], float  r2[]))
_Vml_Api(void,vdSinCos,(int n,  const double a[], double r1[], double r2[]))

/*
//++
//  VML PACK FUNCTION DECLARATIONS.
//--
*/
/* Positive Increment Indexing */
_VML_API(void,VSPACKI,(int *n, const float  a[], const int * incra, \
  float  y[]))
_VML_API(void,VDPACKI,(int *n, const double a[], const int * incra, \
  double y[]))
_vml_api(void,vspacki,(int *n, const float  a[], const int * incra, \
  float  y[]))
_vml_api(void,vdpacki,(int *n, const double a[], const int * incra, \
  double y[]))
_Vml_Api(void,vsPackI,(int n,  const float  a[], const int incra,   \
  float  y[]))
_Vml_Api(void,vdPackI,(int n,  const double a[], const int incra,   \
  double y[]))

/* Index Vector Indexing */
_VML_API(void,VSPACKV,(int *n, const float  a[], const int ia[],    \
  float  y[]))
_VML_API(void,VDPACKV,(int *n, const double a[], const int ia[],    \
  double y[]))
_vml_api(void,vspackv,(int *n, const float  a[], const int ia[],    \
  float  y[]))
_vml_api(void,vdpackv,(int *n, const double a[], const int ia[],    \
  double y[]))
_Vml_Api(void,vsPackV,(int n,  const float  a[], const int ia[],    \
  float  y[]))
_Vml_Api(void,vdPackV,(int n,  const double a[], const int ia[],    \
  double y[]))

/* Mask Vector Indexing */
_VML_API(void,VSPACKM,(int *n, const float  a[], const int ma[],    \
  float  y[]))
_VML_API(void,VDPACKM,(int *n, const double a[], const int ma[],    \
  double y[]))
_vml_api(void,vspackm,(int *n, const float  a[], const int ma[],    \
  float  y[]))
_vml_api(void,vdpackm,(int *n, const double a[], const int ma[],    \
  double y[]))
_Vml_Api(void,vsPackM,(int n,  const float  a[], const int ma[],    \
  float  y[]))
_Vml_Api(void,vdPackM,(int n,  const double a[], const int ma[],    \
  double y[]))

/*
//++
//  VML UNPACK FUNCTION DECLARATIONS.
//--
*/
/* Positive Increment Indexing */
_VML_API(void,VSUNPACKI,(int *n, const float  a[], float  y[], \
  const int * incry))
_VML_API(void,VDUNPACKI,(int *n, const double a[], double y[], \
  const int * incry))
_vml_api(void,vsunpacki,(int *n, const float  a[], float  y[], \
  const int * incry))
_vml_api(void,vdunpacki,(int *n, const double a[], double y[], \
  const int * incry))
_Vml_Api(void,vsUnpackI,(int n,  const float  a[], float  y[], \
  const int incry  ))
_Vml_Api(void,vdUnpackI,(int n,  const double a[], double y[], \
  const int incry  ))

/* Index Vector Indexing */
_VML_API(void,VSUNPACKV,(int *n, const float  a[], float  y[], \
  const int iy[]   ))
_VML_API(void,VDUNPACKV,(int *n, const double a[], double y[], \
  const int iy[]   ))
_vml_api(void,vsunpackv,(int *n, const float  a[], float  y[], \
  const int iy[]   ))
_vml_api(void,vdunpackv,(int *n, const double a[], double y[], \
  const int iy[]   ))
_Vml_Api(void,vsUnpackV,(int n,  const float  a[], float  y[], \
  const int iy[]   ))
_Vml_Api(void,vdUnpackV,(int n,  const double a[], double y[], \
  const int iy[]   ))

/* Mask Vector Indexing */
_VML_API(void,VSUNPACKM,(int *n, const float  a[], float  y[], \
  const int my[]   ))
_VML_API(void,VDUNPACKM,(int *n, const double a[], double y[], \
  const int my[]   ))
_vml_api(void,vsunpackm,(int *n, const float  a[], float  y[], \
  const int my[]   ))
_vml_api(void,vdunpackm,(int *n, const double a[], double y[], \
  const int my[]   ))
_Vml_Api(void,vsUnpackM,(int n,  const float  a[], float  y[], \
  const int my[]   ))
_Vml_Api(void,vdUnpackM,(int n,  const double a[], double y[], \
  const int my[]   ))


/*
//++
//  VML ERROR HANDLING FUNCTION DECLARATIONS.
//--
*/
/* Set VML Error Status */
_VML_API(int,VMLSETERRSTATUS,(int * status))
_vml_api(int,vmlseterrstatus,(int * status))
_Vml_Api(int,vmlSetErrStatus,(int   status))

/* Get VML Error Status */
_VML_API(int,VMLGETERRSTATUS,(void))
_vml_api(int,vmlgeterrstatus,(void))
_Vml_Api(int,vmlGetErrStatus,(void))

/* Clear VML Error Status */
_VML_API(int,VMLCLEARERRSTATUS,(void))
_vml_api(int,vmlclearerrstatus,(void))
_Vml_Api(int,vmlClearErrStatus,(void))

/* Set VML Error Callback Function */
_VML_API(VMLErrorCallBack,VMLSETERRORCALLBACK,(VMLErrorCallBack func))
_vml_api(VMLErrorCallBack,vmlseterrorcallback,(VMLErrorCallBack func))
_Vml_Api(VMLErrorCallBack,vmlSetErrorCallBack,(VMLErrorCallBack func))

/* Get VML Error Callback Function */
_VML_API(VMLErrorCallBack,VMLGETERRORCALLBACK,(void))
_vml_api(VMLErrorCallBack,vmlgeterrorcallback,(void))
_Vml_Api(VMLErrorCallBack,vmlGetErrorCallBack,(void))

/* Reset VML Error Callback Function */
_VML_API(VMLErrorCallBack,VMLCLEARERRORCALLBACK,(void))
_vml_api(VMLErrorCallBack,vmlclearerrorcallback,(void))
_Vml_Api(VMLErrorCallBack,vmlClearErrorCallBack,(void))


/*
//++
//  VML MODE FUNCTION DECLARATIONS.
//--
*/
/* Set VML Mode */
_VML_API(unsigned int,VMLSETMODE,(unsigned int *newmode))
_vml_api(unsigned int,vmlsetmode,(unsigned int *newmode))
_Vml_Api(unsigned int,vmlSetMode,(unsigned int  newmode))

/* Get VML Mode */
_VML_API(unsigned int,VMLGETMODE,(void))
_vml_api(unsigned int,vmlgetmode,(void))
_Vml_Api(unsigned int,vmlGetMode,(void))


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MKL_VML_FUNCTIONS_H__ */

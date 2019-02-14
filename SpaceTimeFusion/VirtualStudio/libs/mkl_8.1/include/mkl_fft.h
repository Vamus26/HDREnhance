/*******************************************************************************
!                             INTEL CONFIDENTIAL
!   Copyright(C) 1999-2006 Intel Corporation. All Rights Reserved.
!   The source code contained  or  described herein and all documents related to
!   the source code ("Material") are owned by Intel Corporation or its suppliers
!   or licensors.  Title to the  Material remains with  Intel Corporation or its
!   suppliers and licensors. The Material contains trade secrets and proprietary
!   and  confidential  information of  Intel or its suppliers and licensors. The
!   Material  is  protected  by  worldwide  copyright  and trade secret laws and
!   treaty  provisions. No part of the Material may be used, copied, reproduced,
!   modified, published, uploaded, posted, transmitted, distributed or disclosed
!   in any way without Intel's prior express written permission.
!   No license  under any  patent, copyright, trade secret or other intellectual
!   property right is granted to or conferred upon you by disclosure or delivery
!   of the Materials,  either expressly, by implication, inducement, estoppel or
!   otherwise.  Any  license  under  such  intellectual property  rights must be
!   express and approved by Intel in writing.
!
!*******************************************************************************
!  Content:
!      Intel(R) Math Kernel Library (MKL) interface for FFT routines
!******************************************************************************/

#ifndef __mkl_fft_h__
#define __mkl_fft_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Upper case declaration */

void   CFFT1D(float* ar,  int* m, int* isign, float* ws);
void   ZFFT1D(double* ar, int* m, int* isign, double* ws);
void   CSFFT1D(float* r,  int* m, int* isign, float* ws);
void   SCFFT1D(float* r,  int* m, int* isign, float* ws);
void   ZDFFT1D(double* r, int* m, int* isign, double* ws);
void   DZFFT1D(double* r, int* m, int* isign, double* ws);
void   CFFT1DC(float* ar, float* ai, int m, int isign, float* ws);
void   ZFFT1DC(double* ar, double* ai, int m, int isign, double* ws);
void   CSFFT1DC(float* r,  int m, int isign, float* ws);
void   SCFFT1DC(float* r,  int m, int isign, float* ws);
void   ZDFFT1DC(double* r, int m, int isign, double* ws);
void   DZFFT1DC(double* r, int m, int isign, double* ws);
void   CFFT2D(float* ar,  int* m, int* n, int* isign);
void   ZFFT2D(double* ar, int* m, int* n, int* isign);
void   CSFFT2D(float* r,  int* m, int* n);
void   SCFFT2D(float* r,  int* m, int* n);
void   ZDFFT2D(double* r, int* m, int* n);
void   DZFFT2D(double* r, int* m, int* n);
void   CFFT2DC(float* ar, float* ai, int m, int n, int isign);
void   ZFFT2DC(double* ar, double* ai, int m, int n, int isign);
void   CSFFT2DC(float* r, int m, int n);
void   SCFFT2DC(float* r, int m, int n);
void   ZDFFT2DC(double* r, int m, int n);
void   DZFFT2DC(double* r, int m, int n);

/* Lower case declaration */

void   cfft1d(float* ar,  int* m, int* isign, float* ws);
void   zfft1d(double* ar, int* m, int* isign, double* ws);
void   csfft1d(float* r,  int* m, int* isign, float* ws);
void   scfft1d(float* r,  int* m, int* isign, float* ws);
void   zdfft1d(double* r, int* m, int* isign, double* ws);
void   dzfft1d(double* r, int* m, int* isign, double* ws);
void   cfft1dc(float* ar, float* ai, int m, int isign, float* ws);
void   zfft1dc(double* ar, double* ai, int m, int isign, double* ws);
void   csfft1dc(float* r,  int m, int isign, float* ws);
void   scfft1dc(float* r,  int m, int isign, float* ws);
void   zdfft1dc(double* r, int m, int isign, double* ws);
void   dzfft1dc(double* r, int m, int isign, double* ws);
void   cfft2d(float* ar,  int* m, int* n, int* isign);
void   zfft2d(double* ar, int* m, int* n, int* isign);
void   csfft2d(float* r,  int* m, int* n);
void   scfft2d(float* r,  int* m, int* n);
void   zdfft2d(double* r, int* m, int* n);
void   dzfft2d(double* r, int* m, int* n);
void   cfft2dc(float* ar, float* ai, int m, int n, int isign);
void   zfft2dc(double* ar, double* ai, int m, int n, int isign);
void   csfft2dc(float* r, int m, int n);
void   scfft2dc(float* r, int m, int n);
void   zdfft2dc(double* r, int m, int n);
void   dzfft2dc(double* r, int m, int n);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mkl_fft_h__ */

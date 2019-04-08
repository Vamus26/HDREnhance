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
!      Intel(R) Math Kernel Library (MKL) interface for BLAS routines
!******************************************************************************/

#ifndef _MKL_BLAS_H_
#define _MKL_BLAS_H_

#include "mkl_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Upper case declaration */

/* BLAS Level1 */
                              
double DCABS1(const MKL_Complex16 *z);
float SASUM(const int *n, const float *x, const int *incx);
void  SAXPY(const int *n, const float *alpha, const float *x, const int *incx,
            float *y, const int *incy);
void  SAXPYI(const int *nz, const float *a, const float *x, const int *indx,float *y);
float SCASUM(const int *n, const MKL_Complex8 *x, const int *incx); 
float SCNRM2(const int *n, const MKL_Complex8 *x, const int *incx); 
void  SCOPY(const int *n, const float *x, const int *incx, float *y, const int *incy);
float SDOT(const int *n, const float *x, const int *incx,
           const float *y, const int *incy);
double DSDOT(const int *n, const float *x, const int *incx, 
             const float *y, const int *incy);
float SDOTI(const int *nz, const float *x, const int *indx, const float *y);
void  SGTHR(const int *nz, const float *y, float *x, const int *indx);
void  SGTHRZ(const int *nz, float *y, float *x, const int *indx);
float SNRM2(const int *n, const float *x, const int *incx);
void  SROT(const int *n, float *x, const int *incx, float *y, const int *incy,
           const float *c, const float *s);
void  SROTG(float *a,float *b,float *c,float *s);
void  SROTI(const int *nz, float *x, const int *indx, float *y, const float *c, const float *s);
void  SROTM(const int *n, float *x, const int *incx, float *y, const int *incy, const float *param);
void  SROTMG(float *d1, float *d2, float *x1, const float *y1, float *param);
void  SSCAL(const int *n, const float *a, float *x, const int *incx);
void  SSCTR(const int *nz, const float *x, const int *indx, float *y);
void  SSWAP(const int *n, float *x, const int *incx, float *y, const int *incy);
int   ISAMAX(const int *n, const float *x, const int *incx);
int   ISAMIN(const int *n, const float *x, const int *incx);

void CAXPY(const int *n, const MKL_Complex8 *alpha, const MKL_Complex8 *x, const int *incx,
           MKL_Complex8 *y, const int *incy); 
void CAXPYI(const int *nz, const MKL_Complex8 *a, 
            const MKL_Complex8 *x, const int *indx, MKL_Complex8 *y); 
void CCOPY(const int *n, const MKL_Complex8 *x, const int *incx,
           MKL_Complex8 *y, const int *incy); 
void CDOTC(MKL_Complex8 *pres, const int *n, const MKL_Complex8 *x, const int *incx,
           const  MKL_Complex8 *y, const int *incy); 
void CDOTCI(MKL_Complex8 *pres, const int *nz, const MKL_Complex8 *x, const int *indx,
            const MKL_Complex8 *y); 
void CDOTU(MKL_Complex8 *pres, const int *n, const MKL_Complex8 *x, const int *incx,
           const  MKL_Complex8 *y, const int *incy); 
void CDOTUI(MKL_Complex8 *pres, const int *nz, const MKL_Complex8 *x, const int *indx,
            const MKL_Complex8 *y); 
void CGTHR(const int *nz, const MKL_Complex8 *y, MKL_Complex8 *x, const int *indx); 
void CGTHRZ(const int *nz, MKL_Complex8 *y, MKL_Complex8 *x, const int *indx); 
void CROTG(MKL_Complex8 *a, const MKL_Complex8 *b, float *c, MKL_Complex8 *s); 
void CSCAL(const int *n, const MKL_Complex8 *a, MKL_Complex8 *x, const int *incx); 
void CSCTR(const int *nz, const MKL_Complex8 *x, const int *indx, MKL_Complex8 *y); 
void CSROT(const int *n, MKL_Complex8 *x, const int *incx, MKL_Complex8 *y, const int *incy,
           const float *c, const float *s); 
void CSSCAL(const int *n, const float *a, MKL_Complex8 *x, const int *incx); 
void CSWAP(const int *n, MKL_Complex8 *x, const int *incx, MKL_Complex8 *y, const int *incy); 
int  ICAMAX(const int *n, const MKL_Complex8 *x, const int *incx); 
int  ICAMIN(const int *n, const MKL_Complex8 *x, const int *incx); 

double DASUM(const int *n, const double *x, const int *incx);
void   DAXPY(const int *n, const double *alpha, const double *x, const int *incx,
             double *y, const int *incy);
void   DAXPYI(const int *nz, const double *a, const double *x, const int *indx, double *y);
void   DCOPY(const int *n, const double *x, const int *incx, double *y, const int *incy);
double DDOT(const  int *n, const double *x, const int *incx, 
            const double *y, const int *incy);
float  SDSDOT(const int *n, const double *x, const int *incx, 
              const double *y, const int *incy);
double DDOTI(const int *nz, const double *x, const int *indx, const double *y);
void   DGTHR(const int *nz, const double *y, double *x, const int *indx);
void   DGTHRZ(const int *nz, double *y, double *x, const int *indx);
double DNRM2(const int *n, const double *x, const int *incx);
void   DROT(const int *n, double *x, const int *incx, double *y, const int *incy,
            const double *c, const double *s);
void   DROTG(double *a,double *b,double *c,double *s);
void   DROTI(const int *nz, double *x, const int *indx, double *y, const double *c, const double *s);
void   DROTM(const int *n, double *x, const int *incx, double *y, const int *incy, const double *param);
void   DROTMG(double *d1, double *d2, double *x1, const double *y1, double *param);
void   DSCAL(const int *n, const double *a, double *x, const int *incx);
void   DSCTR(const int *nz, const double *x, const int *indx, double *y);
void   DSWAP(const int *n, double *x, const int *incx, double *y, const int *incy);
double DZASUM(const int *n, const MKL_Complex16 *x, const int *incx); 
double DZNRM2(const int *n, const MKL_Complex16 *x, const int *incx); 
int    IDAMAX(const int *n, const double *x, const int *incx);
int    IDAMIN(const int *n, const double *x, const int *incx);

void ZAXPY(const int *n, const MKL_Complex16 *alpha, const MKL_Complex16 *x, const int *incx,
           MKL_Complex16 *y, const int *incy); 
void ZAXPYI(const int *nz, const MKL_Complex16 *a,
            const MKL_Complex16 *x, const int *indx, MKL_Complex16 *y); 
void ZCOPY(const int *n, const MKL_Complex16 *x, const int *incx,
           MKL_Complex16 *y, const int *incy); 
void ZDOTC(MKL_Complex16 *pres, const int *n, const MKL_Complex16 *x, const int *incx,
           const  MKL_Complex16 *y, const int *incy); 
void ZDOTCI(MKL_Complex16 *pres,const int *nz, const MKL_Complex16 *x, const int *indx,
            const MKL_Complex16 *y); 
void ZDOTU(MKL_Complex16 *pres, const int *n, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *y, const int *incy); 
void ZDOTUI(MKL_Complex16 *pres, const int *nz, const MKL_Complex16 *x, const int *indx,
            const MKL_Complex16 *y); 
void ZDROT(const int *n, MKL_Complex16 *x, const int *incx, MKL_Complex16 *y, const int *incy,
           const double *c, const double *s); 
void ZDSCAL(const int *n, const double *a, MKL_Complex16 *x, const int *incx); 
void ZGTHR(const int *nz, const MKL_Complex16 *y, MKL_Complex16 *x, const int *indx); 
void ZGTHRZ(const int *nz, MKL_Complex16 *y, MKL_Complex16 *x, const int *indx); 
void ZROTG(MKL_Complex16 *a, const MKL_Complex16 *b, double *c, MKL_Complex16 *s); 
void ZSCAL(const int *n, const MKL_Complex16 *a, MKL_Complex16 *x, const int *incx); 
void ZSCTR(const int *nz, const MKL_Complex16 *x, const int *indx, MKL_Complex16 *y); 
void ZSWAP(const int *n, MKL_Complex16 *x, const int *incx, MKL_Complex16 *y, const int *incy); 
int  IZAMAX(const int *n, const MKL_Complex16 *x, const int *incx); 
int  IZAMIN(const  int *n,const  MKL_Complex16 *x, const int *incx); 

/* BLAS Level2 */

void SGBMV(const char *trans, const int *m, const int *n, const int *kl, const int *ku,
           const float *alpha, const float *a, const int *lda, const float *x, const int *incx,
           const float *beta, float *y, const int *incy);
void SGEMV(const char *trans, const int *m, const int *n, const float *alpha,
           const float *a, const int *lda, const float *x, const int *incx,
           const float *beta, float *y, const int *incy);
void SGER(const int *m, const int *n, const float *alpha, const float *x, const int *incx,
          const float *y, const int *incy, float *a, const int *lda);
void SSBMV(const char *uplo, const int *n, const int *k, 
           const float *alpha, const float *a, const int *lda, const float *x, const int *incx,
           const float *beta, float *y, const int *incy);
void SSPMV(const char *uplo, const int *n, const float *alpha, const float *ap,
           const float *x, const int *incx, const float *beta, float *y, const int *incy);
void SSPR(const char *uplo, const int *n, const float *alpha, const float *x, const int *incx, float *ap);
void SSPR2(const char *uplo, const int *n, const float *alpha, const float *x, const int *incx,
           const float *y, const int *incy, float *ap);
void SSYMV(const char *uplo, const int *n, const float *alpha, const float *a, const int *lda,
           const float *x, const int *incx, const float *beta, float *y, const int *incy);
void SSYR(const char *uplo, const int *n, const float *alpha, const float *x, const int *incx,
          float *a, const int *lda);
void SSYR2(const char *uplo, const int *n, const float *alpha, const float *x, const int *incx,
           const float *y, const int *incy, float *a, const int *lda);
void STBMV(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const float *a, const int *lda, float *x, const int *incx);
void STBSV(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const float *a, const int *lda, float *x, const int *incx);
void STPMV(const char *uplo, const char *trans, const char *diag, const int *n,
           const float *ap, float *x, const int *incx);
void STPSV(const char *uplo, const char *trans, const char *diag, const int *n,
           const float *ap, float *x, const int *incx);
void STRMV(const char *uplo, const char *transa, const char *diag, const int *n,
           const float *a, const int *lda, float *b, const int *incx);
void STRSV(const char *uplo, const char *trans, const char *diag, const int *n,
           const float *a, const int *lda, float *x, const int *incx);

void CGBMV(const char *trans, const int *m, const int *n, const int *kl, const int *ku,
           const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
           const MKL_Complex8 *x, const int *incx, const MKL_Complex8 *beta,
           MKL_Complex8 *y, const int *incy); 
void CGEMV(const char *trans, const int *m, const int *n, const MKL_Complex8 *alpha,
           const MKL_Complex8 *a, const int *lda, const MKL_Complex8 *x, const int *incx,
           const MKL_Complex8 *beta, MKL_Complex8 *y, const int *incy); 
void CGERC(const int *m, const int *n, const MKL_Complex8 *alpha, 
           const MKL_Complex8 *x, const int *incx, const MKL_Complex8 *y, const int *incy,
           MKL_Complex8 *a, const int *lda); 
void CGERU(const int *m, const int *n, const MKL_Complex8 *alpha,
           const MKL_Complex8 *x, const int *incx, const MKL_Complex8 *y, const int *incy,
           MKL_Complex8 *a, const int *lda); 
void CHBMV(const char *uplo, const int *n, const int *k, const MKL_Complex8 *alpha,
           const MKL_Complex8 *a, const int *lda, const MKL_Complex8 *x, const int *incx,
           const MKL_Complex8 *beta, MKL_Complex8 *y, const int *incy); 
void CHEMV(const char *uplo, const int *n, const MKL_Complex8 *alpha, 
           const MKL_Complex8 *a, const int *lda, const MKL_Complex8 *x, const int *incx,
           const MKL_Complex8 *beta, MKL_Complex8 *y, const int *incy); 
void CHER(const char *uplo, const int *n, const float *alpha, const MKL_Complex8 *x, const int *incx,
          MKL_Complex8 *a, const int *lda); 
void CHER2(const char *uplo, const int *n, const MKL_Complex8 *alpha,
           const MKL_Complex8 *x, const int *incx, const MKL_Complex8 *y, const int *incy,
           MKL_Complex8 *a, const int *lda); 
void CHPMV(const char *uplo, const int *n, const MKL_Complex8 *alpha, const MKL_Complex8 *ap,
           const MKL_Complex8 *x, const int *incx, const MKL_Complex8 *beta,
           MKL_Complex8 *y, const int *incy); 
void CHPR(const char *uplo, const int *n, const float *alpha, const MKL_Complex8 *x, const int *incx,
          MKL_Complex8 *ap); 
void CHPR2(const char *uplo, const int *n, const MKL_Complex8 *alpha, 
           const MKL_Complex8 *x, const int *incx, const MKL_Complex8 *y, const int *incy,
           MKL_Complex8 *ap); 
void CTBMV(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const MKL_Complex8 *a, const int *lda, MKL_Complex8 *x, const int *incx); 
void CTBSV(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const MKL_Complex8 *a, const int *lda, MKL_Complex8 *x, const int *incx); 
void CTPMV(const char *uplo, const char *trans, const char *diag, const int *n,
           const MKL_Complex8 *ap, MKL_Complex8 *x, const int *incx); 
void CTPSV(const char *uplo, const char *trans, const char *diag, const int *n,
           const MKL_Complex8 *ap, MKL_Complex8 *x, const int *incx); 
void CTRMV(const char *uplo, const char *transa, const char *diag, const int *n,
           const MKL_Complex8 *a, const int *lda, MKL_Complex8 *b, const int *incx); 
void CTRSV(const char *uplo, const char *trans, const char *diag, const int *n,
           const MKL_Complex8 *a, const int *lda, MKL_Complex8 *x, const int *incx); 

void DGBMV(const char *trans, const int *m, const int *n, const int *kl, const int *ku,
           const double *alpha, const double *a, const int *lda, const double *x, const int *incx,
           const double *beta, double *y, const int *incy);
void DGEMV(const char *trans, const int *m, const int *n, const double *alpha,
           const double *a, const int *lda, const double *x, const int *incx,
           const double *beta, double *y, const int *incy);
void DGER(const int *m, const int *n, const double *alpha, const double *x, const int *incx,
          const double *y, const int *incy, double *a, const int *lda);
void DSBMV(const char *uplo, const int *n, const int *k, const double *alpha,
           const double *a, const int *lda, const double *x, const int *incx,
           const double *beta, double *y, const int *incy);
void DSPMV(const char *uplo, const int *n, const double *alpha, const double *ap,
           const double *x, const int *incx, const double *beta, double *y, const int *incy);
void DSPR(const char *uplo, const int *n, const double *alpha, const double *x, const int *incx, double *ap);
void DSPR2(const char *uplo, const int *n, const double *alpha, const double *x, const int *incx, 
           const double *y, const int *incy, double *ap);
void DSYMV(const char *uplo, const int *n, const double *alpha, const double *a, const int *lda,
           const double *x, const int *incx, const double *beta, double *y, const int *incy);
void DSYR(const char *uplo, const int *n, const double *alpha, const double *x, const int *incx,
          double *a, const int *lda);
void DSYR2(const char *uplo, const int *n, const double *alpha, const double *x, const int *incx,
           const double *y, const int *incy, double *a, const int *lda);
void DTBMV(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const double *a, const int *lda, double *x, const int *incx);
void DTBSV(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const double *a, const int *lda, double *x, const int *incx);
void DTPMV(const char *uplo, const char *trans, const char *diag, const int *n,
           const double *ap, double *x, const int *incx);
void DTPSV(const char *uplo, const char *trans, const char *diag, const int *n,
           const double *ap, double *x, const int *incx);
void DTRMV(const char *uplo, const char *transa, const char *diag, const int *n,
           const double *a, const int *lda, double *b, const int *incx);
void DTRSV(const char *uplo, const char *trans, const char *diag, const int *n,
           const double *a, const int *lda, double *x, const int *incx);

void ZGBMV(const char *trans, const int *m, const int *n, const int *kl, const int *ku,
           const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
           const MKL_Complex16 *x, const int *incx, const MKL_Complex16 *beta,
           MKL_Complex16 *y, const int *incy); 
void ZGEMV(const char *trans, const int *m, const int *n, const MKL_Complex16 *alpha,
           const MKL_Complex16 *a, const int *lda, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *beta, MKL_Complex16 *y, const int *incy); 
void ZGERC(const int *m, const int *n, const MKL_Complex16 *alpha, 
           const MKL_Complex16 *x, const int *incx, const MKL_Complex16 *y, const int *incy,
           MKL_Complex16 *a, const int *lda); 
void ZGERU(const int *m, const int *n, const MKL_Complex16 *alpha,
           const MKL_Complex16 *x, const int *incx, const MKL_Complex16 *y, const int *incy,
           MKL_Complex16 *a, const int *lda); 
void ZHBMV(const char *uplo, const int *n, const int *k, const MKL_Complex16 *alpha,
           const MKL_Complex16 *a, const int *lda, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *beta, MKL_Complex16 *y, const int *incy); 
void ZHEMV(const char *uplo, const int *n, const MKL_Complex16 *alpha, 
           const MKL_Complex16 *a, const int *lda, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *beta, MKL_Complex16 *y, const int *incy); 
void ZHER(const char *uplo, const int *n, const double *alpha,
          const MKL_Complex16 *x, const int *incx, MKL_Complex16 *a, const int *lda); 
void ZHER2(const char *uplo, const int *n, const MKL_Complex16 *alpha,
           const MKL_Complex16 *x, const int *incx, const MKL_Complex16 *y, const int *incy,
           MKL_Complex16 *a, const int *lda); 
void ZHPMV(const char *uplo, const int *n, const MKL_Complex16 *alpha, const MKL_Complex16 *ap,
           const MKL_Complex16 *x, const int *incx, const MKL_Complex16 *beta,
           MKL_Complex16 *y, const int *incy); 
void ZHPR(const char *uplo, const int *n, const double *alpha, const MKL_Complex16 *x,
          const int *incx, MKL_Complex16 *ap); 
void ZHPR2(const char *uplo, const int *n, const MKL_Complex16 *alpha,
           const MKL_Complex16 *x, const int *incx, const MKL_Complex16 *y, const int *incy,
           MKL_Complex16 *ap); 
void ZTBMV(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *x, const int *incx); 
void ZTBSV(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *x, const int *incx); 
void ZTPMV(const char *uplo, const char *trans, const char *diag, const int *n,
           const MKL_Complex16 *ap, MKL_Complex16 *x, const int *incx); 
void ZTPSV(const char *uplo, const char *trans, const char *diag, const int *n,
           MKL_Complex16 *ap, MKL_Complex16 *x, const int *incx); 
void ZTRMV(const char *uplo, const char *transa, const char *diag, const int *n,
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *b, const int *incx); 
void ZTRSV(const char *uplo, const char *trans, const char *diag, const int *n,
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *x, const int *incx); 

/* BLAS Level3 */

void SGEMM(const char *transa, const char *transb, const int *m, const int *n, const int *k,
           const float *alpha, const float *a, const int *lda, const float *b, const int *ldb,
           const float *beta, float *c, const int *ldc);
void SSYMM(const char *side, const char *uplo, const int *m, const int *n, 
           const float *alpha, const float *a, const int *lda, const float *b, const int *ldb,
           const float *beta, float *c, const int *ldc);
void SSYR2K(const char *uplo, const char *trans, const int *n, const int *k, 
            const float *alpha, const float *a, const int *lda, const float *b, const int *ldb,
            const float *beta, float *c, const int *ldc);
void SSYRK(const char *uplo, const char *trans, const int *n, const int *k,
           const float *alpha, const float *a, const int *lda, 
           const float *beta, float *c, const int *ldc);
void STRMM(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const float *alpha, const float *a, const int *lda,
           float *b, const int *ldb);
void STRSM(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const float *alpha, const float *a, const int *lda,
           float *b, const int *ldb);

void CGEMM(const char *transa, const char *transb, const int *m, const int *n, const int *k,
           const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
           const MKL_Complex8 *b, const int *ldb, const MKL_Complex8 *beta,
           MKL_Complex8 *c, const int *ldc); 
void CHEMM(const char *side, const char *uplo, const int *m, const int *n, 
           const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
           const MKL_Complex8 *b, const int *ldb, const MKL_Complex8 *beta,
           MKL_Complex8 *c, const int *ldc); 
void CHER2K(const char *uplo, const char *trans, const int *n, const int *k,
            const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
            const MKL_Complex8 *b, const int *ldb, const float *beta,
            MKL_Complex8 *c, const int *ldc); 
void CHERK(const char *uplo, const char *trans, const int *n, const int *k,
           const float *alpha, const MKL_Complex8 *a, const int *lda,
           const float *beta, MKL_Complex8 *c, const int *ldc); 
void CSYMM(const char *side, const char *uplo, const int *m, const int *n, const MKL_Complex8 *alpha,
           const MKL_Complex8 *a, const int *lda, const MKL_Complex8 *b, const int *ldb,
           const MKL_Complex8 *beta, MKL_Complex8 *c, const int *ldc); 
void CSYR2K(const char *uplo, const char *trans, const int *n, const int *k,
            const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
            const MKL_Complex8 *b, const int *ldb,
            const MKL_Complex8 *beta, MKL_Complex8 *c, const int *ldc); 
void CSYRK(const char *uplo, const char *trans, const int *n, const int *k,
           const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
           const MKL_Complex8 *beta, MKL_Complex8 *c, const int *ldc); 
void CTRMM(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const MKL_Complex8 *alpha, 
           const MKL_Complex8 *a, const int *lda,
           MKL_Complex8 *b, const int *ldb); 
void CTRSM(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const MKL_Complex8 *alpha, 
           const MKL_Complex8 *a, const int *lda,
           MKL_Complex8 *b, const int *ldb); 

void DGEMM(const char *transa, const char *transb, const int *m, const int *n, const int *k,
           const double *alpha, const double *a, const int *lda, const double *b, const int *ldb,
           const double *beta, double *c, const int *ldc);
void DSYMM(const char *side, const char *uplo, const int *m, const int *n, 
           const double *alpha, const double *a, const int *lda, const double *b, const int *ldb,
           const double *beta, double *c, const int *ldc);
void DSYR2K(const char *uplo, const char *trans, const int *n, const int *k,
            const double *alpha, const double *a, const int *lda, const double *b, const int *ldb,
            double *beta, double *c, const int *ldc);
void DSYRK(const char *uplo, const char *trans, const int *n, const int *k, 
           const double *alpha, const double *a, const int *lda, const double *beta,
           double *c, const int *ldc);
void DTRMM(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const double *alpha, const double *a, const int *lda,
           double *b, const int *ldb);
void DTRSM(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const double *alpha, const double *a, const int *lda,
           double *b, const int *ldb);

void ZGEMM(const char *transa, const char *transb, const int *m, const int *n, const int *k,
           const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
           const MKL_Complex16 *b, const int *ldb, const MKL_Complex16 *beta,
           MKL_Complex16 *c, const int *ldc); 
void ZHEMM(const char *side, const char *uplo, const int *m, const int *n,
           const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
           const MKL_Complex16 *b, const int *ldb, const MKL_Complex16 *beta,
           MKL_Complex16 *c, const int *ldc); 
void ZHER2K(const char *uplo, const char *trans, const int *n, const int *k,
            const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
            const MKL_Complex16 *b, const int *ldb, const double *beta,
            MKL_Complex16 *c, const int *ldc); 
void ZHERK(const char *uplo, const char *trans, const int *n, const int *k,
           const double *alpha, const MKL_Complex16 *a, const int *lda,
           const double *beta, MKL_Complex16 *c, const int *ldc); 
void ZSYMM(const char *side, const char *uplo, const int *m, const int *n,
           const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
           const MKL_Complex16 *b, const int *ldb, const MKL_Complex16 *beta,
           MKL_Complex16 *c, const int *ldc); 
void ZSYR2K(const char *uplo, const char *trans, const int *n, const int *k,
            const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
            const MKL_Complex16 *b, const int *ldb, const MKL_Complex16 *beta,
            MKL_Complex16 *c, const int *ldc); 
void ZSYRK(const char *uplo, const char *trans, const int *n, const int *k,
           const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
           const MKL_Complex16 *beta, MKL_Complex16 *c, const int *ldc); 
void ZTRMM(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const MKL_Complex16 *alpha, 
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *b, const int *ldb); 
void ZTRSM(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const MKL_Complex16 *alpha,
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *b, const int *ldb); 

/* Lower case declaration */

/* BLAS Level1 */
double dcabs1(MKL_Complex16 *z);
float sasum(const int *n, const float *x, const int *incx);
void  saxpy(const int *n, const float *alpha, const float *x, const int *incx,
            float *y, const int *incy);
void  saxpyi(const int *nz, const float *a, const float *x, const int *indx, float *y);
float scasum(const int *n, const MKL_Complex8 *x, const int *incx); 
float scnrm2(const int *n, const MKL_Complex8 *x, const int *incx); 
void  scopy(const int *n, const float *x, const int *incx, float *y, const int *incy);
float sdot(const int *n, const float *x, const int *incx, const float *y, const int *incy);
float sdoti(const int *nz, const float *x, const int *indx, const float *y);
void  sgthr(const int *nz, const float *y, float *x, const int *indx);
void  sgthrz(const int *nz, float *y, float *x, const int *indx);
float snrm2(const int *n, const float *x, const int *incx);
void  srot(const int *n, float *x, const int *incx, float *y, const int *incy,
           const float *c, const float *s);
void  srotg(float *a,float *b,float *c,float *s);
void  sroti(const int *nz, float *x, const int *indx, float *y, const float *c, const float *s);
void  srotm(const int *n, float *x, const int *incx, float *y, const int *incy, const float *param);
void  srotmg(float *d1, float *d2, float *x1, const float *y1, float *param);
void  sscal(const int *n, const float *a, float *x, const int *incx);
void  ssctr(const int *nz, const float *x, const int *indx, float *y);
void  sswap(const int *n, float *x, const int *incx, float *y, const int *incy);
int   isamax(const int *n, const float *x, const int *incx);
int   isamin(const int *n, const float *x, const int *incx);

void caxpy(const int *n, const MKL_Complex8 *alpha, const MKL_Complex8 *x, const int *incx,
           MKL_Complex8 *y, const int *incy); 
void caxpyi(const int *nz, const MKL_Complex8 *a, const MKL_Complex8 *x, const int *indx,
            MKL_Complex8 *y); 
void ccopy(const int *n, const MKL_Complex8 *x, const int *incx, MKL_Complex8 *y, const int *incy); 
void cdotc(MKL_Complex8 *pres, const int *n, const MKL_Complex8 *x, const int *incx,
           const MKL_Complex8 *y, const int *incy); 
void cdotci(MKL_Complex8 *pres, const int *nz, const MKL_Complex8 *x, const int *indx,
            const MKL_Complex8 *y); 
void cdotu(MKL_Complex8 *pres, const int *n, const MKL_Complex8 *x, const int *incx,
           const MKL_Complex8 *y, const int *incy); 
void cdotui(MKL_Complex8 *pres, const int *nz, const MKL_Complex8 *x, const int *indx,
            const MKL_Complex8 *y); 
void cgthr(const int *nz, const MKL_Complex8 *y, MKL_Complex8 *x, const int *indx); 
void cgthrz(const int *nz, MKL_Complex8 *y, MKL_Complex8 *x, const int *indx); 
void crotg(MKL_Complex8 *a, const MKL_Complex8 *b, float *c, MKL_Complex8 *s); 
void cscal(const int *n, const MKL_Complex8 *a, MKL_Complex8 *x, const int *incx); 
void csctr(const int *nz, const MKL_Complex8 *x, const int *indx, MKL_Complex8 *y); 
void csrot(const int *n, MKL_Complex8 *x, const int *incx, MKL_Complex8 *y, const int *incy,
           const float *c, const float *s); 
void csscal(const int *n, const float *a, MKL_Complex8 *x, const int *incx); 
void cswap(const int *n, MKL_Complex8 *x, const int *incx, MKL_Complex8 *y, const int *incy); 
int  icamax(const int *n, const MKL_Complex8 *x, const int *incx); 
int  icamin(const int *n, const MKL_Complex8 *x, const int *incx); 

double dasum(const int *n, const double *x, const int *incx);
void   daxpy(const int *n, const double *alpha, const double *x, const int *incx,
             double *y, const int *incy);
void   daxpyi(const int *nz, const double *a, const double *x, const int *indx, double *y);
void   dcopy(const int *n, const double *x, const int *incx, double *y, const int *incy);
double ddot(const int *n, const double *x, const int *incx, const double *y, const int *incy);
double ddoti(const int *nz, const double *x, const int *indx, const double *y);
void   dgthr(const int *nz, const double *y, double *x, const int *indx);
void   dgthrz(const int *nz, double *y, double *x, const int *indx);
double dnrm2(const int *n, const double *x, const int *incx);
void   drot(const int *n, double *x, const int *incx, double *y, const int *incy,
            const double *c, const double *s);
void   drotg(double *a, double *b, double *c, double *s);
void   droti(const int *nz, double *x, const int *indx, double *y, const double *c, const double *s);
void   drotm(const int *n, double *x, const int *incx, double *y, const int *incy, const double *param);
void   drotmg(double *d1, double *d2, double *x1, const double *y1, double *param);
void   dscal(const int *n, const double *a, double *x, const int *incx);
void   dsctr(const int *nz, const double *x, const int *indx, double *y);
void   dswap(const int *n, double *x, const int *incx, double *y, const int *incy);
double dzasum(const int *n, const MKL_Complex16 *x, const int *incx); 
double dznrm2(const int *n, const MKL_Complex16 *x, const int *incx); 
int    idamax(const int *n, const double *x, const int *incx);
int    idamin(const int *n, const double *x, const int *incx);

void zaxpy(const int *n, const MKL_Complex16 *alpha, const MKL_Complex16 *x, const int *incx,
           MKL_Complex16 *y, const int *incy); 
void zaxpyi(const int *nz, const MKL_Complex16 *a, const MKL_Complex16 *x, const int *indx,
            MKL_Complex16 *y); 
void zcopy(const int *n, const MKL_Complex16 *x, const int *incx,
           MKL_Complex16 *y, const int *incy); 
void zdotc(MKL_Complex16 *pres, const int *n, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *y, const int *incy); 
void zdotci(MKL_Complex16 *pres, const int *nz, const MKL_Complex16 *x, const int *indx,
            const MKL_Complex16 *y); 
void zdotu(MKL_Complex16 *pres, const int *n, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *y, const int *incy); 
void zdotui(MKL_Complex16 *pres, const int *nz, const MKL_Complex16 *x, const int *indx,
            const MKL_Complex16 *y); 
void zdrot(const int *n, MKL_Complex16 *x, const int *incx, MKL_Complex16 *y, const int *incy,
           const double *c, const double *s); 
void zdscal(const int *n, const double *a, MKL_Complex16 *x, const int *incx); 
void zgthr(const int *nz, const MKL_Complex16 *y, MKL_Complex16 *x, const int *indx); 
void zgthrz(const int *nz, MKL_Complex16 *y, MKL_Complex16 *x, const int *indx); 
void zrotg(MKL_Complex16 *a, const MKL_Complex16 *b, double *c, MKL_Complex16 *s); 
void zscal(const int *n, const MKL_Complex16 *a, MKL_Complex16 *x, const int *incx); 
void zsctr(const int *nz, const MKL_Complex16 *x, const int *indx, MKL_Complex16 *y); 
void zswap(const int *n, MKL_Complex16 *x, const int *incx, MKL_Complex16 *y, const int *incy); 
int  izamax(const int *n, const MKL_Complex16 *x, const int *incx); 
int  izamin(const int *n, const MKL_Complex16 *x, const int *incx); 

/* blas level2 */

void sgbmv(const char *trans, const int *m, const int *n, const int *kl, const int *ku,
           const float *alpha, const float *a, const int *lda, const float *x, const int *incx,
           const float *beta, float *y, const int *incy);
void sgemv(const char *trans, const int *m, const int *n, const float *alpha,
           const float *a, const int *lda, const float *x, const int *incx,
           const float *beta, float *y, const int *incy);
void sger(const int *m, const int *n, const float *alpha, const float *x, const int *incx,
          const float *y, const int *incy, float *a, const int *lda);
void ssbmv(const char *uplo, const int *n, const int *k, const float *alpha,
           const float *a, const int *lda, const float *x, const int *incx,
           const float *beta, float *y, const int *incy);
void sspmv(const char *uplo, const int *n, const float *alpha, const float *ap,
           const float *x, const int *incx, const float *beta, float *y, const int *incy);
void sspr(const char *uplo, const int *n, const float *alpha, const float *x, const int *incx,
          float *ap);
void sspr2(const char *uplo, const int *n, const float *alpha, const float *x, const int *incx,
           const float *y, const int *incy, float *ap);
void ssymv(const char *uplo, const int *n, const float *alpha, const float *a, const int *lda,
           const float *x, const int *incx, const float *beta, float *y, const int *incy);
void ssyr(const char *uplo, const int *n, const float *alpha, const float *x, const int *incx,
          float *a, const int *lda);
void ssyr2(const char *uplo, const int *n, const float *alpha, const float *x, const int *incx,
           const float *y, const int *incy, float *a, const int *lda);
void stbmv(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const float *a, const int *lda, float *x, const int *incx);
void stbsv(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const float *a, const int *lda, float *x, const int *incx);
void stpmv(const char *uplo, const char *trans, const char *diag, const int *n, const float *ap,
           float *x, const int *incx);
void stpsv(const char *uplo, const char *trans, const char *diag, const int *n, const float *ap,
           float *x, const int *incx);
void strmv(const char *uplo, const char *transa, const char *diag, const int *n, const float *a,
           const int *lda, float *b, const int *incx);
void strsv(const char *uplo, const char *trans, const char *diag, const int *n,
           const float *a, const int *lda, float *x, const int *incx);

void cgbmv(const char *trans, const int *m, const int *n, const int *kl, const int *ku,
           const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
           const MKL_Complex8 *x, const int *incx, const MKL_Complex8 *beta,
           MKL_Complex8 *y, const int *incy); 
void cgemv(const char *trans, const int *m, const int *n, const MKL_Complex8 *alpha,
           const MKL_Complex8 *a, const int *lda, const MKL_Complex8 *x, const int *incx,
           const MKL_Complex8 *beta, MKL_Complex8 *y, const int *incy); 
void cgerc(const int *m, const int *n, const MKL_Complex8 *alpha, 
           const MKL_Complex8 *x, const int *incx, const MKL_Complex8 *y, const int *incy,
           MKL_Complex8 *a, const int *lda); 
void cgeru(const int *m, const int *n, const MKL_Complex8 *alpha, 
           const MKL_Complex8 *x, const int *incx, const MKL_Complex8 *y, const int *incy,
           MKL_Complex8 *a, const int *lda); 
void chbmv(const char *uplo, const int *n, const int *k, const MKL_Complex8 *alpha,
           const MKL_Complex8 *a, const int *lda, const MKL_Complex8 *x, const int *incx,
           const MKL_Complex8 *beta, MKL_Complex8 *y, const int *incy); 
void chemv(const char *uplo, const int *n, const MKL_Complex8 *alpha,
           const MKL_Complex8 *a, const int *lda, const MKL_Complex8 *x, const int *incx,
           const MKL_Complex8 *beta, MKL_Complex8 *y, const int *incy); 
void cher(const char *uplo, const int *n, const float *alpha, const MKL_Complex8 *x, const int *incx,
          MKL_Complex8 *a, const int *lda); 
void cher2(const char *uplo, const int *n, const MKL_Complex8 *alpha,
           const MKL_Complex8 *x, const int *incx, const MKL_Complex8 *y, const int *incy,
           MKL_Complex8 *a, const int *lda); 
void chpmv(const char *uplo, const int *n, const MKL_Complex8 *alpha, const MKL_Complex8 *ap,
           const MKL_Complex8 *x, const int *incx, const MKL_Complex8 *beta,
           MKL_Complex8 *y, const int *incy); 
void chpr(const char *uplo, const int *n, const float *alpha, const MKL_Complex8 *x, const int *incx,
          MKL_Complex8 *ap); 
void chpr2(const char *uplo, const int *n, const MKL_Complex8 *alpha, const MKL_Complex8 *x, const int *incx,
           const MKL_Complex8 *y, const int *incy, MKL_Complex8 *ap); 
void ctbmv(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const MKL_Complex8 *a, const int *lda, MKL_Complex8 *x, const int *incx); 
void ctbsv(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const MKL_Complex8 *a, const int *lda, MKL_Complex8 *x, const int *incx); 
void ctpmv(const char *uplo, const char *trans, const char *diag, const int *n,
           const MKL_Complex8 *ap, MKL_Complex8 *x, const int *incx); 
void ctpsv(const char *uplo, const char *trans, const char *diag, const int *n,
           const MKL_Complex8 *ap, MKL_Complex8 *x, const int *incx); 
void ctrmv(const char *uplo, const char *transa, const char *diag, const int *n,
           const MKL_Complex8 *a, const int *lda, MKL_Complex8 *b, const int *incx); 
void ctrsv(const char *uplo, const char *trans, const char *diag, const int *n,
           const MKL_Complex8 *a, const int *lda, MKL_Complex8 *x, const int *incx); 

void dgbmv(const char *trans, const int *m, const int *n, const int *kl, const int *ku,
           const double *alpha, const double *a, const int *lda, const double *x, const int *incx,
           const double *beta, double *y, const int *incy);
void dgemv(const char *trans, const int *m, const int *n, const double *alpha,
           const double *a, const int *lda, const double *x, const int *incx,
           const double *beta, double *y, const int *incy);
void dger(const int *m, const int *n, const double *alpha, const double *x, const int *incx,
          const double *y, const int *incy, double *a, const int *lda);
void dsbmv(const char *uplo, const int *n, const int *k, const double *alpha,
           const double *a, const int *lda, const double *x, const int *incx,
           const double *beta, double *y, const int *incy);
void dspmv(const char *uplo, const int *n, const double *alpha, const double *ap,
           const double *x, const int *incx, const double *beta,
           double *y, const int *incy);
void dspr(const char *uplo, const int *n, const double *alpha, const double *x, const int *incx,
          double *ap);
void dspr2(const char *uplo, const int *n, const double *alpha, const double *x, const int *incx,
           const double *y, const int *incy, double *ap);
void dsymv(const char *uplo, const int *n, const double *alpha, const double *a, const int *lda,
           const double *x, const int *incx, const double *beta, double *y, const int *incy);
void dsyr(const char *uplo, const int *n, const double *alpha, const double *x, const int *incx,
          double *a, const int *lda);
void dsyr2(const char *uplo, const int *n, const double *alpha, const double *x, const int *incx,
           const double *y, const int *incy, double *a, const int *lda);
void dtbmv(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const double *a, const int *lda, double *x, const int *incx);
void dtbsv(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const double *a, const int *lda, double *x, const int *incx);
void dtpmv(const char *uplo, const char *trans, const char *diag, const int *n,
           const double *ap, double *x, const int *incx);
void dtpsv(const char *uplo, const char *trans, const char *diag, const int *n,
           const double *ap, double *x, const int *incx);
void dtrmv(const char *uplo, const char *transa, const char *diag, const int *n,
           const double *a, const int *lda, double *b, const int *incx);
void dtrsv(const char *uplo, const char *trans, const char *diag, const int *n,
           const double *a, const int *lda, double *x, const int *incx);

void zgbmv(const char *trans, const int *m, const int *n, const int *kl, const int *ku,
           const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
           const MKL_Complex16 *x, const int *incx, const MKL_Complex16 *beta,
           MKL_Complex16 *y, const int *incy); 
void zgemv(const char *trans, const int *m, const int *n, const MKL_Complex16 *alpha,
           const MKL_Complex16 *a, const int *lda, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *beta, MKL_Complex16 *y, const int *incy); 
void zgerc(const int *m, const int *n, const MKL_Complex16 *alpha, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *y, const int *incy, MKL_Complex16 *a, const int *lda); 
void zgeru(const int *m, const int *n, const MKL_Complex16 *alpha, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *y, const int *incy, MKL_Complex16 *a, const int *lda); 
void zhbmv(const char *uplo, const int *n, const int *k, const MKL_Complex16 *alpha,
           const MKL_Complex16 *a, const int *lda, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *beta, MKL_Complex16 *y, const int *incy); 
void zhemv(const char *uplo, const int *n, const MKL_Complex16 *alpha,
           const MKL_Complex16 *a, const int *lda, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *beta, MKL_Complex16 *y, const int *incy); 
void zher(const char *uplo, const int *n, const double *alpha, const MKL_Complex16 *x, const int *incx,
          MKL_Complex16 *a, const int *lda); 
void zher2(const char *uplo, const int *n, const MKL_Complex16 *alpha,
           const MKL_Complex16 *x, const int *incx, const MKL_Complex16 *y, const int *incy,
           MKL_Complex16 *a, const int *lda); 
void zhpmv(const char *uplo, const int *n, const MKL_Complex16 *alpha, const MKL_Complex16 *ap,
           const MKL_Complex16 *x, const int *incx, const MKL_Complex16 *beta,
           MKL_Complex16 *y, const int *incy); 
void zhpr(const char *uplo, const int *n, const double *alpha, const MKL_Complex16 *x, const int *incx,
          MKL_Complex16 *ap); 
void zhpr2(const char *uplo, const int *n, const MKL_Complex16 *alpha, const MKL_Complex16 *x, const int *incx,
           const MKL_Complex16 *y, const int *incy, MKL_Complex16 *ap); 
void ztbmv(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *x, const int *incx); 
void ztbsv(const char *uplo, const char *trans, const char *diag, const int *n, const int *k,
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *x, const int *incx); 
void ztpmv(const char *uplo, const char *trans, const char *diag, const int *n,
           const MKL_Complex16 *ap, MKL_Complex16 *x, const int *incx); 
void ztpsv(const char *uplo, const char *trans, const char *diag, const int *n,
           const MKL_Complex16 *ap, MKL_Complex16 *x, const int *incx); 
void ztrmv(const char *uplo, const char *transa, const char *diag, const int *n,
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *b, const int *incx); 
void ztrsv(const char *uplo, const char *trans, const char *diag, const int *n,
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *x, const int *incx); 

/* blas level3 */

void sgemm(const char *transa, const char *transb, const int *m, const int *n, const int *k,
           const float *alpha, const float *a, const int *lda, const float *b, const int *ldb,
           const float *beta, float *c, const int *ldc);
void ssymm(const char *side, const char *uplo, const int *m, const int *n,
           const float *alpha, const float *a, const int *lda, const float *b, const int *ldb,
           const float *beta, float *c, const int *ldc);
void ssyr2k(const char *uplo, const char *trans, const int *n, const int *k,
            const float *alpha, const float *a, const int *lda, const float *b, const int *ldb,
            float *beta, float *c, const int *ldc);
void ssyrk(const char *uplo, const char *trans, const int *n, const int *k,
           const float *alpha, const float *a, const int *lda, const float *beta,
           float *c, const int *ldc);
void strmm(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const float *alpha, const float *a, const int *lda,
           float *b, const int *ldb);
void strsm(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const float *alpha, const float *a, const int *lda,
           float *b, const int *ldb);

void cgemm(const char *transa, const char *transb, const int *m, const int *n, const int *k,
           const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
           const MKL_Complex8 *b, const int *ldb, const MKL_Complex8 *beta,
           MKL_Complex8 *c, const int *ldc); 
void chemm(const char *side, const char *uplo, const int *m, const int *n,
           const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
           const MKL_Complex8 *b, const int *ldb, const MKL_Complex8 *beta,
           MKL_Complex8 *c, const int *ldc); 
void cher2k(const char *uplo, const char *trans, const int *n, const int *k,
            const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
            const MKL_Complex8 *b, const int *ldb, const float *beta,
            MKL_Complex8 *c, const int *ldc); 
void cherk(const char *uplo, const char *trans, const int *n, const int *k,
           const float *alpha, const MKL_Complex8 *a, const int *lda, const float *beta,
           MKL_Complex8 *c, const int *ldc); 
void csymm(const char *side, const char *uplo, const int *m, const int *n,
           const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
           const MKL_Complex8 *b, const int *ldb, const MKL_Complex8 *beta,
           MKL_Complex8 *c, const int *ldc); 
void csyr2k(const char *uplo, const char *trans, const int *n, const int *k,
            const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
            const MKL_Complex8 *b, const int *ldb, const MKL_Complex8 *beta,
            MKL_Complex8 *c, const int *ldc); 
void csyrk(const char *uplo, const char *trans, const int *n, const int *k,
           const MKL_Complex8 *alpha, const MKL_Complex8 *a, const int *lda,
           const MKL_Complex8 *beta, MKL_Complex8 *c, const int *ldc); 
void ctrmm(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const MKL_Complex8 *alpha, 
           const MKL_Complex8 *a, const int *lda, MKL_Complex8 *b, const int *ldb); 
void ctrsm(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const MKL_Complex8 *alpha,
           const MKL_Complex8 *a, const int *lda, MKL_Complex8 *b, const int *ldb); 

void dgemm(const char *transa, const char *transb, const int *m, const int *n, const int *k,
           const double *alpha, const double *a, const int *lda, const double *b, const int *ldb,
           const double *beta, double *c, const int *ldc);
void dsymm(const char *side, const char *uplo, const int *m, const int *n,
           const double *alpha, const double *a, const int *lda, const double *b, const int *ldb,
           const double *beta, double *c, const int *ldc);
void dsyr2k(const char *uplo, const char *trans, const int *n, const int *k,
            const double *alpha, const double *a, const int *lda, const double *b, const int *ldb,
            const double *beta, double *c, const int *ldc);
void dsyrk(const char *uplo, const char *trans, const int *n, const int *k,
           const double *alpha, const double *a, const int *lda, const double *beta,
           double *c, const int *ldc);
void dtrmm(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const double *alpha, const double *a, const int *lda,
           double *b, const int *ldb);
void dtrsm(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const double *alpha, const double *a, const int *lda,
           double *b, const int *ldb);

void zgemm(const char *transa, const char *transb, const int *m, const int *n, const int *k,
           const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
           const MKL_Complex16 *b, const int *ldb, const MKL_Complex16 *beta,
           MKL_Complex16 *c, const int *ldc); 
void zhemm(const char *side, const char *uplo, const int *m, const int *n,
           const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
           const MKL_Complex16 *b, const int *ldb, const MKL_Complex16 *beta,
           MKL_Complex16 *c, const int *ldc); 
void zher2k(const char *uplo, const char *trans, const int *n, const int *k,
            const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
            const MKL_Complex16 *b, const int *ldb, const double *beta,
            MKL_Complex16 *c, const int *ldc); 
void zherk(const char *uplo, const char *trans, const int *n, const int *k,
           const double *alpha, const MKL_Complex16 *a, const int *lda,
           const double *beta, MKL_Complex16 *c, const int *ldc); 
void zsymm(const char *side, const char *uplo, const int *m, const int *n,
           const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
           const MKL_Complex16 *b, const int *ldb, const MKL_Complex16 *beta,
           MKL_Complex16 *c, const int *ldc); 
void zsyr2k(const char *uplo, const char *trans, const int *n, const int *k,
            const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
            const MKL_Complex16 *b, const int *ldb, const MKL_Complex16 *beta,
            MKL_Complex16 *c, const int *ldc); 
void zsyrk(const char *uplo, const char *trans, const int *n, const int *k,
           const MKL_Complex16 *alpha, const MKL_Complex16 *a, const int *lda,
           const MKL_Complex16 *beta, MKL_Complex16 *c, const int *ldc); 
void ztrmm(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const MKL_Complex16 *alpha,
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *b, const int *ldb); 
void ztrsm(const char *side, const char *uplo, const char *transa, const char *diag,
           const int *m, const int *n, const MKL_Complex16 *alpha, 
           const MKL_Complex16 *a, const int *lda, MKL_Complex16 *b, const int *ldb); 

/* MKL support */

void MKLGetVersion(MKLVersion *ver);
void MKLGetVersionString(char * buffer, int len);
void MKL_FreeBuffers(void);

int ILAENV(const int *ispec, const char *name, const char *opts,
           const int *n1, const int *n2, const int *n3, const int *n4);
int ilaenv(const int *ispec, const char *name, const char *opts,
           const int *n1, const int *n2, const int *n3, const int *n4);     


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MKL_BLAS_H_ */

/*******************************************************************************
!                             INTEL CONFIDENTIAL
!   Copyright(C) 2005-2006 Intel Corporation. All Rights Reserved.
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
!    Intel(R) Math Kernel Library (MKL) interface for Sparse BLAS level 2,3 routines
!******************************************************************************/

#ifndef _MKL_SPBLAS_H_
#define _MKL_SPBLAS_H_

#include "mkl_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Sparse BLAS Level2 lower case */

void mkl_dcsrmv (char *transa, int *m, int *k, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *x, double *beta, double *y);
void mkl_dcsrsv(char *transa, int *m, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *x, double *y);
void mkl_dcsrgemv(char *transa, int *m, double *a, int *ia,  int *ja, double *x,  double *y);
void mkl_dcsrsymv(char *uplo, int *m, double *a, int *ia,  int *ja, double *x,  double *y);
void mkl_dcsrtrsv(char *uplo, char *transa, char *diag, int *m, double *a, int *ia,  int *ja, double *x,  double *y);

void mkl_dcscmv(char *transa, int *m, int *k, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *x, double *beta, double *y);
void mkl_dcscsv(char *transa, int *m, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *x, double *y);

void mkl_dcoomv(char *transa, int *m, int *k, double *alpha, char *matdescra, double  *val, int *rowind,  int *colind, int *nnz, double *x, double *beta, double *y);
void mkl_dcoosv(char *transa, int *m, double *alpha, char *matdescra, double  *val, int *rowind,  int *colind, int *nnz, double *x, double *y);
void mkl_dcoogemv(char *transa, int *m, double *val, int *rowind,  int *colind, int *nnz, double *x,  double *y);
void mkl_dcoosymv(char *uplo, int *m, double *val, int *rowind,  int *colind, int *nnz, double *x,  double *y);
void mkl_dcootrsv(char *uplo, char *transa, char *diag, int *m, double *val, int *rowind, int *colind, int *nnz, double *x,  double *y);

void mkl_ddiamv (char *transa, int *m, int *k, double *alpha, char *matdescra, double  *val, int *lval, int *idiag,  int *ndiag, double *x, double *beta, double *y);
void mkl_ddiasv (char *transa, int *m, double *alpha, char *matdescra, double  *val, int *lval, int *idiag,  int *ndiag, double *x, double *y);
void mkl_ddiagemv(char *transa, int *m, double *val, int *lval,  int *idiag, int *ndiag, double *x,  double *y);
void mkl_ddiasymv(char *uplo, int *m, double *val, int *lval,  int *idiag, int *ndiag, double *x,  double *y);
void mkl_ddiatrsv(char *uplo, char *transa, char *diag, int *m, double *val, int *lval,  int  *idiag, int *ndiag, double *x,  double *y);

void mkl_dskymv (char *transa, int *m, int *k, double *alpha, char *matdescra, double  *val, int *pntr, double *x, double *beta, double *y);
void mkl_dskysv(char *transa, int *m, double *alpha, char *matdescra, double  *val, int *pntr,  double *x, double *y);

/* Sparse BLAS Level3 lower case */

void mkl_dcsrmm(char *transa, int *m, int *n, int *k, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *b, int *ldb, double *beta, double *c, int *ldc);
void mkl_dcsrsm(char *transa, int *m, int *n, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *b, int *ldb,  double *c, int *ldc);

void mkl_dcscmm(char *transa, int *m, int *n, int *k, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *b, int *ldb, double *beta, double *c, int *ldc);
void mkl_dcscsm(char *transa, int *m, int *n, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *b, int *ldb,  double *c, int *ldc);

void mkl_dcoomm(char *transa, int *m, int *n, int *k, double *alpha, char *matdescra, double  *val, int *rowind,  int *colind, int *nnz, double *b, int *ldb, double *beta, double *c, int *ldc);
void mkl_dcoosm(char *transa, int *m, int *n, double *alpha, char *matdescra, double  *val, int *rowind,  int *colind, int *nnz, double *b, int *ldb,  double *c, int *ldc);

void mkl_ddiamm (char *transa, int *m, int *n, int *k, double *alpha, char *matdescra, double  *val, int *lval, int *idiag,  int *ndiag, double *b, int *ldb, double *beta, double *c, int *ldc);
void mkl_ddiasm (char *transa, int *m, int *n, double *alpha, char *matdescra, double  *val, int *lval, int *idiag,  int *ndiag, double *b, int *ldb, double *c, int *ldc);

void mkl_dskysm (char *transa, int *m, int *n, double *alpha, char *matdescra, double  *val, int *pntr,  double *b, int *ldb, double *c, int *ldc);
void mkl_dskymm (char *transa, int *m, int *n, int *k, double *alpha, char *matdescra, double  *val, int *pntr, double *b, int *ldb, double *beta, double *c, int *ldc);

/* Upper case declaration */
/* Sparse BLAS Level2 upper case */


void MKL_DCSRMV (char *transa, int *m, int *k, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *x, double *beta, double *y);
void MKL_DCSRSV(char *transa, int *m, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *x, double *y);
void MKL_DCSRGEMV(char *transa, int *m, double *a, int *ia,  int *ja, double *x,  double *y);
void MKL_DCSRSYMV(char *uplo, int *m, double *a, int *ia,  int *ja, double *x,  double *y);
void MKL_DCSRTRSV(char *uplo, char *transa, char *diag, int *m, double *a, int *ia,  int *ja, double *x,  double *y);

void MKL_DCSCMV(char *transa, int *m, int *k, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *x, double *beta, double *y);
void MKL_DCSCSV(char *transa, int *m, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *x, double *y);

void MKL_DCOOMV(char *transa, int *m, int *k, double *alpha, char *matdescra, double  *val, int *rowind,  int *colind, int *nnz, double *x, double *beta, double *y);
void MKL_DCOOSV(char *transa, int *m, double *alpha, char *matdescra, double  *val, int *rowind,  int *colind, int *nnz, double *x, double *y);
void MKL_DCOOGEMV(char *transa, int *m, double *val, int *rowind,  int *colind, int *nnz, double *x,  double *y);
void MKL_DCOOSYMV(char *uplo, int *m, double *val, int *rowind,  int *colind, int *nnz, double *x,  double *y);
void MKL_DCOOTRSV(char *uplo, char *transa, char *diag, int *m, double *val, int *rowind, int *colind, int *nnz, double *x,  double *y);

void MKL_DDIAMV (char *transa, int *m, int *k, double *alpha, char *matdescra, double  *val, int *lval, int *idiag,  int *ndiag, double *x, double *beta, double *y);
void MKL_DDIASV (char *transa, int *m, double *alpha, char *matdescra, double  *val, int *lval, int *idiag,  int *ndiag, double *x, double *y);
void MKL_DDIAGEMV(char *transa, int *m, double *val, int *lval,  int *idiag, int *ndiag, double *x,  double *y);
void MKL_DDIASYMV(char *uplo, int *m, double *val, int *lval,  int *idiag, int *ndiag, double *x,  double *y);
void MKL_DDIATRSV(char *uplo, char *transa, char *diag, int *m, double *val, int *lval,  int  *idiag, int *ndiag, double *x,  double *y);

void MKL_DSKYMV (char *transa, int *m, int *k, double *alpha, char *matdescra, double  *val, int *pntr, double *x, double *beta, double *y);
void MKL_DSKYSV(char *transa, int *m, double *alpha, char *matdescra, double  *val, int *pntr,  double *x, double *y);

/* Sparse BLAS Level2 upper case */

void MKL_DCSRMM(char *transa, int *m, int *n, int *k, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *b, int *ldb, double *beta, double *c, int *ldc);
void MKL_DCSRSM(char *transa, int *m, int *n, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *b, int *ldb,  double *c, int *ldc);

void MKL_DCSCMM(char *transa, int *m, int *n, int *k, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *b, int *ldb, double *beta, double *c, int *ldc);
void MKL_DCSCSM(char *transa, int *m, int *n, double *alpha, char *matdescra, double  *val, int *indx,  int *pntrb, int *pntre, double *b, int *ldb,  double *c, int *ldc);

void MKL_DCOOMM(char *transa, int *m, int *n, int *k, double *alpha, char *matdescra, double  *val, int *rowind,  int *colind, int *nnz, double *b, int *ldb, double *beta, double *c, int *ldc);
void MKL_DCOOSM(char *transa, int *m, int *n, double *alpha, char *matdescra, double  *val, int *rowind,  int *colind, int *nnz, double *b, int *ldb,  double *c, int *ldc);

void MKL_DDIAMM (char *transa, int *m, int *n, int *k, double *alpha, char *matdescra, double  *val, int *lval, int *idiag,  int *ndiag, double *b, int *ldb, double *beta, double *c, int *ldc);
void MKL_DDIASM (char *transa, int *m, int *n, double *alpha, char *matdescra, double  *val, int *lval, int *idiag,  int *ndiag, double *b, int *ldb, double *c, int *ldc);

void MKL_DSKYSM (char *transa, int *m, int *n, double *alpha, char *matdescra, double  *val, int *pntr,  double *b, int *ldb, double *c, int *ldc);
void MKL_DSKYMM (char *transa, int *m, int *n, int *k, double *alpha, char *matdescra, double  *val, int *pntr, double *b, int *ldb, double *beta, double *c, int *ldc);


/* MKL support */

void MKLGetVersion(MKLVersion *ver);
void MKLGetVersionString(char * buffer, int len);
void MKL_FreeBuffers(void);

    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MKL_SPBLAS_H_ */

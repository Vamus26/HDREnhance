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
!    Intel(R) Math Kernel Library (MKL) interface for Sparse BLAS level 2-3  routines
!******************************************************************************/

#ifndef _MKL_RCISOLVER_H_
#define _MKL_RCISOLVER_H_

#include "mkl_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* PCG Lower case */

void dcg_init_(int *n, double *x,double *b, int *rci_request, int *ipar, double *dpar, double *tmp);
void dcg_check_(int *n, double *x,double *b, int *rci_request, int *ipar, double *dpar, double *tmp);
void dcg_(int *n, double *x,double *b, int *rci_request, int *ipar, double *dpar, double *tmp);
void dcg_get_(int *n, double *x, double *b, int *rci_request, int *ipar, double *dpar, double *tmp, int *itercount);

/* PCG upper case */

void DCG_INIT(int *n, double *x,double *b, int *rci_request, int *ipar, double *dpar, double *tmp);
void DCG_CHECK(int *n, double *x,double *b, int *rci_request, int *ipar, double *dpar, double *tmp);
void DCG(int *n, double *x,double *b, int *rci_request, int *ipar, double *dpar, double *tmp);
void DCG_GET(int *n, double *x, double *b, int *rci_request, int *ipar, double *dpar, double *tmp, int *itercount);


    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MKL_RCISOLVER_H_ */

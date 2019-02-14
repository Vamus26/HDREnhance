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
!      Intel(R) Math Kernel Library (MKL) examples interface
!******************************************************************************/

#ifndef _MKL_EXAMPLE_H_
#define _MKL_EXAMPLE_H_

#include "mkl_types.h"
#include "mkl_cblas.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define COMMENTS ':'
#define MAX_STRING_LEN  512

#define MIN(a,b)  (((a) > (b)) ? (b) : (a))  
#define MAX(a,b)  (((a) > (b)) ? (a) : (b))


#define GENERAL_MATRIX  0
#define UPPER_MATRIX    1
#define LOWER_MATRIX   -1

#define FULLPRINT  0
#define SHORTPRINT 1


/*
 * ===========================================
 * Prototypes for example program functions 
 * ===========================================
 */

int GetIntegerParameters(FILE*, ...);
int GetScalarsD(FILE*, ...);
int GetScalarsS(FILE*, ...);
int GetScalarsC(FILE*, ...);
int GetScalarsZ(FILE*, ...);
int GetVectorS(FILE*, int, float*, int);
int GetVectorC(FILE*, int, MKL_Complex8*, int);
int GetVectorD(FILE*, int, double*, int);
int GetVectorZ(FILE*, int, MKL_Complex16*, int);
int GetArrayS(FILE*, CBLAS_ORDER*, int, int*, int*, float*, int*);
int GetArrayD(FILE*, CBLAS_ORDER*, int, int*, int*, double*, int*);
int GetArrayC(FILE*, CBLAS_ORDER*, int, int*, int*, MKL_Complex8*, int*);
int GetArrayZ(FILE*, CBLAS_ORDER*, int, int*, int*, MKL_Complex16*, int*);
int GetBandArrayS(FILE*, CBLAS_ORDER*, int, int, int, int, float*, int); 
int GetBandArrayD(FILE*, CBLAS_ORDER*, int, int, int, int, double*, int); 
int GetBandArrayC(FILE*, CBLAS_ORDER*, int, int, int, int, MKL_Complex8*, int); 
int GetBandArrayZ(FILE*, CBLAS_ORDER*, int, int, int, int, MKL_Complex16*, int); 
int GetValuesI(FILE *, int *, int, int);
int GetValuesC(FILE *, MKL_Complex8*, int, int, int);
int GetValuesZ(FILE *, MKL_Complex16*, int, int, int);
int GetValuesD(FILE *, double*, int, int, int);
int GetValuesS(FILE *, float*, int, int, int);
void PrintParameters( char*, ... );
void PrintTrans(int, CBLAS_TRANSPOSE*, CBLAS_TRANSPOSE*);
void PrintOrder(CBLAS_ORDER*);
void PrintVectorS(int, int, float*, int, char*);
void PrintVectorC(int, int, MKL_Complex8*, int, char*);
void PrintVectorD(int, int, double*, int, char*);
void PrintArrayS(CBLAS_ORDER*, int, int, int*, int*, float*, int*, char*);
void PrintArrayD(CBLAS_ORDER*, int, int, int*, int*, double*, int*, char*);
void PrintArrayC(CBLAS_ORDER*, int, int, int*, int*, MKL_Complex8*, int*, char*);
void PrintArrayZ(CBLAS_ORDER*, int, int, int*, int*, MKL_Complex16*, int*, char*);
void PrintBandArrayS(CBLAS_ORDER*, int, int, int, int, int, float*, int, char*);
void PrintBandArrayD(CBLAS_ORDER*, int, int, int, int, int, double*, int, char*);
void PrintBandArrayC(CBLAS_ORDER*, int, int, int, int, int, MKL_Complex8*, int, char*);
void PrintBandArrayZ(CBLAS_ORDER*, int, int, int, int, int, MKL_Complex16*, int, char*);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MKL_EXAMPLE_H_ */

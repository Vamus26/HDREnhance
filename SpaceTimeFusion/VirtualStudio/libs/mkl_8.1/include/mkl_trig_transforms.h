/*******************************************************************************
!                             INTEL CONFIDENTIAL
!   Copyright(C) 2006 Intel Corporation. All Rights Reserved.
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
!      Intel(R) Math Kernel Library (MKL) interface for TT routines
!******************************************************************************/

#ifndef _MKL_TRIG_TRANSFORMS_H_
#define _MKL_TRIG_TRANSFORMS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Parameters definitions for the kind of the Trigonometric Transform: */
#define MKL_SINE_TRANSFORM             0
#define MKL_COSINE_TRANSFORM           1 
#define MKL_STAGGERED_COSINE_TRANSFORM 2

/* TT lower case */
void d_init_trig_transform(int *, int *, int *, double *, int *);
void d_commit_trig_transform(double *, DFTI_DESCRIPTOR_HANDLE *, int *, double *, int *); 
void d_forward_trig_transform(double *, DFTI_DESCRIPTOR_HANDLE *, int *, double *, int *);
void d_backward_trig_transform(double *, DFTI_DESCRIPTOR_HANDLE *, int *, double *, int *);
void s_init_trig_transform(int *, int *, int *, float *, int *);
void s_commit_trig_transform(float *, DFTI_DESCRIPTOR_HANDLE *, int *, float *, int *); 
void s_forward_trig_transform(float *, DFTI_DESCRIPTOR_HANDLE *, int *, float *, int *);
void s_backward_trig_transform(float *, DFTI_DESCRIPTOR_HANDLE *, int *, float *, int *);
void free_trig_transform(DFTI_DESCRIPTOR_HANDLE *, int *, int *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MKL_DFTI_H_ */

!*******************************************************************************
!                              INTEL CONFIDENTIAL
!   Copyright(C) 2004-2006 Intel Corporation. All Rights Reserved.
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
!   Content : MKL PARDISO Fortran-90 header file
!
!           Contains PARDISO routine definition.
!           For Windows use only. 
!           Can be used both in CDECL, CVF calling conventions.
!
!*******************************************************************************
!DEC$ IF .NOT. DEFINED( __MKL_PARDISO_F90 )

!DEC$ DEFINE __MKL_PARDISO_F90 

      MODULE MKL_PARDISO_PRIVATE
        TYPE MKL_PARDISO_HANDLE; INTEGER(KIND=8) DUMMY; END TYPE
      END MODULE MKL_PARDISO_PRIVATE

      MODULE MKL_PARDISO
        USE MKL_PARDISO_PRIVATE

!
! Subroutine prototype for PARDISO
!

      INTERFACE
        SUBROUTINE PARDISO( PT, MAXFCT, MNUM, MTYPE, PHASE, N, A, IA, JA, PERM, NRHS, IPARM, MSGLVL, B, X, ERROR )
          USE MKL_PARDISO_PRIVATE
!DEC$ ATTRIBUTES C, REFERENCE, ALIAS:'_PARDISO' :: PARDISO
          TYPE(MKL_PARDISO_HANDLE), INTENT(INOUT) :: PT(*)
          INTEGER(KIND=4),          INTENT(IN)    :: MAXFCT
          INTEGER(KIND=4),          INTENT(IN)    :: MNUM
          INTEGER(KIND=4),          INTENT(IN)    :: MTYPE
          INTEGER(KIND=4),          INTENT(IN)    :: PHASE
          INTEGER(KIND=4),          INTENT(IN)    :: N
          INTEGER(KIND=4),          INTENT(IN)    :: IA(*)
          INTEGER(KIND=4),          INTENT(IN)    :: JA(*)
          INTEGER(KIND=4),          INTENT(IN)    :: PERM(*)
          INTEGER(KIND=4),          INTENT(IN)    :: NRHS
          INTEGER(KIND=4),          INTENT(INOUT) :: IPARM(*)
          INTEGER(KIND=4),          INTENT(IN)    :: MSGLVL
          INTEGER(KIND=4),          INTENT(OUT)   :: ERROR
          REAL(KIND=8),             INTENT(IN)    :: A(*)
          REAL(KIND=8),             INTENT(INOUT) :: B(*)
          REAL(KIND=8),             INTENT(OUT)   :: X(*)
        END SUBROUTINE PARDISO
      END INTERFACE
      INTERFACE

      END MODULE MKL_PARDISO

!DEC$ ENDIF



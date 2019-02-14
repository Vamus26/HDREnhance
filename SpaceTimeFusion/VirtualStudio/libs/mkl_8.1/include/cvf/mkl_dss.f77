********************************************************************************
*                              INTEL CONFIDENTIAL
*   Copyright(C) 2004-2006 Intel Corporation. All Rights Reserved.
*   The source code contained  or  described herein and all documents related to
*   the source code ("Material") are owned by Intel Corporation or its suppliers
*   or licensors.  Title to the  Material remains with  Intel Corporation or its
*   suppliers and licensors. The Material contains trade secrets and proprietary
*   and  confidential  information of  Intel or its suppliers and licensors. The
*   Material  is  protected  by  worldwide  copyright  and trade secret laws and
*   treaty  provisions. No part of the Material may be used, copied, reproduced,
*   modified, published, uploaded, posted, transmitted, distributed or disclosed
*   in any way without Intel's prior express written permission.
*   No license  under any  patent, copyright, trade secret or other intellectual
*   property right is granted to or conferred upon you by disclosure or delivery
*   of the Materials,  either expressly, by implication, inducement, estoppel or
*   otherwise.  Any  license  under  such  intellectual property  rights must be
*   express and approved by Intel in writing.
*
********************************************************************************
*   Content : MKL DSS Fortran-77 header file
*
*           Contains main datatypes, routines and constants definition
*           For Windows use only. 
*           Can be used both in CDECL, CVF calling conventions.
*
********************************************************************************
      INTEGER MKL_DSS_DEFAULTS
      PARAMETER ( MKL_DSS_DEFAULTS = 0 )


!
! Message level option definitions
!

      INTEGER MKL_DSS_MSG_LVL_SUCCESS
      PARAMETER ( MKL_DSS_MSG_LVL_SUCCESS = -2147483647 )

      INTEGER MKL_DSS_MSG_LVL_DEBUG  
      PARAMETER ( MKL_DSS_MSG_LVL_DEBUG   = -2147483646 )

      INTEGER MKL_DSS_MSG_LVL_INFO   
      PARAMETER ( MKL_DSS_MSG_LVL_INFO    = -2147483645 )

      INTEGER MKL_DSS_MSG_LVL_WARNING
      PARAMETER ( MKL_DSS_MSG_LVL_WARNING = -2147483644 )

      INTEGER MKL_DSS_MSG_LVL_ERROR  
      PARAMETER ( MKL_DSS_MSG_LVL_ERROR   = -2147483643 )

      INTEGER MKL_DSS_MSG_LVL_FATAL  
      PARAMETER ( MKL_DSS_MSG_LVL_FATAL   = -2147483642 )


!
! Termination level option definitions
!

      INTEGER MKL_DSS_TERM_LVL_SUCCESS
      PARAMETER ( MKL_DSS_TERM_LVL_SUCCESS = 1073741832 )

      INTEGER MKL_DSS_TERM_LVL_DEBUG  
      PARAMETER ( MKL_DSS_TERM_LVL_DEBUG   = 1073741840 )

      INTEGER MKL_DSS_TERM_LVL_INFO   
      PARAMETER ( MKL_DSS_TERM_LVL_INFO    = 1073741848 )

      INTEGER MKL_DSS_TERM_LVL_WARNING
      PARAMETER ( MKL_DSS_TERM_LVL_WARNING = 1073741856 )

      INTEGER MKL_DSS_TERM_LVL_ERROR  
      PARAMETER ( MKL_DSS_TERM_LVL_ERROR   = 1073741864 )

      INTEGER MKL_DSS_TERM_LVL_FATAL  
      PARAMETER ( MKL_DSS_TERM_LVL_FATAL   = 1073741872 )


!
! Structure option definitions
!

      INTEGER MKL_DSS_SYMMETRIC          
      PARAMETER ( MKL_DSS_SYMMETRIC           = 536870976 )

      INTEGER MKL_DSS_SYMMETRIC_STRUCTURE
      PARAMETER ( MKL_DSS_SYMMETRIC_STRUCTURE = 536871040 )

      INTEGER MKL_DSS_NON_SYMMETRIC      
      PARAMETER ( MKL_DSS_NON_SYMMETRIC       = 536871104 )


!
! Reordering option definitions
!

      INTEGER MKL_DSS_AUTO_ORDER   
      PARAMETER ( MKL_DSS_AUTO_ORDER    = 268435520 )

      INTEGER MKL_DSS_MY_ORDER     
      PARAMETER ( MKL_DSS_MY_ORDER      = 268435584 )

      INTEGER MKL_DSS_OPTION1_ORDER
      PARAMETER ( MKL_DSS_OPTION1_ORDER = 268435648 )


!
! Factorization option definitions
!

      INTEGER MKL_DSS_POSITIVE_DEFINITE          
      PARAMETER ( MKL_DSS_POSITIVE_DEFINITE           = 134217792 )

      INTEGER MKL_DSS_INDEFINITE                 
      PARAMETER ( MKL_DSS_INDEFINITE                  = 134217856 )

      INTEGER MKL_DSS_HERMITIAN_POSITIVE_DEFINITE
      PARAMETER ( MKL_DSS_HERMITIAN_POSITIVE_DEFINITE = 134217920 )

      INTEGER MKL_DSS_HERMITIAN_INDEFINITE       
      PARAMETER ( MKL_DSS_HERMITIAN_INDEFINITE        = 134217984 )


!
! Return status values
!

      INTEGER MKL_DSS_SUCCESS        
      PARAMETER ( MKL_DSS_SUCCESS         = 0 )

      INTEGER MKL_DSS_ZERO_PIVOT     
      PARAMETER ( MKL_DSS_ZERO_PIVOT      = -1 )

      INTEGER MKL_DSS_OUT_OF_MEMORY  
      PARAMETER ( MKL_DSS_OUT_OF_MEMORY   = -2 )

      INTEGER MKL_DSS_FAILURE        
      PARAMETER ( MKL_DSS_FAILURE         = -3 )

      INTEGER MKL_DSS_ROW_ERR        
      PARAMETER ( MKL_DSS_ROW_ERR         = -4 )

      INTEGER MKL_DSS_COL_ERR        
      PARAMETER ( MKL_DSS_COL_ERR         = -5 )

      INTEGER MKL_DSS_TOO_FEW_VALUES 
      PARAMETER ( MKL_DSS_TOO_FEW_VALUES  = -6 )

      INTEGER MKL_DSS_TOO_MANY_VALUES
      PARAMETER ( MKL_DSS_TOO_MANY_VALUES = -7 )

      INTEGER MKL_DSS_NOT_SQUARE     
      PARAMETER ( MKL_DSS_NOT_SQUARE      = -8 )

      INTEGER MKL_DSS_STATE_ERR      
      PARAMETER ( MKL_DSS_STATE_ERR       = -9 )

      INTEGER MKL_DSS_INVALID_OPTION 
      PARAMETER ( MKL_DSS_INVALID_OPTION  = -10 )

      INTEGER MKL_DSS_OPTION_CONFLICT
      PARAMETER ( MKL_DSS_OPTION_CONFLICT = -11 )

      INTEGER MKL_DSS_MSG_LVL_ERR    
      PARAMETER ( MKL_DSS_MSG_LVL_ERR     = -12 )

      INTEGER MKL_DSS_TERM_LVL_ERR   
      PARAMETER ( MKL_DSS_TERM_LVL_ERR    = -13 )

      INTEGER MKL_DSS_STRUCTURE_ERR  
      PARAMETER ( MKL_DSS_STRUCTURE_ERR   = -14 )

      INTEGER MKL_DSS_REORDER_ERR    
      PARAMETER ( MKL_DSS_REORDER_ERR     = -15 )

      INTEGER MKL_DSS_VALUES_ERR     
      PARAMETER ( MKL_DSS_VALUES_ERR      = -16 )

      INTEGER MKL_DSS_STATISTICS_INVALID_MATRIX 
      PARAMETER ( MKL_DSS_STATISTICS_INVALID_MATRIX   = -17 )

      INTEGER MKL_DSS_STATISTICS_INVALID_STATE
      PARAMETER ( MKL_DSS_STATISTICS_INVALID_STATE    = -18 )

      INTEGER MKL_DSS_STATISTICS_INVALID_STRING
      PARAMETER ( MKL_DSS_STATISTICS_INVALID_STRING   = -19 )

!
! Function prototypes for DSS routines
!

!DEC$ ATTRIBUTES C, REFERENCE, ALIAS:'_DSS_CREATE' :: DSS_CREATE
      INTEGER dss_create
      EXTERNAL dss_create

!DEC$ ATTRIBUTES C, REFERENCE, ALIAS:'_DSS_DEFINE_STRUCTURE' :: 
!DEC$& DSS_DEFINE_STRUCTURE
      INTEGER dss_define_structure
      EXTERNAL dss_define_structure

!DEC$ ATTRIBUTES C, REFERENCE, ALIAS:'_DSS_REORDER' :: DSS_REORDER
      INTEGER dss_reorder
      EXTERNAL dss_reorder

!DEC$ ATTRIBUTES C, REFERENCE, ALIAS:'_DSS_FACTOR_REAL' ::
!DEC$& DSS_FACTOR_REAL
      INTEGER dss_factor_real
      EXTERNAL dss_factor_real

!DEC$ ATTRIBUTES C, REFERENCE, ALIAS:'_DSS_FACTOR_COMPLEX' ::
!DEC$& DSS_FACTOR_COMPLEX
      INTEGER dss_factor_complex
      EXTERNAL dss_factor_complex

!DEC$ ATTRIBUTES C, REFERENCE, ALIAS:'_DSS_SOLVE_REAL' :: DSS_SOLVE_REAL
      INTEGER dss_solve_real
      EXTERNAL dss_solve_real

!DEC$ ATTRIBUTES C, REFERENCE, ALIAS:'_DSS_SOLVE_COMPLEX' ::
!DEC$& DSS_SOLVE_COMPLEX
      INTEGER dss_solve_complex
      EXTERNAL dss_solve_complex

!DEC$ ATTRIBUTES C, REFERENCE, ALIAS:'_DSS_DELETE' :: DSS_DELETE
      INTEGER dss_delete
      EXTERNAL dss_delete

!DEC$ ATTRIBUTES C, REFERENCE, ALIAS:'_DSS_STATISTICS' :: DSS_STATISTICS
      INTEGER dss_statistics
      EXTERNAL dss_statistics

!DEC$ ATTRIBUTES C, REFERENCE, ALIAS:'_MKL_CVT_TO_NULL_TERMINATED_STR' 
!DEC$& :: MKL_CVT_TO_NULL_TERMINATED_STR
      EXTERNAL mkl_cvt_to_null_terminated_str

!
!   A module  that describes  interfaces  of  interval  arithmetic  procedures
!   implemented in C/C++ for plugging them into programs written in Fortran-95 
!
!
!   Mnemonics for the types of operations arguments: 
!
!   S  - single precision real 
!
!   D  - double precision real 
!
!   SI - single precision interval 
!
!   DI - double precision interval 
! 
!==============================================================================!


MODULE INTERVAL_ARITHMETIC 
    
    
    IMPLICIT NONE
          
    !--------------------------------------------------------------------------!
  
  
    TYPE S_INTERVAL 
        REAL(4) ::  INF, SUP 
    END TYPE S_INTERVAL 

    TYPE D_INTERVAL
        REAL(8) ::  INF, SUP
    END TYPE D_INTERVAL
     
        
    !--------------------------------------------------------------------------!
  
    
    INTERFACE ASSIGNMENT(=)

        SUBROUTINE SI_COPY(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(OUT) ::  X
            TYPE(S_INTERVAL), INTENT(IN)  ::  Y
        END SUBROUTINE SI_COPY 
  
        SUBROUTINE DI_COPY(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(OUT) ::  X
            TYPE(D_INTERVAL), INTENT(IN)  ::  Y
        END SUBROUTINE DI_COPY 
        
    END INTERFACE 
  
  
    !--------------------------------------------------------------------------!

    
    INTERFACE SINTERVAL 
                               
        FUNCTION I_SI_CONVERT(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            INTEGER, INTENT(IN) ::  X         
            TYPE(S_INTERVAL) ::  I_SI_CONVERT
        END FUNCTION I_SI_CONVERT
       
        FUNCTION I_I_SI_CONVERT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            INTEGER, INTENT(IN) ::  X, Y          
            TYPE(S_INTERVAL) ::  I_I_SI_CONVERT
        END FUNCTION I_I_SI_CONVERT
        
        FUNCTION SR_SI_CONVERT(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(4), INTENT(IN) ::  X         
            TYPE(S_INTERVAL) ::  SR_SI_CONVERT
        END FUNCTION SR_SI_CONVERT
   
        FUNCTION DR_SI_CONVERT(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(8), INTENT(IN) ::  X         
            TYPE(S_INTERVAL) ::  DR_SI_CONVERT
        END FUNCTION DR_SI_CONVERT
   
        FUNCTION SR_SR_SI_CONVERT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(4), INTENT(IN) ::  X, Y 
            TYPE(S_INTERVAL) ::  SR_SR_SI_CONVERT 
        END FUNCTION SR_SR_SI_CONVERT

        FUNCTION DR_SR_SI_CONVERT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(8), INTENT(IN) ::  X 
            REAL(4), INTENT(IN) ::  Y 
            TYPE(S_INTERVAL) ::  DR_SR_SI_CONVERT 
        END FUNCTION DR_SR_SI_CONVERT
   
        FUNCTION SR_DR_SI_CONVERT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(4), INTENT(IN) ::  X
            REAL(8), INTENT(IN) ::  Y 
            TYPE(S_INTERVAL) ::  SR_DR_SI_CONVERT 
        END FUNCTION SR_DR_SI_CONVERT
     
        FUNCTION DR_DR_SI_CONVERT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(8), INTENT(IN) ::  X, Y 
            TYPE(S_INTERVAL) ::  DR_DR_SI_CONVERT 
        END FUNCTION DR_DR_SI_CONVERT
   
        FUNCTION DI_SI_CONVERT(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            TYPE(S_INTERVAL) ::  DI_SI_CONVERT 
        END FUNCTION DI_SI_CONVERT  
              
    END INTERFACE    
    
  
    !--------------------------------------------------------------------------!

   
    INTERFACE DINTERVAL
              
        FUNCTION I_DI_CONVERT(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            INTEGER, INTENT(IN) ::  X         
            TYPE(D_INTERVAL) ::  I_DI_CONVERT
        END FUNCTION I_DI_CONVERT
       
        FUNCTION I_I_DI_CONVERT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            INTEGER, INTENT(IN) ::  X, Y          
            TYPE(D_INTERVAL) ::  I_I_DI_CONVERT
        END FUNCTION I_I_DI_CONVERT
       
        FUNCTION DR_DI_CONVERT(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(8), INTENT(IN) ::  X         
            TYPE(D_INTERVAL) ::  DR_DI_CONVERT
        END FUNCTION DR_DI_CONVERT

        FUNCTION SR_DI_CONVERT(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(4), INTENT(IN) ::  X         
            TYPE(D_INTERVAL) ::  SR_DI_CONVERT
        END FUNCTION SR_DI_CONVERT

        FUNCTION DR_DR_DI_CONVERT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(8), INTENT(IN) ::  X, Y 
            TYPE(D_INTERVAL) ::  DR_DR_DI_CONVERT 
        END FUNCTION DR_DR_DI_CONVERT

        FUNCTION SR_DR_DI_CONVERT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(4), INTENT(IN) ::  X
            REAL(8), INTENT(IN) ::  Y
            TYPE(D_INTERVAL) ::  SR_DR_DI_CONVERT 
        END FUNCTION SR_DR_DI_CONVERT

        FUNCTION DR_SR_DI_CONVERT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(8), INTENT(IN) ::  X
            REAL(4), INTENT(IN) ::  Y 
            TYPE(D_INTERVAL) ::  DR_SR_DI_CONVERT 
        END FUNCTION DR_SR_DI_CONVERT

        FUNCTION SR_SR_DI_CONVERT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(4), INTENT(IN) ::  X, Y 
            TYPE(D_INTERVAL) ::  SR_SR_DI_CONVERT 
        END FUNCTION SR_SR_DI_CONVERT

        FUNCTION SI_DI_CONVERT(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            TYPE(D_INTERVAL) ::  SI_DI_CONVERT 
        END FUNCTION SI_DI_CONVERT 
                              
    END INTERFACE
   
           
    !--------------------------------------------------------------------------!
     
    
    INTERFACE OPERATOR(+) 
                               
        FUNCTION SI_SI_ADDI(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y
            TYPE(S_INTERVAL) ::  SI_SI_ADDI
        END FUNCTION SI_SI_ADDI
    
        FUNCTION SR_SI_ADDI(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(4), INTENT(IN) ::  X   
            TYPE(S_INTERVAL), INTENT(IN) ::  Y
            TYPE(S_INTERVAL) ::  SR_SI_ADDI
        END FUNCTION SR_SI_ADDI
    
        FUNCTION SI_SR_ADDI(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4), INTENT(IN) ::  Y
            TYPE(S_INTERVAL) ::  SI_SR_ADDI
        END FUNCTION SI_SR_ADDI
    
        FUNCTION DI_DI_ADDI(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y
            TYPE(D_INTERVAL) ::  DI_DI_ADDI
        END FUNCTION DI_DI_ADDI
   
        FUNCTION DR_DI_ADDI(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(8), INTENT(IN) :: X 
            TYPE(D_INTERVAL), INTENT(IN) ::  Y
            TYPE(D_INTERVAL) ::  DR_DI_ADDI
        END FUNCTION DR_DI_ADDI

        FUNCTION DI_DR_ADDI(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8), INTENT(IN) ::  Y 
            TYPE(D_INTERVAL) ::  DI_DR_ADDI
        END FUNCTION DI_DR_ADDI
                             
    END INTERFACE         
  
   
    !--------------------------------------------------------------------------!
  
  
    INTERFACE OPERATOR(-) 
       
        FUNCTION SI_SI_SUBT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y
            TYPE(S_INTERVAL) ::  SI_SI_SUBT
        END FUNCTION SI_SI_SUBT

        FUNCTION SR_SI_SUBT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(4), INTENT(IN) ::  X 
            TYPE(S_INTERVAL), INTENT(IN) ::  Y
            TYPE(S_INTERVAL) ::  SR_SI_SUBT
        END FUNCTION SR_SI_SUBT
    
        FUNCTION SI_SR_SUBT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4), INTENT(IN) ::  Y
            TYPE(S_INTERVAL) ::  SI_SR_SUBT
        END FUNCTION SI_SR_SUBT
   
        FUNCTION DI_DI_SUBT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y
            TYPE(D_INTERVAL) ::  DI_DI_SUBT
        END FUNCTION DI_DI_SUBT
  
        FUNCTION DR_DI_SUBT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(8), INTENT(IN) ::  X 
            TYPE(D_INTERVAL), INTENT(IN) ::  Y
            TYPE(D_INTERVAL) ::  DR_DI_SUBT
        END FUNCTION DR_DI_SUBT
  
        FUNCTION DI_DR_SUBT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8), INTENT(IN) ::  Y
            TYPE(D_INTERVAL) ::  DI_DR_SUBT
        END FUNCTION DI_DR_SUBT
                               
    END INTERFACE         
  
      
    !--------------------------------------------------------------------------!
  
    
    INTERFACE OPERATOR(*) 
             
        FUNCTION SI_SI_MULT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y
            TYPE(S_INTERVAL) ::  SI_SI_MULT
        END FUNCTION SI_SI_MULT
  
        FUNCTION SR_SI_MULT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(4), INTENT(IN) ::  X 
            TYPE(S_INTERVAL), INTENT(IN) ::  Y
            TYPE(S_INTERVAL) ::  SR_SI_MULT
        END FUNCTION SR_SI_MULT
   
        FUNCTION SI_SR_MULT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4), INTENT(IN) ::  Y
            TYPE(S_INTERVAL) ::  SI_SR_MULT
        END FUNCTION SI_SR_MULT
 
        FUNCTION DI_DI_MULT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y
            TYPE(D_INTERVAL) ::  DI_DI_MULT
        END FUNCTION DI_DI_MULT

        FUNCTION DR_DI_MULT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(8), INTENT(IN) ::  X    
            TYPE(D_INTERVAL), INTENT(IN) ::  Y
            TYPE(D_INTERVAL) ::  DR_DI_MULT
        END FUNCTION DR_DI_MULT
  
        FUNCTION DI_DR_MULT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8), INTENT(IN) ::  Y
            TYPE(D_INTERVAL) ::  DI_DR_MULT
        END FUNCTION DI_DR_MULT
                            
    END INTERFACE         
   
       
    !--------------------------------------------------------------------------!

  
    INTERFACE OPERATOR(/) 
              
        FUNCTION SI_SI_DIVI(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y         
            TYPE(S_INTERVAL) ::  SI_SI_DIVI
        END FUNCTION SI_SI_DIVI

        FUNCTION SR_SI_DIVI(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(4), INTENT(IN) ::  X 
            TYPE(S_INTERVAL), INTENT(IN) ::  Y 
            TYPE(S_INTERVAL) ::  SR_SI_DIVI
        END FUNCTION SR_SI_DIVI

        FUNCTION SI_SR_DIVI(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4), INTENT(IN) :: Y         
            TYPE(S_INTERVAL) ::  SI_SR_DIVI
        END FUNCTION SI_SR_DIVI
  
        FUNCTION DI_DI_DIVI(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y
            TYPE(D_INTERVAL) ::  DI_DI_DIVI
        END FUNCTION DI_DI_DIVI
  
        FUNCTION DR_DI_DIVI(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(8), INTENT(IN) ::  X 
            TYPE(D_INTERVAL), INTENT(IN) ::  Y
            TYPE(D_INTERVAL) ::  DR_DI_DIVI
        END FUNCTION DR_DI_DIVI
  
        FUNCTION DI_DR_DIVI(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8), INTENT(IN) ::  Y
            TYPE(D_INTERVAL) ::  DI_DR_DIVI
        END FUNCTION DI_DR_DIVI
                           
    END INTERFACE         

     
    !--------------------------------------------------------------------------!


    INTERFACE INF
            
        FUNCTION S_INF(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4) ::  S_INF
        END FUNCTION S_INF

        FUNCTION D_INF(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8) ::  D_INF
        END FUNCTION D_INF
                   
    END INTERFACE 


    !--------------------------------------------------------------------------!

  
    INTERFACE SUP
            
        FUNCTION S_SUP(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4) ::  S_SUP
        END FUNCTION S_SUP

        FUNCTION D_SUP(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8) ::  D_SUP
        END FUNCTION D_SUP 
          
    END INTERFACE 
  

    !--------------------------------------------------------------------------!

  
    INTERFACE MID
            
        FUNCTION S_MID(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4) ::  S_MID
        END FUNCTION S_MID

        FUNCTION D_MID(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8) ::  D_MID
        END FUNCTION D_MID 
            
    END INTERFACE
  
 
    !--------------------------------------------------------------------------!

  
    INTERFACE RAD
        
        FUNCTION S_RAD(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4) ::  S_RAD
        END FUNCTION S_RAD

        FUNCTION D_RAD(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8) ::  D_RAD
        END FUNCTION D_RAD
     
    END INTERFACE 
  
 
    !--------------------------------------------------------------------------!

    INTERFACE WID
         
        FUNCTION S_WID(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4) ::  S_WID
        END FUNCTION S_WID

        FUNCTION D_WID(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8) ::  D_WID
        END FUNCTION D_WID
   
    END INTERFACE 

    !--------------------------------------------------------------------------!
       
  
    INTERFACE MIG
         
        FUNCTION S_MIG(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4) ::  S_MIG
        END FUNCTION S_MIG

        FUNCTION D_MIG(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8) ::  D_MIG
        END FUNCTION D_MIG
        
    END INTERFACE 

  
    !--------------------------------------------------------------------------!
       
  
    INTERFACE MAG
           
        FUNCTION S_MAG(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4) ::  S_MAG
        END FUNCTION S_MAG

        FUNCTION D_MAG(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8) ::  D_MAG
        END FUNCTION D_MAG
         
    END INTERFACE 

    
    !--------------------------------------------------------------------------!
  
  
    INTERFACE ABS
           
        FUNCTION SI_ABS(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            TYPE(S_INTERVAL) ::  SI_ABS
        END FUNCTION SI_ABS
  
        FUNCTION DI_ABS(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            TYPE(D_INTERVAL) ::  DI_ABS
        END FUNCTION DI_ABS
      
    END INTERFACE 
    
    !--------------------------------------------------------------------------!
  
  
    INTERFACE SQR
           
        FUNCTION SI_SQR(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            TYPE(S_INTERVAL) ::  SI_SQR
        END FUNCTION SI_SQR
  
        FUNCTION DI_SQR(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            TYPE(D_INTERVAL) ::  DI_SQR
        END FUNCTION DI_SQR
      
    END INTERFACE 
    
    
    !--------------------------------------------------------------------------!
  
  
    INTERFACE OPERATOR(.IX.)
         
        FUNCTION S_IX(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            TYPE(S_INTERVAL) ::  S_IX
        END FUNCTION S_IX

        FUNCTION D_IX(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y
            TYPE(D_INTERVAL) ::  D_IX
        END FUNCTION D_IX
    
    END INTERFACE


    !--------------------------------------------------------------------------!


    INTERFACE OPERATOR(.IH.)
      
        FUNCTION SI_SI_IH(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            TYPE(S_INTERVAL) ::  SI_SI_IH
        END FUNCTION SI_SI_IH
  
        FUNCTION SR_SI_IH(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(4), INTENT(IN) ::  X       
            TYPE(S_INTERVAL), INTENT(IN) ::  Y 
            TYPE(S_INTERVAL) ::  SR_SI_IH
        END FUNCTION SR_SI_IH
  
        FUNCTION SI_SR_IH(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            REAL(4), INTENT(IN) ::  Y 
            TYPE(S_INTERVAL) ::  SI_SR_IH
        END FUNCTION SI_SR_IH
  
        FUNCTION DI_DI_IH(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            TYPE(D_INTERVAL) ::  DI_DI_IH
        END FUNCTION DI_DI_IH 
  
        FUNCTION DR_DI_IH(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(8), INTENT(IN) ::  X 
            TYPE(D_INTERVAL), INTENT(IN) ::  Y 
            TYPE(D_INTERVAL) ::  DR_DI_IH
        END FUNCTION DR_DI_IH 
  
        FUNCTION DI_DR_IH(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            REAL(8), INTENT(IN) ::  Y 
            TYPE(D_INTERVAL) ::  DI_DR_IH
        END FUNCTION DI_DR_IH 
                                      
    END INTERFACE 


    !--------------------------------------------------------------------------!

    INTERFACE ISEMPTY
          
        FUNCTION S_ISEMPTY(X)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X
            LOGICAL ::  S_ISEMPTY
        END FUNCTION S_ISEMPTY

        FUNCTION D_ISEMPTY(X)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X
            LOGICAL ::  D_ISEMPTY
        END FUNCTION D_ISEMPTY
              
    END INTERFACE       
  
    !--------------------------------------------------------------------------!

    INTERFACE DIST
     
        FUNCTION S_DIST(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y
            REAL(4) ::  S_DIST
        END FUNCTION S_DIST

        FUNCTION D_DIST(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y
            REAL(8) ::  D_DIST
        END FUNCTION D_DIST 
               
    END INTERFACE 

  
    !--------------------------------------------------------------------------!

  
    INTERFACE OPERATOR (.SB.)
           
        FUNCTION S_SB(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_SB
        END FUNCTION S_SB
  
        FUNCTION D_SB(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_SB
        END FUNCTION D_SB
        
    END INTERFACE 

  
    !--------------------------------------------------------------------------!

  
    INTERFACE OPERATOR (.SP.)
         
        FUNCTION S_SP(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_SP
        END FUNCTION S_SP
  
        FUNCTION D_SP(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_SP
        END FUNCTION D_SP
      
    END INTERFACE 

    
    !--------------------------------------------------------------------------!
    
  
    INTERFACE OPERATOR (.PSB.)
          
        FUNCTION S_PSB(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_PSB
        END FUNCTION S_PSB
  
        FUNCTION D_PSB(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_PSB
        END FUNCTION D_PSB
             
    END INTERFACE 

  
    !--------------------------------------------------------------------------!

  
    INTERFACE OPERATOR (.PSP.)
        
        FUNCTION S_PSP(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_PSP
        END FUNCTION S_PSP
  
        FUNCTION D_PSP(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_PSP
        END FUNCTION D_PSP
               
    END INTERFACE 


    !--------------------------------------------------------------------------!
   
   
    INTERFACE OPERATOR (.IN.)
        
        FUNCTION S_IN(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            REAL(4), INTENT(IN) ::  X
            TYPE(S_INTERVAL), INTENT(IN) ::  Y 
            LOGICAL ::  S_IN 
        END FUNCTION S_IN 
        
        FUNCTION D_IN(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            REAL(8), INTENT(IN) ::  X
            TYPE(D_INTERVAL), INTENT(IN) ::  Y 
            LOGICAL ::  D_IN 
        END FUNCTION D_IN 
               
    END INTERFACE
    
        
    !--------------------------------------------------------------------------!

    
    INTERFACE OPERATOR (.EQ.) 
          
        FUNCTION S_EQ(X,Y) 
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_EQ 
        END FUNCTION S_EQ 
    
        FUNCTION D_EQ(X,Y) 
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_EQ 
        END FUNCTION D_EQ 
              
    END INTERFACE 
     
    
    !--------------------------------------------------------------------------!
   
    
    INTERFACE OPERATOR (.NE.)
          
        FUNCTION S_NE(X,Y) 
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_NE 
        END FUNCTION S_NE 
    
        FUNCTION D_NE(X,Y) 
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_NE 
        END FUNCTION D_NE 
             
    END INTERFACE

           
    !--------------------------------------------------------------------------!
         
        
    INTERFACE OPERATOR (.ITR.)
         
        FUNCTION S_ITR(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_ITR
        END FUNCTION S_ITR
  
        FUNCTION D_ITR(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_ITR
        END FUNCTION D_ITR
            
    END INTERFACE 

   
    !--------------------------------------------------------------------------!
  
  
    INTERFACE OPERATOR (.DJ.) 
         
        FUNCTION S_DJ(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_DJ
        END FUNCTION S_DJ
  
        FUNCTION D_DJ(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_DJ
        END FUNCTION D_DJ
            
    END INTERFACE 

  
    !--------------------------------------------------------------------------!
   
      
    INTERFACE OPERATOR (.LT.) 
        
        FUNCTION S_LT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_LT
        END FUNCTION S_LT
  
        FUNCTION D_LT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_LT
        END FUNCTION D_LT
              
    END INTERFACE 


    !--------------------------------------------------------------------------!

 
    INTERFACE OPERATOR (.LE.) 
           
        FUNCTION S_LE(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_LE
        END FUNCTION S_LE
  
        FUNCTION D_LE(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_LE
        END FUNCTION D_LE
      
    END INTERFACE 


    !--------------------------------------------------------------------------!

 
    INTERFACE OPERATOR (.GT.) 
            
        FUNCTION S_GT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_GT
        END FUNCTION S_GT
  
        FUNCTION D_GT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_GT
        END FUNCTION D_GT
   
    END INTERFACE 


    !--------------------------------------------------------------------------!

 
    INTERFACE OPERATOR (.GE.) 
          
        FUNCTION S_GE(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_GE
        END FUNCTION S_GE
  
        FUNCTION D_GE(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_GE
        END FUNCTION D_GE
            
    END INTERFACE 
   
   
    !--------------------------------------------------------------------------!
   
  
    INTERFACE OPERATOR (.WLT.) 
       
        FUNCTION S_WLT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_WLT
        END FUNCTION S_WLT
  
        FUNCTION D_WLT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_WLT
        END FUNCTION D_WLT
            
    END INTERFACE 


    !--------------------------------------------------------------------------!

 
    INTERFACE OPERATOR (.WLE.) 
    
        FUNCTION S_WLE(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_WLE
        END FUNCTION S_WLE
  
        FUNCTION D_WLE(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_WLE
        END FUNCTION D_WLE
                 
    END INTERFACE 


    !--------------------------------------------------------------------------!

 
    INTERFACE OPERATOR (.WGT.) 
       
        FUNCTION S_WGT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_WGT
        END FUNCTION S_WGT
  
        FUNCTION D_WGT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_WGT
        END FUNCTION D_WGT
               
    END INTERFACE 


    !--------------------------------------------------------------------------!

 
    INTERFACE OPERATOR (.WGE.) 
              
        FUNCTION S_WGE(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_WGE
        END FUNCTION S_WGE
  
        FUNCTION D_WGE(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_WGE
        END FUNCTION D_WGE
                
    END INTERFACE 


    !--------------------------------------------------------------------------!

 
    INTERFACE OPERATOR (.SLT.) 
              
        FUNCTION S_SLT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_SLT
        END FUNCTION S_SLT
  
        FUNCTION D_SLT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_SLT
        END FUNCTION D_SLT
                 
    END INTERFACE 


    !--------------------------------------------------------------------------!

 
    INTERFACE OPERATOR (.SLE.) 
              
        FUNCTION S_SLE(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_SLE
        END FUNCTION S_SLE
  
        FUNCTION D_SLE(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_SLE
        END FUNCTION D_SLE
                 
    END INTERFACE 


    !--------------------------------------------------------------------------!

 
    INTERFACE OPERATOR (.SGT.) 
                
        FUNCTION S_SGT(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_SGT
        END FUNCTION S_SGT
  
        FUNCTION D_SGT(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_SGT
        END FUNCTION D_SGT
                  
    END INTERFACE 


    !--------------------------------------------------------------------------!

 
    INTERFACE OPERATOR (.SGE.) 
               
        FUNCTION S_SGE(X,Y)
            TYPE S_INTERVAL 
                REAL(4) ::  INF, SUP 
            END TYPE S_INTERVAL 
            TYPE(S_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  S_SGE
        END FUNCTION S_SGE
  
        FUNCTION D_SGE(X,Y)
            TYPE D_INTERVAL 
                REAL(8) ::  INF, SUP 
            END TYPE D_INTERVAL 
            TYPE(D_INTERVAL), INTENT(IN) ::  X, Y 
            LOGICAL ::  D_SGE
        END FUNCTION D_SGE
              
    END INTERFACE 


    !--------------------------------------------------------------------------!
  
      
END MODULE INTERVAL_ARITHMETIC


!==============================================================================!


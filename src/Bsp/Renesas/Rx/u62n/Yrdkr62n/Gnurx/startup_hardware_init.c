/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 



///////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

extern void Bsp_hardwareSetup_(void);

#ifdef __cplusplus
};
#endif
///////////////////////////////////////////

/** This method is called as part of the C/C++ runtime initialization, i.e.
    it is called BEFORE main() is entered!  
 */
void Bsp_hardwareSetup_(void)
{
/*
  SCI.SMR.BYTE = 0;
  SCI.SMR.BIT.CA   = 1;
  SCI.SMR.BIT.CHR  = 1;
  SCI.SMR.BIT.OE   = 1;
  SCI.SMR.BIT.STOP = 1;
  SCI.SMR.BIT.MP   = 1;
  SCI.SMR.BIT.CKS  = 3;
  SCI.BRR = 0;
  SCI.SCR.BYTE = 0;
  SCI.SCR.BIT.TIE  = 1;
  SCI.SCR.BIT.RIE  = 1;
  SCI.SCR.BIT.TE   = 1;
  SCI.SCR.BIT.RE   = 1;
  SCI.SCR.BIT.MPIE = 1;
  SCI.SCR.BIT.TEIE = 1;
  SCI.SCR.BIT.CKE  = 3;
  SCI.TDR = 0;
  SCI.SSR.BYTE = 0;
  SCI.SSR.BIT.TDRE = 1;
  SCI.SSR.BIT.RDRF = 1;
  SCI.SSR.BIT.ORER = 1;
  SCI.SSR.BIT.FER  = 1;
  SCI.SSR.BIT.PER  = 1;
  SCI.SSR.BIT.TEND = 1;
  SCI.SSR.BIT.MPB  = 1;
  SCI.SSR.BIT.MPBT = 1;
  SCI.RDR = 0;

*/
}




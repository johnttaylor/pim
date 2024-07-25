#ifndef Bsp_UserSpace_Api_h_
#define Bsp_UserSpace_Api_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2022  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file 


    This BSP is a null/empty BSP to be used when compiling applications that
    are dependent on "Bsp/Api.h", by the compile target is a desktop OS running
    in user space.


    DO NOT include this file directly! Instead include the generic BSP
    interface - src/Bsp/Api.h - and then configure your project's 
    'colony_map.h' to include THIS file.

*----------------------------------------------------------------------------*/ 
 
#include <stdint.h>


// Start C++
#ifdef __cplusplus
extern "C" {    
#endif


/*---------------------------------------------------------------------------*/

/// Generic API - map to a single/global mutex to simulate EI/DI
void Bsp_Api_disableIrqs_MAP(void);

/// Generic API - map to a single/global mutex to simulate EI/DI
void Bsp_Api_enableIrqs_MAP(void);

/// Generic API - map to a single/global mutex to simulate EI/DI
void Bsp_Api_pushAndDisableIrqs_MAP(void);       
                     
/// Generic API - map to a single/global mutex to simulate EI/DI
void Bsp_Api_popIrqs_MAP(void);                    

/// Generic API -->use a empty function to simulate a nop
void Bsp_Api_nop_MAP(void);


/// Generic API -->do nothing
#define Bsp_Api_turnOn_debug1_MAP()    

/// Generic API -->do nothing
#define Bsp_Api_turnOff_debug1_MAP()   

/// Generic API -->do nothing
#define Bsp_Api_toggle_debug1_MAP()    


/// Generic API -->do nothing
#define Bsp_Api_turnOn_debug2_MAP()    

/// Generic API -->do nothing
#define Bsp_Api_turnOff_debug2_MAP()   

/// Generic API -->do nothing
#define Bsp_Api_toggle_debug2_MAP()    


// End C++
#ifdef __cplusplus
};
#endif


#endif

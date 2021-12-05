#ifndef Bsp_Renesas_62n_cmp_timers_h_
#define Bsp_Renesas_62n_cmp_timers_h_
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
/** @file 


    This file provides a basic interface for the RX62N's Compare/Match timers

    NOTE: This interface is NOT THREAD SAFE.  It it left to application
          to manage/handle/implement thread safety.


    DO NOT include this file directly! Instead include the generic BSP
    interface - //colony.core/src/Bsp/Api.h.

*----------------------------------------------------------------------------*/ 


#include <stdint.h>
#include "Bsp/Api.h"


// Start C++
#ifdef __cplusplus
extern "C" {
#endif


/*------------------ PUBLIC API -----------------------------------------*/

/** This funciton initializes a 16bit timer channel for a Compare-Match 
    interrupt.  In addition to calling this function, the application is 
    responsible for populating the corresponding IRQ/Vector for the CMT
    interrupt. The CMT vectors are stored in the Relocatable vector table.
    <pre>

    Interupt/Vector mappings:
    ------------------------
    CMT0  :=  CMTU0_CMI0, Irq#27 (0x70h)
    CMT1  :=  CMTU0_CMI1, Irq#28 (0x74h)
    CMT2  :=  CMTU1_CMI2, Irq#29 (0x78h)
    CMT3  :=  CMTU1_CMI3, Irq#30 (0x7Ch)

    </pre>
    This function can/should ONLY be called when the desired timer channel is
    in the 'stopped' state.  All timer channels after reset are considered to
    be in the 'stopped' state.


    @param channelNum       CMT channel number. 0=CMT0, 1=CMT1, 2=CMT2, 3=CMT3
    @param irqRateInHZ      Desired frequency of the CMT interrupt.  
    @param pclkDivider      PCLK divider/prescaler. 0= PCLK/8, 1=PCLK/32, 2=PCLK/128, 3=PCLK/512
    @param irqPriority      Interrupt Priority. 1=lowest, ... 15=highest

    @returns 1 if successful; else 0 is returned. Note: The error 
             checking is VERY LIMITED, i.e. it only checks validates the
             range of arguments (except for irqRateInHZ which is not checked).
             There is NO checking if the actual PCLK / divider / irqRate is
             a 'valid' or accurate (i.e. a integer value).
     
    Example:
    <pre>
        // Sets CMT0 to generate an IRQ (CMI0) at every millisecond (1KHz) with 
        // an interrupt priority of 14.  A PCLK divider of 8 is used.
        Bsp_initCompareMatchTimer( 0, 1000, 0, 14 );  

    </pre>
 */  
uint8_t Bsp_initCompareMatchTimer( uint8_t channelNum, uint32_t irqRateInHZ, uint8_t pclkDivider, uint8_t irqPriority );


/** This method starts the specified Compare Match Timer channel. 

    @param channelNum       CMT channel number. 0=CMT0, 1=CMT1, 2=CMT2, 3=CMT3

    @returns 1 if successful; else 0 is returned. Note: The error 
             checking is VERY LIMITED, i.e. it only checks validates the
             range of arguments 
 */
uint8_t Bsp_startCompareMatchTimer( uint8_t channelNum );


/** This method stops the specified Compare Match Timer channel. 

    @param channelNum       CMT channel number. 0=CMT0, 1=CMT1, 2=CMT2, 3=CMT3

    @returns 1 if successful; else 0 is returned. Note: The error 
             checking is VERY LIMITED, i.e. it only checks validates the
             range of arguments 
 */
uint8_t Bsp_stopCompareMatchTimer( uint8_t channelNum );






// END C++
#ifdef __cplusplus
};
#endif
#endif  // end header latch

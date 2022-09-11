#ifndef Bsp_YRDKR62N_Api_h_
#define Bsp_YRDKR62N_Api_h_
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


    This BSP is developed/build with a 'just-in-time' approach.  This means
    as functional is added to the BSP incrementally as there is 'client'
    need for.  This BSP does not claim to provide full/complete functional
    and/or APIs for everything the board supports.


    DO NOT include this file directly! Instead include the generic BSP
    interface - src/Bsp/Api.h - and then configure your project's 
    'colony_map.h' to include THIS file.

*----------------------------------------------------------------------------*/ 


#include <stdint.h>
#include "colony_config.h"
#include "Bsp/Renesas/Rx/u62n/iodefine.h"
#include "Bsp/Renesas/Rx/u62n/hal_mappings_.h"
#include "Bsp/Renesas/Rx/u62n/uarts.h"               
#include "Bsp/Renesas/Rx/u62n/cmp_timers.h"               
#include "Bsp/Renesas/Rx/u62n/Yrdkr62n/GraphicLcd.h"  // For the OKAYA 96x64 Monochromo Graphic display


// Start C++
#ifdef __cplusplus
extern "C" {
#endif




//////////////////////////////////////////////////////////
/*
** BOARD Specific APIs
*/

//////////////////////////
/// System Clocks.
///
/// NOTE: The clock settings are adjustable via the 'colony.config.h' header file
//////////////////////////


// CPU's main crystal frequency
#ifndef     BSP_SYSCLK_X1_XTAL_FREQUENCY
#define     BSP_SYSCLK_X1_XTAL_FREQUENCY        (12000000L)
#endif

/// Realtime clock crystal
#ifndef     BSP_SYSCLK_X2_XTAL_FREQUENCY
#define     BSP_SYSCLK_X2_XTAL_FREQUENCY        (32768)
#endif

/// Multipler for the internal clock
#ifndef     BSP_SYSCLK_ICLK_MUL
#define     BSP_SYSCLK_ICLK_MUL                 (8)              
#endif

/// Multipler for the peripheral clock
#ifndef     BSP_SYSCLK_PCLK_MUL
#define     BSP_SYSCLK_PCLK_MUL                 (4)
#endif

/// Multipler for the external bus clock
#ifndef     BSP_SYSCLK_BCLK_MUL
#define     BSP_SYSCLK_BCLK_MUL                 (4)
#endif

/// System clock frequency
#define     BSP_SYSCLK_ICLK_FREQUENCY           (BSP_SYSCLK_X1_XTAL_FREQUENCY * BSP_SYSCLK_ICLK_MUL)

/// Peripheral clock frequency
#define     BSP_SYSCLK_PCLK_FREQUENCY           (BSP_SYSCLK_X1_XTAL_FREQUENCY * BSP_SYSCLK_PCLK_MUL)

/// External bus clock frequency
#define     BSP_SYSCLK_BCLK_FREQUENCY           (BSP_SYSCLK_X1_XTAL_FREQUENCY * BSP_SYSCLK_BCLK_MUL)





//////////////////////////
/// LEDs
//////////////////////////

/** Turns on (1) or off (0) one of the 'circle' LEDs.  A index of 0 will turn on/off all
    of the 'circle' LEDs.  Index 1 maps to physical LED4, Index 12 maps to 
    physical LED15.
 */
void Bsp_setCircleLED( uint8_t ledIndex, uint8_t setToOn );


/** Toggles one of the 'circle' LEDs.  A index of 0 will toggle all
    of the 'circle' LEDs.  Index 1 maps to physical LED4, Index 12 maps 
    to physical LED15.
 */
void Bsp_toggleCircleLED( uint8_t ledIndex );



//////////////////////////
/// Push Button Switches
//////////////////////////

/** This method returns 1 if the specificed push button is
    'pressed'. Index of 1 maps to SW1, ... 3 maps to SW3.  An index
    of out range will always return 0.
 */
uint8_t Bsp_isButtonPressed( uint8_t buttonIndex );



//////////////////////////
/// Serial Ports
//////////////////////////

/** Identifies which SCI port is wired to the DB9 connector on the board
 */
#define BSP_DB9_CONSOLE                 BSP_ID_SERIAL_PORT2


/** Pin Configuration/Select for the DB9 Console
 */
#define BSP_DB9_CONSOLE_PIN_SELECT      BSP_SERIAL_PORT_PINSEL_B


/** Pin configuration method.
   
    NOTE: This is a COMPONENT Scoped METHOD that is called by the generic 'uarts' 
          initialization routine.  The app should NEVER call this method
          directly.


    @param sciPortID        UART/SCI Port ID (must be one of BSP_ID_SERIAL_PORTx)
    @param pinSelect        Pin select, i.e. Pin configuration 'A' vs 'B'.  Note: On SCI2 has the option of A/B on the 100LQP part

    @returns 1 if successful; else 0 is returned. Note: The error 
             checking is VERY LIMITED, i.e. it only checks if 'sciPortID' is
             a valid ID.

 */
uint8_t Bsp_uartInitPin_( uint8_t sciPortID, uint8_t pinSelect );



//////////////////////////////////////////////////////////
/// Generic APIs
//////////////////////////////////////////////////////////

/// Generic API
#define Bsp_Api_nop_MAP()                       __asm("nop")

/// Generic API
#define Bsp_Api_disableIrqs_MAP()               __builtin_rx_clrpsw('I')

/// Generic API
#define Bsp_Api_enableIrqs_MAP()                __builtin_rx_setpsw('I')

/// Generic API
#define Bsp_Api_pushAndDisableIrqs_MAP()        {__asm volatile ( "PUSHC PSW" );__builtin_rx_clrpsw('I')} 

/// Generic API
#define Bsp_Api_popIrqs_MAP                     __asm volatile ( "POPC PSW" )



/// Default Mapping for the "debug LED 1"
#ifndef OPTION_BSP_DEBUG_LED1_INDEX
#define OPTION_BSP_DEBUG_LED1_INDEX    4
#endif

/// Default Mapping for the "debug LED 2"
#ifndef OPTION_BSP_DEBUG_LED2_INDEX
#define OPTION_BSP_DEBUG_LED2_INDEX    5
#endif


/// Generic API
#define Bsp_Api_turnOn_debug1_MAP()             Bsp_setCircleLED( OPTION_BSP_DEBUG_LED1_INDEX, 1 )

/// Generic API
#define Bsp_Api_turnOff_debug1_MAP()            Bsp_setCircleLED( OPTION_BSP_DEBUG_LED1_INDEX, 0 )

/// Generic API
#define Bsp_Api_toggle_debug1_MAP()             Bsp_toggleCircleLED( OPTION_BSP_DEBUG_LED1_INDEX )


/// Generic API
#define Bsp_Api_turnOn_debug2_MAP()             Bsp_setCircleLED( OPTION_BSP_DEBUG_LED2_INDEX, 1 )

/// Generic API
#define Bsp_Api_turnOff_debug2_MAP()            Bsp_setCircleLED( OPTION_BSP_DEBUG_LED2_INDEX, 0 )

/// Generic API
#define Bsp_Api_toggle_debug2_MAP()             Bsp_toggleCircleLED( OPTION_BSP_DEBUG_LED2_INDEX )




// END C++
#ifdef __cplusplus
};
#endif
#endif  // end header latch

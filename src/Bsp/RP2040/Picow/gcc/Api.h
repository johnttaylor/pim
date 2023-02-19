#ifndef Bsp_RP2040_PicoW_Api_h_
#define Bsp_RP2040_PicoW_Api_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
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

    OPTIONAL Compile switches:

        USE_BSP_NO_STDIO    - Defining this symbol will prevent initializing 
                              SDK's stdio sub-system.  This is for when the
                              application is supplying its own driver for
                              UART0
*----------------------------------------------------------------------------*/


#include "colony_config.h"
#include "pico/sync.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"



//////////////////////////////////////////////////////////
/// Mappings for Generic APIs
//////////////////////////////////////////////////////////

/// Generic API
#define Bsp_Api_nop_MAP()                       __asm("nop")                

/// Generic API
#define Bsp_Api_disableIrqs_MAP                 Bsp_saveAndDisableIrqs  // FIXME: Does not 100% meet the semantics of the method definition

/// Generic API
#define Bsp_Api_enableIrqs_MAP                  Bsp_restoredIrqs        // FIXME: Does not 100% meet the semantics of the method definition

/// Generic API
#define Bsp_Api_pushAndDisableIrqs_MAP          Bsp_saveAndDisableIrqs

/// Generic API
#define Bsp_Api_popIrqs_MAP()                   Bsp_restoredIrqs





/// Generic API
#define Bsp_Api_turnOn_debug1_MAP               Bsp_Api_turnOnDebug1

/// Generic API
#define Bsp_Api_turnOff_debug1_MAP              Bsp_Api_turnOffDebug1

/// Generic API
#define Bsp_Api_toggle_debug1_MAP               Bsp_Api_toggleDebug1



/// Generic API
#define Bsp_Api_turnOn_debug2_MAP()             // Not supported. There is one LED on the PICO board

/// Generic API
#define Bsp_Api_turnOff_debug2_MAP()            // Not supported. There is one LED on the PICO board

/// Generic API
#define Bsp_Api_toggle_debug2_MAP()             // Not supported. There is one LED on the PICO board


/// Function to operate on Debug LED1
extern void Bsp_Api_turnOnDebug1();

/// Function to operate on Debug LED1
extern void Bsp_Api_turnOffDebug1();

/// Function to operate on Debug LED1
extern void Bsp_Api_toggleDebug1();

//////////////////////////////////////////////////////////
/// Target/Board specific Functions 
//////////////////////////////////////////////////////////

/// Default UART Settings
#define BSP_DEFAULT_UART_HANDLE     uart0

/// Default UART Settings
#define BSP_DEFAULT_UART_TX_PIN     0

/// Default UART Settings
#define BSP_DEFAULT_UART_RX_PIN     1

/// Depth of the UART's HW RX FIFO
#define BSP_UART_RX_FIFO_DEPTH      32

/// Depth of the UART's HW TX FIFO
#define BSP_UART_TX_FIFO_DEPTH      32

/// Save the current IRQ-enabled-state and disables interrupts. Only effects the core where the method is called
void Bsp_saveAndDisableIrqs();

/// Restore the previous saved IRQ-enabled-state. Only effects the core where the method is called
void Bsp_restoredIrqs();


#endif  // end header latch

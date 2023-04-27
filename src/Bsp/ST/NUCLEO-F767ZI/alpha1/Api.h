#ifndef Bsp_Stm32_alpha1_Api_h_
#define Bsp_Stm32_alpha1_Api_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2017  John T. Taylor
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


#include "colony_config.h"
#include "Bsp/ST/NUCLEO-F767ZI/alpha1/MX/Core/Inc/main.h"   // Access the PINs
#include "Bsp/ST/NUCLEO-F767ZI/alpha1/MX/Core/Inc/usart.h"  // Access the UART handles/instances


#ifdef ENABLE_BSP_SEGGER_SYSVIEW   
#include "SEGGER_SYSVIEW.h" // Expose (to the application) the SYSVIEW APIs when enabled
#endif

//////////////////////////////////////////////////////////
/// ARM Specific APIs
//////////////////////////////////////////////////////////

/// Disable a specific interrupt (with memory barrier protection)
#define Bsp_NVIC_disableIRQ( irqNum )   do { HAL_NVIC_DisableIRQ(irqNum); __DSB(); __ISB(); } while(0)

/// Enable a specific interrupt (with memory barrier protection)
#define Bsp_NVIC_enableIRQ( irqNum )    do { HAL_NVIC_EnableIRQ(irqNum); __DSB(); __ISB(); } while(0)



//////////////////////////////////////////////////////////
/// Generic APIs
//////////////////////////////////////////////////////////

/// Generic API
#define Bsp_Api_nop_MAP()                       __asm("nop")               

/// Generic API
#define Bsp_Api_disableIrqs_MAP()               __disable_irq()

/// Generic API (with memory barrier protection)
#define Bsp_Api_enableIrqs_MAP()                do { __enable_irq(); __ISB(); } while(0)

/// Generic API
#define Bsp_Api_pushAndDisableIrqs_MAP()        Bsp_Api_disableIrqs_MAP()    // FIXME: This really needs to PUSH the IRQ state!!!

/// Generic API
#define Bsp_Api_popIrqs_MAP()                   Bsp_Api_enableIrqs_MAP()     // FIXME: This really needs to POP the IRQ state!!!!



/// Generic API
#define Bsp_Api_turnOn_debug1_MAP()             HAL_GPIO_WritePin( LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET )

/// Generic API
#define Bsp_Api_turnOff_debug1_MAP()            HAL_GPIO_WritePin( LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET )

/// Generic API
#define Bsp_Api_toggle_debug1_MAP()             HAL_GPIO_TogglePin( LD1_GPIO_Port, LD1_Pin )


/// Generic API
#define Bsp_Api_turnOn_debug2_MAP()             HAL_GPIO_WritePin( LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET )

/// Generic API
#define Bsp_Api_turnOff_debug2_MAP()            HAL_GPIO_WritePin( LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET )

/// Generic API
#define Bsp_Api_toggle_debug2_MAP()             HAL_GPIO_TogglePin( LD2_GPIO_Port, LD2_Pin )


//////////////////////////////////////////////////////////
/// FreeRTOS specific APIs
//////////////////////////////////////////////////////////

/** This method informs the schedule that a context switch is required on exit 
    from the ISR.  The 'r' argument is the result for the su_signal() call.
 */
#define Bsp_yieldOnExit(r)                      portYIELD_FROM_ISR(r)



#endif  // end header latch

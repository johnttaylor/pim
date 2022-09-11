#ifndef Cpl_Io_Serial_Renesas_Rx62n_Hal_h_
#define Cpl_Io_Serial_Renesas_Rx62n_Hal_h_
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

	This file defines a hardware abstraction layer (HAL) for initializing
	the UARTs (SCIn) peripherals on the Renesas RX62n microcontroller.
*/

#include "Driver/Uart/Hal.h"


/*-------------- PUBLIC API ------------------------------------------------*/
/** This method provides an initialization routine for the micro-controller's
	UARTs.

	@param sciPortID        UART/SCI Port ID (must be one of BSP_ID_SERIAL_PORTx)
	@param pinSelect        Pin select, i.e. Pin configuration 'A' vs 'B'.  Not all pin configuration options are available on all packages of the microcontroller
	@param baudrate         BRR value
	@param baudrateDivider  PCLK divider/prescaler. 0= PCLK, 1=PCLK/4, 2=PCLK/16, 3=PCLK/11
	@param irqPriority      Interrupt Priority. 1=lowest, ... 15=highest
	@param frameConfig      is set by bit-wise ORing the Parity, Stopbits, and data length values together

	@returns 1 if successful; else 0 is returned. Note: The error
			 checking is VERY LIMITED, i.e. it only checks if 'sciPortID' is
			 a valid ID.

	\b Prototype:
		uint8_t Cpl_Io_Serial_Renesas_Rx62n_Hal_uartInit( Cpl_Driver_Uart_Hal_T sciPortID,
														  uint8_t               pinSelect,
														  uint8_t               irqPriority,
														  uint8_t               baudrate,
														  uint8_t               baudrateDivider,
														  uint8_t               frameConfig
														);
 */
#define Cpl_Io_Serial_Renesas_Rx62n_Hal_uartInit                Cpl_Io_Serial_Renesas_Rx62n_Hal_uartInit_MAP




 /*--------------------------------------------------------------------------*/
#endif  

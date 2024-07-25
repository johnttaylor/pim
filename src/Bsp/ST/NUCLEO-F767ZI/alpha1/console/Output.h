#ifndef Bsp_Stm32_Console_h_
#define Bsp_Stm32_Console_h_
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
/** @file */

#include "colony_config.h"
#include "Cpl/Io/Serial/ST/M32F4/InputOutput.h"

/** Default size of the software TX FIFO used by the console UART Stream
 */
#ifndef OPTION_BSP_CONSOLE_TX_FIFO_SIZE
#define OPTION_BSP_CONSOLE_TX_FIFO_SIZE     1024
#endif

 /** Default size of the software RX FIFO used by the console UART Stream
  */
#ifndef OPTION_BSP_CONSOLE_RX_FIFO_SIZE
#define OPTION_BSP_CONSOLE_RX_FIFO_SIZE     1024
#endif



/// Expose the Console stream
extern Cpl::Io::Serial::ST::M32F4::InputOutput   g_bspConsoleStream;


#endif  // end header latch

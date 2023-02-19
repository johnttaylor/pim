#ifndef Cpl_System_RP2040_Console_h_
#define Cpl_System_RP2040_Console_h_
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

    This interface provides a 'Console' replacement for SDK's stdio.  It
    uses a interrupt based (with software FIFOs) UART driver for input/output.

 */


#include "colony_config.h"
#include "Cpl/Io/InputOutput.h"
#include "Bsp/Api.h"


 /// Size, in bytes, of the Software TX FIFO for the Console's output 
#ifndef OPTION_CPL_SYSTEM_RP2040_CONSOLE_TX_FIFO_SIZE    
#define OPTION_CPL_SYSTEM_RP2040_CONSOLE_TX_FIFO_SIZE    256
#endif

/// Size, in bytes, of the Software RX FIFO for the Console's input 
#ifndef OPTION_CPL_SYSTEM_RP2040_CONSOLE_RX_FIFO_SIZE    
#define OPTION_CPL_SYSTEM_RP2040_CONSOLE_RX_FIFO_SIZE    512
#endif


///
namespace Cpl {
///
namespace System {
///
namespace RP2040 {


/** This method is used to initialize and start the Console.
 */
void startConsole( unsigned long baudRate  = 115200,                       //!< Baud rate in hertz
                   unsigned      txPin     = BSP_DEFAULT_UART_TX_PIN,      //!< Transmit Pin
                   unsigned      rxPin     = BSP_DEFAULT_UART_RX_PIN,      //!< Receive Pin
                   unsigned      dataBits  = 8,                            //!< Number of data bits.  Range is [5..8]
                   unsigned      stopBits  = 1,                            //!< Number of stop bits.  Range is [1..2]
                   uart_parity_t parity    = UART_PARITY_NONE              //!< Parity setting.  See hardware/uart.h for enumeration
);

/** This method returns a handle to the Console's CPL Stream instance
 */
Cpl::Io::InputOutput& getConsoleStream();

}       // end namespace
}
}
#endif  // end header latch
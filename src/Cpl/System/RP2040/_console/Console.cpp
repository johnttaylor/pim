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

#include "Cpl/System/RP2040/Console.h"
#include "Cpl/Io/Serial/RP2040/Uart/InputOutput.h"


static uint8_t txFIFO_[OPTION_CPL_SYSTEM_RP2040_CONSOLE_TX_FIFO_SIZE];
static uint8_t rxFIFO_[OPTION_CPL_SYSTEM_RP2040_CONSOLE_RX_FIFO_SIZE];

static Cpl::Io::Serial::RP2040::Uart::InputOutput uartfd_( txFIFO_, sizeof( txFIFO_ ), rxFIFO_, sizeof( rxFIFO_ ) );


void Cpl::System::RP2040::startConsole( unsigned long baudRate,
                                        unsigned      txPin,
                                        unsigned      rxPin,
                                        unsigned      dataBits,
                                        unsigned      stopBits,
                                        uart_parity_t parity )
{
    uartfd_.start( baudRate, txPin, rxPin, dataBits, stopBits, parity );
}

Cpl::Io::InputOutput& Cpl::System::RP2040::getConsoleStream()
{
    return uartfd_;
}

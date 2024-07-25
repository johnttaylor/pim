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

#include "InputOutput.h"

///
using namespace Cpl::Io::Serial::ST::M32F4;



////////////////////////////////////
InputOutput::InputOutput( Cpl::Container::RingBuffer<uint8_t>& txBuffer,
                          Cpl::Container::RingBuffer<uint8_t>& rxBuffer
)
    :m_driver( txBuffer, rxBuffer )
{
}


InputOutput::~InputOutput( void )
{
    close();
}


void InputOutput::start( IRQn_Type           uartIrqNum,
                         UART_HandleTypeDef* uartHdlToUse ) noexcept
{
    m_driver.start( uartIrqNum, uartHdlToUse );
}


////////////////////////////////////
bool InputOutput::read( void* buffer, int numBytes, int& bytesRead )
{
    return m_driver.read( buffer, numBytes, bytesRead );
}

bool InputOutput::available()
{
    return m_driver.available();
}

size_t InputOutput::getRxErrorsCounts( bool clearCount ) noexcept
{
    return m_driver.getRXErrorsCounts( clearCount );
}


////////////////////////////////////
bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    bytesWritten = maxBytes;
    return m_driver.write( buffer, (size_t) maxBytes );
}


void InputOutput::flush()
{
    // Not supported/has no meaning for a serial port
}

bool InputOutput::isEos()
{
    // Does not really have meaning for serial port (assuming the serial port is opened/active)
    return false;
}

void InputOutput::close()
{
    m_driver.stop();
}

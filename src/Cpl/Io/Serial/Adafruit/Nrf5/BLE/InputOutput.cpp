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

#include "InputOutput.h"
#include <stdint.h>

///
using namespace Cpl::Io::Serial::Adafruit::Nrf5::BLE;



////////////////////////////////////
InputOutput::InputOutput( BLEUart& serialPort )
    : m_commPort( serialPort )
    , m_started( false )
{
}


InputOutput::~InputOutput( void )
{
    close();
}


void InputOutput::start( void )noexcept
{
    if ( !m_started )
    {
        m_commPort.begin();
        m_started = true;
    }
}


////////////////////////////////////
bool InputOutput::read( void* buffer, int numBytes, int& bytesRead )
{
    // Fail if I have not been started
    if ( !m_started )
    {
        return false;
    }

    uint8_t* ptr = (uint8_t*) buffer;
    for ( bytesRead = 0; bytesRead < numBytes; bytesRead++ )
    {
        int inbyte = m_commPort.read();
        if ( inbyte < 0 )
        {
            // No more data left in the RX queue -->return
            return true;
        }

        // Store the read-in byte
        *ptr++ = (uint8_t) inbyte;
    }

    // The Adafruit Uart class NEVER has an 'error' on the read() call
    return true;
}

bool InputOutput::available()
{
    // Fail if I have not been started
    if ( !m_started )
    {
        return false;
    }

    return m_commPort.available();
}


////////////////////////////////////
bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    // Fail if I have not been started
    if ( !m_started )
    {
        return false;
    }

    const uint8_t* ptr = (const uint8_t*) buffer;
    bytesWritten = maxBytes;

    for ( bytesWritten=0; bytesWritten < maxBytes; bytesWritten++ )
    {
        if ( m_commPort.write( *ptr++ ) != 1 )
        {
            return false;
        }
    }

    return true;
}


void InputOutput::flush()
{
    // Do nothing if I have not been started
    if ( m_started )
    {
        m_commPort.flush();
    }
}


void InputOutput::close()
{
    // Do nothing if I have not been started
    if ( m_started )
    {
        // The BLE Uart class has no stop/end method!  And this class does not have access to 'enable/disable BLE Services' methods/objects.
        m_started = false;
    }
}

bool InputOutput::isEos()
{
    return !m_started;
}

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
/** @file */

#include "Master.h"
#include "Cpl/System/Assert.h"
#include "pico/time.h"

#include "Cpl/System/Trace.h"
#define SECT_ "_0testxx"

using namespace Driver::I2C::RP2040;

//////////////////////////////////////////////////////////////////////////////
Master::Master( i2c_inst_t* i2cInstance,
                size_t      baudrate,
                size_t      timeoutMs )
    : m_i2cDevice( i2cInstance )
    , m_baudrate( baudrate )
    , m_timeoutMs( timeoutMs )
    , m_started( false )
{
    CPL_SYSTEM_ASSERT( i2cInstance == i2c0 || i2cInstance == i2c1 );
}

//////////////////////////////////////////////////////////////////////////////
bool Master::start() noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    if ( !m_started )
    {
        m_started = true;
        setBaudRate( m_baudrate );
        return true;
    }

    return false;
}

void Master::stop() noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
 
    if ( m_started )
    {
        m_started = false;
        i2c_deinit( m_i2cDevice );
    }
}


Driver::I2C::Master::Result_T Master::writeToDevice( uint8_t     device7BitAddress,
                                                     size_t      numBytesToTransmit,
                                                     const void* srcData,
                                                     bool        noStop ) noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    // Fail if not started
    if ( !m_started )
    {
        return eNOT_STARTED;
    }

    int ret = i2c_write_blocking_until( m_i2cDevice,
                                        device7BitAddress,
                                        (const uint8_t*) srcData,
                                        numBytesToTransmit,
                                        noStop,
                                        make_timeout_time_ms( (uint32_t) m_timeoutMs ) );
    return ret == (int) numBytesToTransmit ? eSUCCESS : ret == PICO_ERROR_GENERIC ? eNO_ACK : eTIMEOUT;
}

Driver::I2C::Master::Result_T Master::readFromDevice( uint8_t   device7BitAddress,
                                                      size_t    numBytesToRead,
                                                      void*     dstData,
                                                      bool      noStop )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    // Fail if not started
    if ( !m_started )
    {
        return eNOT_STARTED;
    }

    int ret = i2c_read_blocking_until( m_i2cDevice,
                                       device7BitAddress,
                                       (uint8_t*) dstData,
                                       numBytesToRead,
                                       noStop,
                                       make_timeout_time_ms( (uint32_t) m_timeoutMs ) );

    return ret == (int) numBytesToRead ? eSUCCESS : ret == PICO_ERROR_GENERIC ? eNO_ACK : eTIMEOUT;
}


size_t Master::setBaudRate( size_t newBaudRateHz ) noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    size_t prevBaud = m_baudrate;
    m_baudrate      = newBaudRateHz;
    i2c_init( m_i2cDevice, m_baudrate );
    return prevBaud;
}

size_t Master::setTransactionTimeout( size_t maxTimeMs ) noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );

    size_t prevTimeout = m_timeoutMs;
    m_timeoutMs        = maxTimeMs;
    return prevTimeout;
}
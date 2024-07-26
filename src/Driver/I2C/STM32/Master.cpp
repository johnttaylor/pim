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
#include <stdint.h>

using namespace Driver::I2C::STM32;

//////////////////////////////////////////////////////////////////////////////
Master::Master( I2C_HandleTypeDef* i2cInstance,
                uint32_t           timeoutMs )
    : m_i2cDevice( i2cInstance )
    , m_timeout( timeoutMs )
{
}

//////////////////////////////////////////////////////////////////////////////
bool Master::start() noexcept
{
    if ( !m_started )
    {
        m_started = true;

        // TODO: Support setting/changing the baud rate

        return true;
    }

    return false;
}

void Master::stop() noexcept
{
    if ( m_started )
    {
        m_started = false;

        /* TODO: Support freeing up the GPIO pins.  This require being able to
                 config the I2C peripheral/pins independently of the ST MX tools
        */
    }
}

/////////////////////////////////////////////////////
static Master::Result_T convertHALErrorCode( HAL_StatusTypeDef r )
{
    switch ( r )
    {
    case HAL_OK:
        return Master::Result_T::eSUCCESS;
    case HAL_BUSY:
        return Master::Result_T::eTIMEOUT;
    default:
        break;
    }

    return Master::Result_T::eERROR;
}

Master::Result_T  Master::writeToDevice( uint8_t        device7BitAddress,
                                         size_t         numBytesToTransmit,
                                         const void*    srcData,
                                         bool           noStop ) noexcept
{
    // TODO: Add support for the 'noStop' semantics!
    
    if ( m_started )
    {
        // Note: Left shift the 7bit address so it is 'properly' align for the I2C peripheral 
        HAL_StatusTypeDef r = HAL_I2C_Master_Transmit( m_i2cDevice, device7BitAddress<<1, (uint8_t*) srcData, (uint16_t) numBytesToTransmit, m_timeout );
        return convertHALErrorCode( r );
    }
    
    return Master::Result_T::eNOT_STARTED;
}

Master::Result_T Master::readFromDevice( uint8_t   device7BitAddress,
                                         size_t    numBytesToRead,
                                         void*     dstData,
                                         bool      noStop )
{
    // TODO: Add support for the 'noStop' semantics!

    if ( m_started )
    {
        // Note: Left shift the 7bit address so it is 'properly' align for the I2C peripheral 
        HAL_StatusTypeDef r = HAL_I2C_Master_Receive( m_i2cDevice, device7BitAddress<<1, (uint8_t*) dstData, (uint16_t) numBytesToRead, m_timeout );
        return convertHALErrorCode( r );
    }

    return Master::Result_T::eNOT_STARTED;
}

size_t Master::setBaudRate( size_t newBaudRateHz ) noexcept
{
    // TODO: Actually implementation
    return newBaudRateHz;
}

/// See Driver::I2C::Master
size_t Master::setTransactionTimeout( size_t maxTimeMs ) noexcept
{
    size_t oldTimeout = m_timeout;
    m_timeout = maxTimeMs;
    return oldTimeout;
}


#ifndef Driver_I2C_STM32_Master_h_
#define Driver_I2C_STM32_Master_h_
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


#include "Driver/I2C/Master.h"
#include "Bsp/Api.h"    // Pull's in the ST HAL APIs

///
namespace Driver {
///
namespace I2C {
///
namespace STM32 {


/** This class implements the I2C interface for the STM32 family of
    micro-controller using the ST's MX Cube/IDE to configure the SPI peripherals
    and IO pins

    NOTE: Currently (8/2023) the 'noStop' semantics of the write/read methods
          is NOT supported.

    NOTE: The driver is not thread safe.  The application must ensure that
          only one thread is accessing the driver at a time.

    TODO: The driver intermittently fails when the application is writing large 
          amounts of data (>16K ish) and a thread switch occurs (and no it is
          not because other thread attempts to access the driver).  The 
          recommendation is to re-factor the driver to be interrupt based 
          instead of using the STM32 HAL 'polling' model.
 */
class Master : public Driver::I2C::Master
{
public:
    /** Constructor.

        The 'i2cInstance' MUST have already been initialize, i.e. the low
        level MX_I2Cx_Init() from the ST HAL APIs has been called

        TODO: Add support for configuring the I2C interface WITHOUT using
              ST's MX tools.
     */
    Master( I2C_HandleTypeDef* i2cInstance,
            uint32_t           timeoutMs = 50 );    // Default timeout is 50ms


public:
    /// See Driver::I2C::Master
    bool start() noexcept;

    /// See Driver::I2C::Master
    void stop() noexcept;

    /// See Driver::I2C::Master
    Result_T  writeToDevice( uint8_t        device7BitAddress,
                             size_t         numBytesToTransmit,
                             const void*    srcData,
                             bool           noStop = false ) noexcept;

    /// See Driver::I2C::Master
    Result_T readFromDevice( uint8_t   device7BitAddress,
                             size_t    numBytesToRead,
                             void*     dstData,
                             bool      noStop = false );


    /// See Driver::I2C::Master
    size_t setBaudRate( size_t newBaudRateHz ) noexcept;
    
    /// See Driver::I2C::Master
    size_t setTransactionTimeout( size_t maxTimeMs ) noexcept;

protected:

    /// Handle the low-level ST HAL driver instance
    I2C_HandleTypeDef*  m_i2cDevice;

    /// Timeout period for a SPI transaction
    uintptr_t           m_timeout;

    /// Track my started state
    bool                m_started;
};




};      // end namespaces
};
};
#endif  // end header latch

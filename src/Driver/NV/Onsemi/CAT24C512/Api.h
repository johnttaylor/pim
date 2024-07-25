#ifndef Driver_NV_Onsemi_Cat24C512_Api_h_
#define Driver_NV_Onsemi_Cat24C512_Api_h_
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


#include "colony_config.h"
#include "Driver/NV/Api.h"
#include "Driver/I2C/Master.h"


/// Number of Pages
#ifndef OPTION_DRIVER_NV_ONSEMI_CAT24C512_NUM_PAGES
#define OPTION_DRIVER_NV_ONSEMI_CAT24C512_NUM_PAGES         512
#endif

/// Number of bytes per page
#ifndef OPTION_DRIVER_NV_ONSEMI_CAT24C512_BYTES_PER_PAGE
#define OPTION_DRIVER_NV_ONSEMI_CAT24C512_BYTES_PER_PAGE    128
#endif

/** The amount of time, in milliseconds, to wait between queries
    to determine if the current write cycles has completed
 */
#ifndef OPTION_DRIVER_NV_ONSEMI_CAT24C512_WAIT_WRITE_COMPLETE_MS
#define OPTION_DRIVER_NV_ONSEMI_CAT24C512_WAIT_WRITE_COMPLETE_MS    1 // 1ms
#endif

/** Number of retry-attempts (while waiting on the write cycle to
    complete) before failing the write/read operation.  This value
    should be such that OPTION_DRIVER_NV_ONSEMI_CAT24C512_MAX_WAIT_RETRIES * OPTION_DRIVER_NV_ONSEMI_CAT24C512_WAIT_WRITE_COMPLETE_MS
    is greater than the maximum write/erase cycle time.
 */
#ifndef OPTION_DRIVER_NV_ONSEMI_CAT24C512_MAX_WAIT_RETRIES
#define OPTION_DRIVER_NV_ONSEMI_CAT24C512_MAX_WAIT_RETRIES          6
#endif


///
namespace Driver {
///
namespace NV {
///
namespace Onsemi {
///
namespace CAT24C512 {


/** This class implements the Non-volatile storage driver for the Onsemi CAT24C512
    serial 512Kb (64KB) I2C EEPROM.

    The implementation ASSUMES (i.e. requires) that the Application is RESPONSIBLE
    for starting/stopping the i2cDriver since it is a shared resource.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class Api : public Driver::NV::Api
{
public: 
    /// Constructor 
    Api( Driver::I2C::Master& i2cDriver, uint8_t i2cDevice7BitAddress );

public:
    /// See Driver::NV::Api
    bool start() noexcept;

    /// See Driver::NV::Api
    void stop() noexcept;

    /// See Driver::NV::Api
    bool write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept;
    
    /// See Driver::NV::Api
    bool read( size_t srcOffset, void* dstData, size_t numBytesToRead ) noexcept;

    /// See Driver::NV::Api
    size_t getNumPages() const noexcept;

    /// See Driver::NV::Api
    size_t getPageSize() const noexcept;
    
protected:
    /// Helper method to set the EEPROM internal address/offset pointer. 
    bool setReadOffsetPointer( size_t newOffset );

    /// Helper method to write data to the EEPROM
    bool writePage( size_t offset, const uint8_t* srcBuffer, size_t numBytesToWrite );

protected:
    /// I2C driver
    Driver::I2C::Master&    m_i2cDriver;
    
    /// Device address of 'my' EEPROM IC
    uint8_t                 m_devAddress;

    /// Started state
    bool                    m_started;
};




};      // end namespaces
};
};
};
#endif  // end header latch

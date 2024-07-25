#ifndef Driver_SPI_Master_h_
#define Driver_SPI_Master_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2023  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */


#include <stdlib.h>

///
namespace Driver {
///
namespace SPI {


/** This class defines a non-platform specific interface for an SPI master device
    driver. The intended usage is to create ONE driver per physical SPI bus, i.e.
    the driver instance can be shared with multiple clients.  
    
    The class does NOT manage the chip/slave select signal, i.e. the client is
    responsible for asserting and de-asserting the chip/slave select before and 
    after a transaction.

    The driver is NOT thread safe.  It is the responsibility of the Application
    to ensure thread safety when driver is used and/or shared with multiple 
    clients.
 */
class Master
{
public:
    /** This method is used initialize/start the driver.  To 'restart' the driver,
        the application must call stop(), then start().

        When 'newBaudRateHz' is non-zero, the SPI's transfer frequency is updated
        (from what was provided in the constructor).  When the driver is stopped
        and restarted, the default baudrate on restart will be the value provided
        in the constructor.

        The method returns true if successful; else false is returned when an
        error occurred.  If false is returned, future read/write calls will always
        return a failure.
     */
    virtual bool start( size_t newBaudRateHz = 0 ) noexcept = 0;

    /// This method is used to stop/shutdown the driver.
    virtual void stop() noexcept = 0;

public:
    /** This method writes 'numBytes' and reads 'numBytes' to/from the peripheral 
        device.  The buffer pointed to by 'dstData' must be at least 'numBytes'
        in size. If 'dstData' is nullptr, then the incoming bytes are NOT copied.
        
        Returns true if successful; else false is returned.
     */
    virtual bool  transfer( size_t      numBytes,
                            const void* srcData,
                            void*       dstData = nullptr ) noexcept = 0;
                            
public:
    /// Virtual destructor
    virtual ~Master() {}
};




};      // end namespaces
};
#endif  // end header latch

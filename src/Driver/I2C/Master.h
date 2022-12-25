#ifndef Driver_I2C_Master_h_
#define Driver_I2C_Master_h_
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


#include <stdint.h>
#include <stdlib.h>

///
namespace Driver {
///
namespace I2C {


/** This class defines a non-platform specific interface for an I2C master device
    driver. The intended usage is to create ONE driver per physical I2C bus, i.e.
    the driver instance can/should-be shared with multiple clients.  
    
    The driver is thread safe in that ALL operations are atomic. This means that
    all of the methods have the potential to BLOCK if another client/thread has
    in-progress method call.
 */
class Master
{
public:
    /// Result codes
    enum Result_T
    {
        eSUCCESS = 0,   //!< Operation was successful
        eNO_ACK,        //!< No acknowledgment received for the device address or the device is not present
        eTIMEOUT,       //!< The operation was aborted due to a timeout error
        eNOT_STARTED,   //!< The driver was not properly started
        eERROR          //!< Generic/non-classified error
    };

public:
    /** This method is used initialize/start the driver.  To 'restart' the driver,
        the application must call stop(), then start().

        The method returns true if successful; else false is returned when an
        error occurred.  If false is returned, future read/write calls will always
        return a failure.
     */
    virtual bool start() noexcept = 0;

    /// This method is used to stop/shutdown the driver.
    virtual void stop() noexcept = 0;


public:
    /** This method writes 'numBytesToTransmit' from 'srcData' to the I2C 
        peripheral device.  

        If 'noStop' is true, the driver retains control of the bus at the end
        of the transfer (no Stop is issued), and the next I2C transaction will 
        begin with a Restart rather than a Start.
     */
    virtual Result_T  writeToDevice( uint8_t        device7BitAddress, 
                                     size_t         numBytesToTransmit,
                                     const void*    srcData,
                                     bool           noStop = false ) noexcept = 0;

    /** This method reads 'numBytesToRead' bytes from the I2C peripheral device
        into 'dstData'. The application is RESPONSIBLE for ensure that 'dstData' 
        is AT LEAST 'numBytesToRead' in size.

        If 'noStop' is true, the driver retains control of the bus at the end
        of the transfer (no Stop is issued), and the next transfer will begin
        with a Restart rather than a Start.
     */
    virtual Result_T readFromDevice( uint8_t   device7BitAddress,
                                     size_t    numBytesToRead,
                                     void*     dstData,
                                     bool      noStop = false ) = 0;


public:
    /** This method changes the default/current I2C baud-rate.  The application
        is responsible for setting values that are supported by the actual
        platform hardware.  The method returns the previous baud rate setting.

        Baud rate is Hz, e.g. 100kHz is 100000.

        The default baud rate is specific to the concrete driver.

        NOTE: This method can ONLY be called when there is no I2C transaction
              in progress.  The Application is RESPONSIBLE for enforcing this
              constraint.

        This method can be called before start() is called.
     */
    virtual size_t setBaudRate( size_t newBaudRateHz ) noexcept = 0;

    /** This method changes the default/current timeout duration.  The timeout
        duration is the maximum amount of time, in milliseconds, the driver 
        will wait for a I2C transaction to complete.   The method returns the 
        previous timeout setting.

        The default timeout is specific to the concrete driver.
        
        NOTE: This method can ONLY be called when there is no I2C transaction
              in progress.  The Application is RESPONSIBLE for enforcing this
              constraint.    

        This method can be called before start() is called.
     */
    virtual size_t setTransactionTimeout( size_t maxTimeMs ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~Master() {}
};




};      // end namespaces
};
#endif  // end header latch

#ifndef Cpl_Io_Serial_ST_M32F4_InputOutput_h_
#define Cpl_Io_Serial_ST_M32F4_InputOutput_h_
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

#include "Cpl/Io/InputOutput.h"
#include "Cpl/Io/Serial/ST/M32F4/StreamDriver.h"

///
namespace Cpl {
///
namespace Io {
///
namespace Serial {
///
namespace ST {
///
namespace M32F4 {


/** This concrete class implements the InputOutput stream interface
    built on top of ST HAL Layer/SDK.

    The implementation ASSUMES that at most there is only ONE client attempt to
    use the InputOutput stream at any given time.  It is okay to call the
    read(), write(), etc.from  different threads - but the calls CANNOT be
    concurrent.  It is the application's responsibility to provide additional
    thread-safety/concurrence protection.

    Note: The 'receiver' and the 'transmitter' methods MAY be called concurrently.
 */

class InputOutput : public Cpl::Io::InputOutput
{
protected:
    /// Driver
    StreamDriver m_driver;


public:
    /// Constructor
    InputOutput( Cpl::Container::RingBuffer<uint8_t>& txBuffer,
                 Cpl::Container::RingBuffer<uint8_t>& rxBuffer );

    /// Destructor
    ~InputOutput( void );


public:
    /** This method MUST be used to finish the initialization of the stream
        and start is underlying drivers.  The application is responsible
        for fully initializing 'uartHdlToUse' BEFORE calling this method.

        NOTE: The stream CAN be restarted (with a possibly different UART handle)
              if close() has been called.
     */
    void start( IRQn_Type           uartIrqNum, 
                UART_HandleTypeDef*	uartHdlToUse ) noexcept;


public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::InputOutput::read;

    /// See Cpl::Io::Input
    bool read( void* buffer, int numBytes, int& bytesRead );

    /// See Cpl::Io::Input
    bool available();

    /// This method is used to retreive and optionally clear the RX Error counter
    size_t getRxErrorsCounts( bool clearCount=true ) noexcept;

public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::InputOutput::write;

    /// See Cpl::Io::Output
    bool write( const void* buffer, int maxBytes, int& bytesWritten );

    /// See Cpl::Io::Output
    void flush();

    /// See Cpl::Io::IsEos. Note: This method always returns false
    bool isEos();

    /// See Cpl::Io::Output
    void close();

};

};      // end namespaces
};
};
};
};
#endif  // end header latch

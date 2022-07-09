#ifndef Cpl_Io_Serial_Adafruit_Nrf5_BLE_InputOutput_h
#define Cpl_Io_Serial_Adafruit_Nrf5_BLE_InputOutput_h
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
/** @file */

#include "Cpl/Io/InputOutput.h"
#include "libraries/Bluefruit52Lib/src/services/BLEUart.h"   

///
namespace Cpl {
namespace Io {
namespace Serial {
namespace Adafruit {
namespace Nrf5 {
namespace BLE {


/** This concrete class implements the InputOutput stream interface
    as a wrapper around Adafruit's BLEUart class. The BLEUart class
    is a client/peripheral service for Nordic's Uart Service (NUS).

    The implementation assumes that application is responsible for
    starting the Bluetooth stack and enabling advertising (and any other
    BLE servces).

    USAGE:
    // Create an instance using the Arduino primary serial port for 'stdio'
    static BLEUart actualBleUart( NRF_UART0, NRF_UART0_IRQn, PIN_SERIAL_RX, PIN_SERIAL_TX );
    static Cpl::Io::Serial::Adafruit::Nrf5::BLE::InputOutput myStdio( actualBleUart );

 */

class InputOutput : public Cpl::Io::InputOutput
{
protected:
    /// Adafruit's BLE UART driver
    BLEUart&        m_commPort;

    /// Flag to keep track of started/stopped state
    bool            m_started;


public:
    /** Constructor.
     */
    InputOutput( BLEUart&  commPort );   //!< Reference to an Arduino 'BLE UART' that provides the actual transfer of data

    /// Destructor
    virtual ~InputOutput( void );


public:
    /** This method starts UART and must be called BEFORE any of
        InputOutput methods are called. This allows the instance to
        be statically created and the initialized/started once the
        'system is running' (aka in the Arduino setup() method). It
        is the same as calling 'bleuart.begin();

        This method should be called BEFORE starting BLE advertising

        Note: If using an instance of this class for CPL Tracing logging, then
              the start() method MUST be called BEFORE the Cpl::System::Initialize()
              method is called.
     */
    void start( void ) noexcept;

public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::InputOutput::read;

    /// See Cpl::Io::Input
    bool read( void* buffer, int numBytes, int& bytesRead );

    /// See Cpl::Io::Input
    bool available( void );

public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::InputOutput::write;

    /// See Cpl::Io::Output
    bool write( const void* buffer, int maxBytes, int& bytesWritten );

    /// See Cpl::Io::Output.  This method does nothing.  See comments about the semantics of the write() methods
    void flush();

    /// See Cpl::Io::IsEos.
    bool isEos();

    /// See Cpl::Io::Output.  Note: An can be 're-opened' by calling the start() method.
    void close();

};

};      // end namespaces
};
};
};
};
};
#endif  // end header latch

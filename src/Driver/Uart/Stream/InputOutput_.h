#ifndef Driver_Uart_Stream_InputOutput_h_
#define Driver_Uart_Stream_InputOutput_h_
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
#include "Driver/Uart/Stream/Transmitter.h"
#include "Driver/Uart/Stream/Receiver.h"

///
namespace Driver {
///
namespace Uart {
///
namespace Stream {


/** This private concrete class implements the InputOutput stream interface
	using the UART stream driver.  A concrete sub class is required to
	provide the actual UART initialization component (i.e. the platform
	specific part of the stream driver.

	The implementation ASSUMES that at most there is only ONE client attempt to
	use the InputOutput stream at any given time.  It is okay to call the
	read(), write(), etc.from  different threads - but the calls CANNOT be
	concurrent.  It is the application's responsibility to provide additional
	thread-safety/concurrence protection. Note: The 'receiver' and the
	'transmitter' methods MAY be called concurrently.

 */

class InputOutput_ : public Cpl::Io::InputOutput
{
protected:
	/// Receiver
	Receiver    m_rx;

	/// Transmitter
	Transmitter m_tx;


protected:
	/** Constructor. The child class is responsible for providing the HAL
		UART Handle of an INITIALIZED UART.
	 */
	InputOutput_( Driver_Uart_Hal_T uartHdl,         //!< HAL handle for the initialized UART
				  bool              manualFirstTx,   //!< Tx-first-behavior of the UART
				  uint8_t           memTxBuffer[],   //!< Memory for the transmit buffer
				  size_t            txBufSize,       //!< Size, in bytes, of the transmit buffer   
				  uint8_t           memRxBuffer[],   //!< Memory for the receive buffer
				  size_t            rxBufSize        //!< Size, in bytes, of the receive buffer   
	);

	/// Destructor
	~InputOutput_( void );


protected:
	/// This method is used by a sub-class to start the internal drivers
	void start( void ) noexcept;


public:
	/// Pull in overloaded methods from base class
	using Cpl::Io::InputOutput::read;

	/// See Cpl::Io::Input
	bool read( void* buffer, int numBytes, int& bytesRead );

	/// See Cpl::Io::Input
	bool available();


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
#endif  // end header latch

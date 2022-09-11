#ifndef Driver_Uart_Stream_Receiver_h_
#define Driver_Uart_Stream_Receiver_h_
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

#include "Driver/Uart/Hal.h"
#include "Cpl/Container/RingBuffer.h"
#include "Cpl/System/Thread.h"



/// 
namespace Driver {
/// 
namespace Uart {
/// 
namespace Stream {


/** This concrete class implements a blocking Receive Stream UART driver. What
	does 'blocking Transmit Stream' mean?  'Blocking' means that the calling
	thread will block until at least one new byte has been received. 'Stream'
	means that data is received as a continual stream of bytes, i.e. the data is
	 NOT broken down into frames, packets, etc. (think C stdin stream).

	This driver ASSUMES that at most there is only ONE client attempt to use the
	driver at any given time.  It is okay to call the start(), stop(), read()
	from  different threads - but the calls CANNOT be concurrent.  It is the
	application's  responsibility to provide additional
	thread-safety/concurrence protection.
 */
class Receiver
{
protected:
	/// Handle to my low level hardware
	Driver_Uart_Hal_T       m_uartHdl;

	/// Handle of the blocked client thread (if there is one)
	Cpl::System::Thread*    m_waiterPtr;

	/// A Receive error was encountered
	size_t                  m_errCount;

	/// Transmit buffer
	Cpl::Container::RingBuffer<uint8_t> m_buffer;

	/// Started state of the driver
	bool                    m_started;




public:
	/** Constructor. Note: The driver does not begin operating until start() is
		called.

		@param uartHdl          The HAL layer UART Handle.  The application is
								required to initialize the low level UART before
								starting this driver.
		@param bufSize          The number of bytes in the supplied receive
								buffer.
		@param bufMem           Array of bytes for the receive/inbound buffer.
	 */
	Receiver( Driver_Uart_Hal_T uartHdl, unsigned bufSize, uint8_t bufMem[] ) noexcept;



public:
	/** This method starts/enables the driver.  Note: The application is
		RESPONSIBLE for performing the low level initialization (Baud Rate,
		framing, etc.) of the UART BEFORE this method is called. For the
		application to change the Baud rate, framing, etc - it must first stop
		the driver and then restart it.
	 */
	void start( void ) noexcept;

	/** This method will stop/disable the driver.  The driver can be restarted
		by call start() again.  The state of the contents of the inbound buffer
		and the byte(s) 'incoming' when the driver is stop is undetermined.
	 */
	void stop( void ) noexcept;



public:
	/** Receives at most the specified maximum number of bytes.  The actual
		number of bytes received is returned via 'numBytesRx'.  The method does
		not  return until at least one byte is available in the inbound buffer.
		The  method returns true if successful; else false is returned.

		NOTE: UART Framing errors are silents discarded, i.e. an incoming
			  byte that is received with an associated framing error is
			  NOT put into the inbound buffer.  A free running counter is
			  maintain of the number of framing errors encountered.
	 */
	bool read( void* data, size_t maxBytes, size_t& numBytesRx ) noexcept;


	/** This method returns true if at least one byte is available in the
		inbound buffer.
	 */
	bool available( void ) const noexcept;


	/** This method returns and optionally clears the driver's framing error
		counter.
	 */
	size_t getFramingErrorsCount( bool clearCount=true ) noexcept;


public:
	/** This is a quasi-private method that the application should NOT call. The
		method SHOULD only be called by the interrupt service routine (ISR) for
		the 'RX-Data-Available interrupt for the UART identified by the
		'uartHdl' supplied  in the constructor.  This requires that a reference
		(at a minimum) to the  driver instance be a global variable.

		The method returns the result of signaling waiter (i.e. return code
		from su_signal()), or zero if no waiter was signaled.
	 */
	int su_rxDataAndErrorIsr_( void ) noexcept;


private:
	/// Prevent access to the copy constructor -->Driver can not be copied!
	Receiver( const Receiver& m );

	/// Prevent access to the assignment operator -->Driver can not be copied!
	const Receiver& operator=( const Receiver& m );

};



};      // end namespaces
};
};
#endif  // end header latch

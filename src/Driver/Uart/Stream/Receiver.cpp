/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Receiver.h"
#include "Bsp/Api.h"


/// 
using namespace Driver::Uart::Stream;


////////////////////////
Receiver::Receiver( Driver_Uart_Hal_T uartHdl, unsigned bufSize, uint8_t bufMem[] ) noexcept
	:m_uartHdl( uartHdl ),
	m_waiterPtr( 0 ),
	m_errCount( 0 ),
	m_buffer( bufSize, bufMem ),
	m_started( false )
{
}



////////////////////////
void Receiver::start( void ) noexcept
{
	// If in the wrong state -->get in the correct state
	if ( m_started )
	{
		stop();
	}

	// Housekeeping
	m_started = true;

	// Enable the RX side of the UART
	Driver_Uart_Hal_enableRx( m_uartHdl );
	Driver_Uart_Hal_enableRxIrq( m_uartHdl );
}


void Receiver::stop( void ) noexcept
{
	// Disable the RX side of the UART
	Driver_Uart_Hal_disableRxIrq( m_uartHdl );
	Driver_Uart_Hal_disableRx( m_uartHdl );

	// Free up any blocked client (if there is one)
	if ( m_waiterPtr )
	{
		m_waiterPtr->signal();
	}

	// Housekeeping
	m_buffer.clearTheBuffer();
	m_waiterPtr = 0;
	m_started   = false;
	m_errCount  = 0;
}




////////////////////////
bool Receiver::available( void ) const noexcept
{
	// NOTE: If the driver is NOT started, then the RingBuffer count will always be zero -->i.e. returns false

	// INTERRUPT/CRITICAL SECTION: Get Ring Buffer count
	Bsp_Api_disableIrqs();
	unsigned count = m_buffer.getNumItems();
	Bsp_Api_enableIrqs();

	return count > 0;
}


size_t Receiver::getFramingErrorsCount( bool clearCount ) noexcept
{
	// INTERRUPT/CRITICAL SECTION: Get RX Framing Error counts
	Bsp_Api_disableIrqs();
	size_t count = m_errCount;
	if ( clearCount )
	{
		m_errCount = 0;
	}
	Bsp_Api_enableIrqs();

	return count;
}


bool Receiver::read( void* data, size_t maxBytes, size_t& numBytesRx ) noexcept
{
	numBytesRx = 0;

	// Return an error if the drive has not yet been started
	if ( !m_started )
	{
		return false;
	}

	// Do nothing if app is requesting zero bytes to read
	if ( maxBytes == 0 )
	{
		return true;
	}

	// Block if this is no data available
	if ( !available() )
	{
		Cpl::System::Thread& myThread = Cpl::System::Thread::getCurrent();

		// INTERRUPT/CRITICAL SECTION: Set Waiter
		Bsp_Api_disableIrqs();
		m_waiterPtr = &myThread;
		Bsp_Api_enableIrqs();

		// Wait for incoming byte
		Cpl::System::Thread::wait();
	}

	// Housekeeping
	uint8_t  byteIn;
	uint8_t* ptr = (uint8_t*) data;

	// Read as much a possible (at this point there is at least one byte in the inbound buffer)
	while ( maxBytes )
	{
		// INTERRUPT/CRITICAL SECTION: Get the next byte from the inbound buffer
		Bsp_Api_disableIrqs();
		bool result = m_buffer.remove( byteIn );
		Bsp_Api_enableIrqs();

		// Copy the inbound byte to the client's data buffer
		if ( result )
		{
			*ptr++ = byteIn;
			numBytesRx++;
			maxBytes--;
		}

		// No data (or no more data) is available -->exit the loop
		else
		{
			break;
		}
	}

	// Always return success
	return true;
}



int Receiver::su_rxDataAndErrorIsr_( void ) noexcept
{
	int result = 0;

	// Check for RX errors
	if ( Driver_Uart_Hal_isRxError( m_uartHdl ) )
	{
		Driver_Uart_Hal_clrRxErrors( m_uartHdl );
		Driver_Uart_Hal_clrRxIrq( m_uartHdl );
		m_errCount++;
	}

	// Check for a byte received 
	else if ( Driver_Uart_Hal_isRxIrq( m_uartHdl ) )
	{
		// Clear the RX interrupt request flag
		Driver_Uart_Hal_clrRxIrq( m_uartHdl );

		// Add incoming byte to the inbound buffer
		uint8_t byte = Driver_Uart_Hal_getRxByte( m_uartHdl );
		m_buffer.add( byte );

		// unblock waiting client (if there is one) now that I have least one byte
		if ( m_waiterPtr )
		{
			result      = m_waiterPtr->su_signal();
			m_waiterPtr = 0;
		}
	}

	return result;
}

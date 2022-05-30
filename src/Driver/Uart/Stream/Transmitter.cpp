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

#include "Transmitter.h"
#include "Bsp/Api.h"


/// 
using namespace Driver::Uart::Stream;


////////////////////////
Transmitter::Transmitter( Driver_Uart_Hal_T uartHdl, unsigned bufSize, uint8_t bufMem[], bool manualFirstTx ) noexcept
	:m_uartHdl( uartHdl ),
	m_waiterPtr( 0 ),
	m_buffer( bufSize, bufMem ),
	m_started( false ),
	m_manualFirstTx( manualFirstTx )
{
}



////////////////////////
void Transmitter::start( void ) noexcept
{
	// If in the wrong state -->get in the correct state
	if ( m_started )
	{
		stop();
	}

	// Housekeeping
	m_started = true;

	// Enable the TX side of the UART
	Driver_Uart_Hal_enableTx( m_uartHdl );
}


void Transmitter::stop( void ) noexcept
{
	// Disable the TX side of the UART
	Driver_Uart_Hal_disableTxIrq( m_uartHdl );
	Driver_Uart_Hal_disableTx( m_uartHdl );

	// Free up any blocked client (if there is one)
	if ( m_waiterPtr )
	{
		m_waiterPtr->signal();
	}

	// Housekeeping
	m_buffer.clearTheBuffer();
	m_waiterPtr = 0;
	m_started   = false;
}



////////////////////////
bool Transmitter::write( const void* data, size_t numBytesToTx ) noexcept
{
	// Fail the write if the driver has not been started
	if ( !m_started )
	{
		return false;
	}

	// Do nothing if no data needs to be transmitted
	if ( numBytesToTx == 0 )
	{
		return true;
	}

	// Loop until all data has been transferred to the outbound buffer
	uint8_t* ptr = (uint8_t*) data;
	for ( ;;)
	{
		// Transfer my data to the outbound buffer
		while ( numBytesToTx )
		{
			// INTERRUPT/CRITICAL SECTION: Populate Ring Buffer
			Bsp_Api_disableIrqs();
			bool result = m_buffer.add( *ptr );
			Bsp_Api_enableIrqs();

			// Get the next byte if my Ring Buffer was not full
			if ( result )
			{
				ptr++;
				numBytesToTx--;
			}

			// Ring Buffer is full -->skip to transmitting
			else
			{
				break;
			}
		}

		// If not all my outbound data fits in the buffer -->I will block
		if ( numBytesToTx )
		{
			Cpl::System::Thread& myThread = Cpl::System::Thread::getCurrent();

			// INTERRUPT/CRITICAL SECTION: Set Waiter
			Bsp_Api_disableIrqs();
			m_waiterPtr = &myThread;
			Bsp_Api_enableIrqs();
		}


		// Start the initial transmit my outbound buffer
		if ( !Driver_Uart_Hal_isTxIrqEnabled( m_uartHdl ) )
		{
			// Enable TX Done interrupt to trigger the initial byte
			if ( m_manualFirstTx == false )
			{
				Driver_Uart_Hal_enableTxIrq( m_uartHdl );
			}

			// Brute force approach to trigger the initial byte
			else
			{
				// Get the first/next byte from the ring buffer
				uint8_t byte;
				m_buffer.remove( byte );

				// Transmit/load first byte
				Driver_Uart_Hal_resetTxPipe( m_uartHdl );
				Driver_Uart_Hal_enableTxIrq( m_uartHdl );
				Driver_Uart_Hal_transmitByte( m_uartHdl, byte );
			}
		}


		// Wait (if necessary) for buffer to be transmitted/drained
		if ( numBytesToTx )
		{
			Cpl::System::Thread::wait();
		}

		// All done -->exit outer loop
		else
		{
			break;
		}
	}

	return true;
}



int Transmitter::su_txDoneIsr_( void ) noexcept
{
	uint8_t byte;
	int     result = 0;

	// Do nothing if there is no transmit IRQ
	if ( Driver_Uart_Hal_isTxIrq( m_uartHdl ) )
	{
		// Ensure the interrupt request gets cleared
		Driver_Uart_Hal_clrTxIrq( m_uartHdl );

		// Transmit the next byte
		if ( m_buffer.remove( byte ) )
		{
			Driver_Uart_Hal_transmitByte( m_uartHdl, byte );
		}

		// Out of data -->disable TX interrupt
		else
		{
			Driver_Uart_Hal_disableTxIrq( m_uartHdl );

			// unblock waiting client (if there is one) once I am out of data
			if ( m_waiterPtr )
			{
				result      = m_waiterPtr->su_signal();
				m_waiterPtr = 0;
			}
		}
	}

	return result;
}

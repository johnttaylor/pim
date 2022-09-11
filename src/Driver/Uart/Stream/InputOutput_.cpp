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

#include "InputOutput_.h"

///
using namespace Driver::Uart::Stream;



////////////////////////////////////
InputOutput_::InputOutput_( Driver_Uart_Hal_T uartHdl,
	bool                  manualFirstTx,
	uint8_t               memTxBuffer[],
	size_t                txBufSize,
	uint8_t               memRxBuffer[],
	size_t                rxBufSize
)
	:m_rx( uartHdl, rxBufSize, memRxBuffer ),
	m_tx( uartHdl, txBufSize, memTxBuffer, manualFirstTx )
{
}


InputOutput_::~InputOutput_( void )
{
	close();
}


void InputOutput_::start( void ) noexcept
{
	m_rx.start();
	m_tx.start();
}


////////////////////////////////////
bool InputOutput_::read( void* buffer, int numBytes, int& bytesRead )
{
	size_t bytesIn = 0;
	bool   result  = m_rx.read( buffer, (size_t) numBytes, bytesIn );
	bytesRead      = (int) bytesIn;
	return result;
}

bool InputOutput_::available()
{
	return m_rx.available();
}


////////////////////////////////////
bool InputOutput_::write( const void* buffer, int maxBytes, int& bytesWritten )
{
	bytesWritten = maxBytes;
	return m_tx.write( buffer, (size_t) maxBytes );
}


void InputOutput_::flush()
{
	// Not supported/has no meaning for a serial port
}

bool InputOutput_::isEos()
{
	// Does not really have meaning for serial port (assuming the serial port is opened/active)
	return false;
}

void InputOutput_::close()
{
	m_tx.stop();
	m_rx.stop();
}

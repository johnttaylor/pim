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

#include "colony_config.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/FatalError.h"
#include "Driver/Uart/Stream/Transmitter.h"
#include "Driver/Uart/Stream/Receiver.h"
#include "Cpl/Text/FString.h"
#include <string.h>


#define SECT_     "_0test"

#ifndef TEXT_TX_RINGBUFFER_SIZE
#define TEXT_TX_RINGBUFFER_SIZE     7
#endif

#ifndef TEXT_RX_RINGBUFFER_SIZE
#define TEXT_RX_RINGBUFFER_SIZE     11
#endif

#define MAX_MESSAGE                 128

#define MSG1                        "[Bob's your uncle, well at least he is someones uncle]"
#define MSG2                        "[3.14159]"
#define MSG3                        "[And the traditional... hello world!]"


/// 
using namespace Driver::Uart::Stream;

extern void loopback_test( Driver_Uart_Hal_T uartHdl, bool manualFirstTx );

Transmitter* txPtr;
Receiver*    rxPtr;


////////////////////////////////////////////////////////////////////////////////
namespace {



class Tx : public Cpl::System::Runnable
{
public:
	///
	Transmitter m_tx;
	/// 
	uint8_t     m_bufMem[TEXT_TX_RINGBUFFER_SIZE];
	///
	const char* m_msg1;
	///
	const char* m_msg2;
	///
	const char* m_msg3;


public:
	///
	Tx( Driver_Uart_Hal_T uartHdl, bool manualFirstTx, const char* msg1, const char* msg2, const char* msg3 )
		:m_tx( uartHdl, TEXT_TX_RINGBUFFER_SIZE, m_bufMem, manualFirstTx ),
		m_msg1( msg1 ),
		m_msg2( msg2 ),
		m_msg3( msg3 )
	{
	}

public:
	///
	void appRun()
	{
		// start the driver
		m_tx.start();

		size_t loopCount = 0;
		for ( ;;)
		{
			Bsp_Api_toggle_debug1();

			Cpl::System::Thread::wait();
			m_tx.write( m_msg1, strlen( m_msg1 ) );
			Cpl::System::Thread::wait();
			m_tx.write( m_msg2, strlen( m_msg2 ) );
			Cpl::System::Thread::wait();
			m_tx.write( m_msg3, strlen( m_msg3 ) );
			loopCount++;
		}
	}
};

class Rx : public Cpl::System::Runnable
{
public:
	///
	Receiver                        m_rx;
	///
	Tx&                             m_tx;
	/// 
	uint8_t                         m_bufMem[TEXT_TX_RINGBUFFER_SIZE];
	///
	Cpl::System::Thread&            m_txThread;
	///
	Cpl::Text::FString<MAX_MESSAGE> m_inMsg;
	///
	char                            m_temp[MAX_MESSAGE];

public:
	Rx( Cpl::System::Thread& txThread, Tx& tx, Driver_Uart_Hal_T uartHdl )
		:m_rx( uartHdl, TEXT_TX_RINGBUFFER_SIZE, m_bufMem ),
		m_tx( tx ),
		m_txThread( txThread )
	{
	}

public:
	void appRun()
	{
		m_rx.start();

		// Throw any trash bytes on startup
		while ( m_rx.available() )
		{
			size_t dummy = 0;
			m_rx.read( m_temp, MAX_MESSAGE, dummy );
		}

		size_t loopCount = 0;
		for ( ;;)
		{
			if ( !rx_message( m_tx.m_msg1 ) )
			{
				Cpl::System::FatalError::logf( "Failed 'msg1' loopback" );
			}
			if ( !rx_message( m_tx.m_msg2 ) )
			{
				Cpl::System::FatalError::logf( "Failed 'msg2' loopback" );
			}
			if ( !rx_message( m_tx.m_msg3 ) )
			{
				Cpl::System::FatalError::logf( "Failed 'msg3' loopback" );
			}

			loopCount++;
		}
	}

	bool rx_message( const char* msg )
	{
		size_t len   = strlen( msg );
		size_t rxlen;
		m_inMsg.clear();

		m_txThread.signal();
		while ( len )
		{
			m_rx.read( m_temp, len, rxlen );
			m_inMsg.appendTo( m_temp, rxlen );
			len -= rxlen;
		}

		return m_inMsg == msg;
	}
};


class Led : public Cpl::System::Runnable
{
public:
	/// 
	int m_onTime;
	///
	int m_offTime;

public:
	///
	Led( int ontime, int offtime )
		:m_onTime( ontime ),
		m_offTime( offtime )
	{
	}

	///
	void appRun()
	{
		for ( ;;)
		{
			Bsp_Api_toggle_debug2();
			Cpl::System::Api::sleep( m_onTime );
			Bsp_Api_toggle_debug2();
			Cpl::System::Api::sleep( m_offTime );
		}
	}
};


};  // end namespace


////////////////////////////////////////////////////////////////////////////////


void loopback_test( Driver_Uart_Hal_T uartHdl, bool manualFirstTx )
{
	// Create some threads....
	Tx*                  transmitterPtr = new( std::nothrow ) Tx( uartHdl, manualFirstTx, MSG1, MSG2, MSG3 );
	Cpl::System::Thread* ptr            = Cpl::System::Thread::create( *transmitterPtr, "TX" );
	txPtr                               = &( transmitterPtr->m_tx );

	Rx* receiverPtr = new( std::nothrow ) Rx( *ptr, *transmitterPtr, uartHdl );
	Cpl::System::Thread::create( *receiverPtr, "RX" );
	rxPtr           = &( receiverPtr->m_rx );

	Led* blinkPtr = new( std::nothrow ) Led( 250, 750 );
	Cpl::System::Thread::create( *blinkPtr, "LEDs" );


	// Start the schedular
	Cpl::System::Api::enableScheduling();
}



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

#include "colony_config.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Io/InputOutput.h"
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



extern void loopback_test( Cpl::Io::InputOutput& fd );



////////////////////////////////////////////////////////////////////////////////
namespace {



class Tx : public Cpl::System::Runnable
{
public:
	///
	Cpl::Io::Output& m_fd;
	///
	const char* m_msg1;
	///
	const char* m_msg2;
	///
	const char* m_msg3;


public:
	///
	Tx( Cpl::Io::Output& fd, const char* msg1, const char* msg2, const char* msg3 )
		:m_fd( fd ),
		m_msg1( msg1 ),
		m_msg2( msg2 ),
		m_msg3( msg3 )
	{
	}

public:
	///
	void appRun()
	{
		size_t loopCount = 0;
		for ( ;;)
		{
			Bsp_Api_toggle_debug1();

			Cpl::System::Thread::wait();
			m_fd.write( m_msg1 );
			Cpl::System::Thread::wait();
			m_fd.write( m_msg2 );
			Cpl::System::Thread::wait();
			m_fd.write( m_msg3 );
			loopCount++;
		}
	}
};

class Rx : public Cpl::System::Runnable
{
public:
	///
	Cpl::Io::Input&                 m_fd;
	///
	Tx&                             m_tx;
	///
	Cpl::System::Thread&            m_txThread;
	///
	Cpl::Text::FString<MAX_MESSAGE> m_inMsg;
	///
	char                            m_temp[MAX_MESSAGE];

public:
	Rx( Cpl::Io::Input& fd, Cpl::System::Thread& txThread, Tx& tx )
		:m_fd( fd ),
		m_tx( tx ),
		m_txThread( txThread )
	{
	}

public:
	void appRun()
	{
		// Throw any trash bytes on startup
		while ( m_fd.available() )
		{
			m_fd.read( m_inMsg );
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
		m_txThread.signal();

		int len = strlen( msg );
		m_inMsg.clear();
		while ( len )
		{
			char c;
			m_fd.read( c );
			m_inMsg += c;
			len--;
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


void loopback_test( Cpl::Io::InputOutput& fd )
{
	// Create some threads....
	Tx*                  transmitterPtr = new( std::nothrow ) Tx( fd, MSG1, MSG2, MSG3 );
	Cpl::System::Thread* ptr            = Cpl::System::Thread::create( *transmitterPtr, "TX" );

	Rx* receiverPtr = new( std::nothrow ) Rx( fd, *ptr, *transmitterPtr );
	Cpl::System::Thread::create( *receiverPtr, "RX" );

	Led* blinkPtr = new( std::nothrow ) Led( 250, 750 );
	Cpl::System::Thread::create( *blinkPtr, "LEDs" );


	// Start the scheduler
	Cpl::System::Api::enableScheduling();
}



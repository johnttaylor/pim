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

#include "Socket.h"
#include "Cpl/System/Thread.h"


///
using namespace Cpl::TShell;


/////////////////////////////////////////
Socket::Socket( ProcessorApi& shell, Cpl::Io::Socket::Listener& listener, const char* threadName, int threadPriority, bool thisIsAStaticInstance ) noexcept
	: m_shell( shell )
	, m_threadPtr( 0 )
	, m_priority( threadPriority )
	, m_name( threadName )
	, m_listener( listener )
	, m_staticInstance( thisIsAStaticInstance )
{
}

Socket::~Socket()
{
	if ( !m_staticInstance && m_threadPtr )
	{
		Cpl::System::Thread::destroy( *m_threadPtr );
	}
}


/////////////////////////////////////////
void Socket::launch( int portNumToListenOn ) noexcept
{
	// If I am re-launched -->kill the previous shell
	if ( m_threadPtr )
	{
		Cpl::System::Thread::destroy( *m_threadPtr );
	}

	// Create Listener/Shell thread and start listening for a socket connection.
	m_threadPtr = Cpl::System::Thread::create( m_listener, m_name, m_priority );
	m_listener.startListening( *this, portNumToListenOn );
}



/////////////////////////////////////////
bool Socket::newConnection( Cpl::Io::Descriptor newFd, const char* rawConnectionInfo )
{
	m_stream.activate( newFd );
	m_shell.start( m_stream, m_stream );
	m_stream.close();
	return true;
}

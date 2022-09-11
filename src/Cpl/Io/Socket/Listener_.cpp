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

#include "Listener_.h"
#include "Cpl/System/FatalError.h"



//
using namespace Cpl::Io::Socket;


///////////////////////////////
Listener_::Listener_()
	:m_socket( 0 ),
	m_clientPtr( 0 ),
	m_myThreadPtr( 0 ),
	m_startCalled( false )
{
}

///////////////////////////////
void Listener_::startListening( Listener::Client& client, int portNumToListenOn )
{
	Cpl::System::Mutex::ScopeBlock lock( m_lock );

	if ( !m_startCalled )
	{
		m_startCalled 	= true;
		m_socket  		= portNumToListenOn;
		m_clientPtr 	= &client;

		if ( m_myThreadPtr )
		{
			m_myThreadPtr->signal();
		}
	}
}

void Listener_::terminate()
{
	m_lock.lock();
	bool started = m_startCalled;
	m_lock.unlock();

	if ( !started )
	{
		Cpl::System::FatalError::logf( "Cpl::Io::Socket::Listener - Protocol Error.  terminate() called before startListening()" );
	}

	stopListener();
	m_startCalled = false;
}


////////////////////////////////////////
void Listener_::appRun()
{
	m_lock.lock();
	bool started = m_startCalled;
	m_lock.unlock();
	if ( !started )
	{
		Cpl::System::Thread::wait();
	}

	listen();
}


void Listener_::setThreadOfExecution_( Cpl::System::Thread* myThreadPtr )
{
	Cpl::System::Mutex::ScopeBlock lock( m_lock );
	m_myThreadPtr = myThreadPtr;
}


void Listener_::pleaseStop()
{
	terminate();
}



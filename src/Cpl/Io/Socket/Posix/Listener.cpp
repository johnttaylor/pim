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

#include "Listener.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>



///
using namespace Cpl::Io::Socket::Posix;

#define SECT_	"Cpl::Io::Socket::Posix"


///////////////////////////////
Listener::Listener()
	:m_fd( -1 )
{
}

Listener::~Listener()
{
	stopListener();
}


///////////////////////////////
void Listener::stopListener()
{
	if ( m_fd >= 0 )
	{
		::shutdown( m_fd, 2 ); // 2 == SHUT_RDWR
		::close( m_fd );
		m_fd = -1;
	}
}


void Listener::listen()
{
	struct sockaddr_in local;
	struct sockaddr_in client_addr;
	socklen_t  		   client_len;
	int                one = 1;

	// Create the Socket to listen on
	m_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( m_fd < 0 )
	{
		Cpl::System::FatalError::logf( "Cpl::Io::Socket::Posix::Listener: Can not create Stream Socket" );
		return;
	}

	// Set Options on the socket
	if ( setsockopt( m_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof( one ) ) )
	{
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "Cpl::Io::Socket::Posix::Listener: Failed setsockopt SO_REUSEADDR" ) );
	}

	// Set the "address" of the socket		
	int retry = USE_CPL_IO_SOCKET_POSIX_BIND_RETRIES;
	while ( retry )
	{
		memset( &local, 0, sizeof( local ) );
		local.sin_family 		= AF_INET;
		local.sin_addr.s_addr 	= htonl( INADDR_ANY );
		local.sin_port 			= htons( m_socket );
		if ( bind( m_fd, ( struct sockaddr * ) &local, sizeof( local ) ) >= 0 )
		{
			break;
		}

		CPL_SYSTEM_TRACE_MSG( SECT_, ( "Cpl::Io::Socket::Posix::Listener:: Listening on Port=%d. Bind error - retrying...", m_socket ) );
		Cpl::System::Api::sleep( USE_CPL_IO_SOCKET_POSIX_BIND_RETRY_WAIT );
		if ( --retry == 0 )
		{
			Cpl::System::FatalError::logf( "Cpl::Io::Socket::Posix::Listener: Bind error - exceed retry count -->giving up" );
			return;
		}
	}

	// Create a queue to hold connection requests	
	if ( ::listen( m_fd, SOMAXCONN ) < 0 )
	{
		Cpl::System::FatalError::logf( "Cpl::Io::Socket::Posix::Listener: listen() failed" );
		return;
	}


	// Start listening for connections
	for ( ;;)
	{
		// Wait on the 'accept'
		int newfd;
		client_len = sizeof( client_addr );
		if ( ( newfd=accept( m_fd, ( struct sockaddr * )&client_addr, &client_len ) ) < 0 )
		{
			break;
		}

		// Enable SO_KEEPALIVE so we know when the client terminated the TCP session
		int bOptVal = 1;
		int bOptLen = sizeof( int );
		if ( setsockopt( newfd, SOL_SOCKET, SO_KEEPALIVE, (char*) &bOptVal, bOptLen ) < 0 )
		{
			CPL_SYSTEM_TRACE_ALLOCATE( int, sockerr, errno );
			CPL_SYSTEM_TRACE_MSG( SECT_, ( "Cpl::Io::Socket::Posix::Listener:: start() - Failed enable SO_KEEPALIVE. errCode=(%X) %s.", sockerr, strerror( sockerr ) ) );
		}

		// Create a Descriptor for the accepted connection and pass it to the client
		Cpl::Io::Descriptor streamFd( newfd );
		if ( !m_clientPtr->newConnection( streamFd, inet_ntoa( client_addr.sin_addr ) ) )
		{
			::shutdown( newfd, 2 ); // 2==SHUT_RDWR
			::close( newfd );
			continue;
		}
	}


	// Clean-up 
	stopListener();
}

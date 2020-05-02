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

/// Disable able 'deprecated warnings' for use of inet_ntoa() function
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include "Listener.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"

#define SECT_ "Cpl::Io:Socket::Win32"


///
using namespace Cpl::Io::Socket::Win32;



///////////////////////////////
Listener::Listener()
	:m_fd( INVALID_SOCKET )
{
}

Listener::~Listener()
{
}


///////////////////////////////
void Listener::stopListener()
{
	closesocket( m_fd );
}


void Listener::listen()
{
	struct sockaddr_in local;
	struct sockaddr_in client_addr;
	int      		   client_len;


	// Create the Socket to listen on
	m_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( m_fd == INVALID_SOCKET )
	{
		Cpl::System::FatalError::logf( "Cpl::Io::Socket::Win32::Listener: Can not create Stream Socket" );
		return;
	}


	// Set the "address" of the socket		
	int retry = OPTION_CPL_IO_SOCKET_WIN32_BIND_RETRIES;
	while ( retry )
	{
		memset( &local, 0, sizeof( local ) );
		local.sin_family 		= AF_INET;
		local.sin_addr.s_addr 	= htonl( INADDR_ANY );
		local.sin_port 			= htons( m_socket );
		if ( bind( m_fd, ( struct sockaddr * ) &local, sizeof( local ) ) != SOCKET_ERROR )
		{
			break;
		}

		CPL_SYSTEM_TRACE_MSG( SECT_, ( "Cpl::Io::Socket::Win32::Listener:: Listening on Port=%d. Bind error - retrying...", m_socket ) );
		Cpl::System::Api::sleep( OPTION_CPL_IO_SOCKET_WIN32_BIND_RETRY_WAIT );
		if ( --retry == 0 )
		{
			Cpl::System::FatalError::logf( "Cpl::Io::Socket::Win32::Listener: Bind error - exceed retry count -->giving up" );
			return;
		}
	}

	// Create a queue to hold connection requests	
	if ( ::listen( m_fd, SOMAXCONN ) == SOCKET_ERROR )
	{
		Cpl::System::FatalError::logf( "Cpl::Io::Socket::Win32::Listener: listen() failed" );
		return;
	}


	// Start listening for connections
	for ( ;;)
	{
		// Wait on the 'accept'
		SOCKET newfd;
		client_len = sizeof( client_addr );
		if ( ( newfd=accept( m_fd, ( struct sockaddr * )&client_addr, &client_len ) ) == INVALID_SOCKET )
		{
			break;
		}

		// Enable SO_KEEPALIVE so we know when the client terminated the TCP session
		BOOL bOptVal = TRUE;
		int  bOptLen = sizeof( BOOL );
		if ( setsockopt( newfd, SOL_SOCKET, SO_KEEPALIVE, (char*) &bOptVal, bOptLen ) == SOCKET_ERROR )
		{
			CPL_SYSTEM_TRACE_MSG( SECT_, ( "Cpl::Io::Socket::Win32::Listener:: start() - Failed enable SO_KEEPALIVE. errCode=%X.", WSAGetLastError() ) );
		}

		// Create a Descriptor for the accepted connection and pass it to the client
		Cpl::Io::Descriptor streamFd( (void*) newfd );
		if ( !m_clientPtr->newConnection( streamFd, inet_ntoa( client_addr.sin_addr ) ) )
		{
			closesocket( newfd );    // Connection refused
		}
	}


	// Clean-up 
	closesocket( m_fd );
}

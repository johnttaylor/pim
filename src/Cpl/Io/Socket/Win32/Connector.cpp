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


#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include "Connector.h"
#include "Cpl/Text/FString.h"
#include "Cpl/System/FatalError.h"


///
using namespace Cpl::Io::Socket::Win32;



///////////////////////////////
Connector::Connector()
{
}

Connector::~Connector()
{
}


///////////////////////////////
Connector::Result_T Connector::establish( const char* remoteHostName, int portNumToConnectTo, Cpl::Io::Descriptor& fdOut )
{
	SOCKET             sockfd    = INVALID_SOCKET;
	struct addrinfo*   resultPtr = NULL;
	struct addrinfo*   ptr       = NULL;
	struct addrinfo    hints;

	// Default the returned Stream Descriptor to "INVALID"
	fdOut.m_handlePtr = (void*) INVALID_SOCKET;

	// Resolve the server address and port
	Cpl::Text::FString<5> port( portNumToConnectTo );
	ZeroMemory( &hints, sizeof( hints ) );
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if ( getaddrinfo( remoteHostName, port, &hints, &resultPtr ) != 0 )
	{
		return eERROR;
	}

	// Attempt to connect to address until one succeeds
	for ( ptr=resultPtr; ptr != NULL; ptr=ptr->ai_next )
	{
		// Create a SOCKET for connecting to server
		sockfd = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );
		if ( sockfd == INVALID_SOCKET )
		{
			freeaddrinfo(resultPtr);
			return eERROR;
		}

		// Connect to server.
		if ( connect( sockfd, ptr->ai_addr, (int) ptr->ai_addrlen ) == SOCKET_ERROR )
		{
			closesocket( sockfd );
			sockfd = INVALID_SOCKET;
			continue;   // Try the next address
		}

		// If I get here, I successfully connected to the server
		break;
	}

	// Housekeeping
	freeaddrinfo( resultPtr );

	// Check if a connect was made
	if ( sockfd == INVALID_SOCKET )
	{
		return eREFUSED;
	}

	// If I get here, I have successful connection to the remote Host
	fdOut.m_handlePtr = (void*) sockfd;
	return eSUCCESS;
}

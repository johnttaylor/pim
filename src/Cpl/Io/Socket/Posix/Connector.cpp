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


#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include "Connector.h"
#include "Cpl/Text/FString.h"
#include "Cpl/System/FatalError.h"


///
using namespace Cpl::Io::Socket::Posix;



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
	int                sockfd    = -1;
	struct addrinfo*   resultPtr = NULL;
	struct addrinfo*   ptr       = NULL;
	struct addrinfo    hints;

	// Default the returned Stream Descriptor to "INVALID"
	fdOut.m_fd = -1;

	// Resolve the server address and port
	Cpl::Text::FString<5> port( portNumToConnectTo );
	memset( &hints, 0, sizeof( hints ) );
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
		if ( sockfd == -1 )
		{
			return eERROR;
		}

		// Connect to server.
		if ( connect( sockfd, ptr->ai_addr, ptr->ai_addrlen ) == -1 )
		{
			close( sockfd );
			sockfd = -1;
			continue;   // Try the next address
		}

		// If I get here, I successfully connected to the server
		break;
	}

	// Housekeeping
	freeaddrinfo( resultPtr );

	// Check if a connect was made
	if ( sockfd == -1 )
	{
		return eREFUSED;
	}

	// If I get here, I have successful connection to the remote Host
	fdOut.m_fd = sockfd;
	return eSUCCESS;
}

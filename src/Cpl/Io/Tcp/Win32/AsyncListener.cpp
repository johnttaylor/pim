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

#include "AsyncListener.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/ElapsedTime.h"

#define SECT_ "Cpl::Io::Tcp::Win32"

using namespace Cpl::Io::Tcp::Win32;

#define STATE_NOT_STARTED       0
#define STATE_BINDING           1
#define STATE_RETRING_BINDING   2
#define STATE_LISTENING         3
#define STATE_CONNECTED         4


///////////////////////////////////////////////////
AsyncListener::AsyncListener()
    : m_fd( INVALID_SOCKET )
    , m_clientPtr( nullptr )
    , m_portNum( 0 )
    , m_state( STATE_NOT_STARTED )
{
}

AsyncListener::~AsyncListener()
{
    terminate();
}

///////////////////////////////////////////////////
bool AsyncListener::startListening( Client& client,
                                    int     portNumToListenOn ) noexcept
{
    // Once started, must first be terminate() before restarting
    if ( m_state == STATE_NOT_STARTED )
    {
        m_portNum   = portNumToListenOn;
        m_clientPtr = &client;

        // Create the Socket to listen on
        m_fd = socket( AF_INET, SOCK_STREAM, 0 );
        if ( m_fd == INVALID_SOCKET )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Cpl::Io::Tcp::Win32::AsyncListener: Can not create Stream Socket") );
            return false;
        }

        // Start the listening sequence
        m_state           = STATE_BINDING;
        m_clientConnected = false;
        m_retryCounter    = OPTION_CPL_IO_TCP_WIN32_BIND_RETRIES;
        poll();
        return true;
    }

    return false;
}

void AsyncListener::poll() noexcept
{
    if ( m_state == STATE_RETRING_BINDING )
    {
        // The delay has expired
        if ( Cpl::System::ElapsedTime::expiredMilliseconds( m_timeMarker, OPTION_CPL_IO_TCP_WIN32_BIND_RETRY_WAIT ) )
        {
            m_state = STATE_BINDING;
        }
    }

    if ( m_state == STATE_BINDING )
    {
        struct sockaddr_in local;

        // Set the "address" of the socket		
        memset( &local, 0, sizeof( local ) );
        local.sin_family 		= AF_INET;
        local.sin_addr.s_addr 	= htonl( INADDR_ANY );
        local.sin_port 			= htons( m_portNum );
        if ( bind( m_fd, (struct sockaddr *) &local, sizeof( local ) ) != SOCKET_ERROR )
        {
            // Create a queue to hold connection requests
            // NOTE: Set the backlog to 0 per the interface semantics
            if ( ::listen( m_fd, 0 ) == SOCKET_ERROR )
            {
                Cpl::System::FatalError::logf( "Cpl::Io::Tcp::Win32::AsyncListener: listen() failed" );
                return;
            }

            // Set the Listening socket to be non-blocking
            u_long iMode = 1;
            ioctlsocket( m_fd, FIONBIO, &iMode );

            // Advance to the next state
            m_state = STATE_LISTENING;
        }

        // The bind failed -->start retrying
        else
        {
            // Fatal error when retry count is exhausted
            if ( m_retryCounter-- == 0 )
            {
                Cpl::System::FatalError::logf( "Cpl::Io::Tcp::AsyncListener::Listener: Bind error - exceed retry count -->giving up" );
                return;
            }

            CPL_SYSTEM_TRACE_MSG( SECT_, ("Cpl::Io::Tcp::Win32::AsyncListener:: Listening on Port=%d. Bind error - retrying...", m_portNum) );
            m_timeMarker = Cpl::System::ElapsedTime::milliseconds();
            m_state      = STATE_RETRING_BINDING;
        }
    }


    // Start listening for connections
    if ( m_state == STATE_LISTENING )
    {
        // Monitor the current remote connection 
        if ( m_clientConnected && m_clientPtr )
        {
            // NOTE: This only works if the server/application attempts to write data 
            //       to the socket after the remote host has disconnected.  In theory
            //       I could use WSApoll (Windoze equivalent of poll()) to determine
            //       the socket state - but WSApoll is apparently broken - google: 
            //       “Windows 8 Bugs 309411 – WSAPoll does not report failed connections”.
            if ( m_clientPtr->isEos() )
            {
                // Accept connections again
                m_clientPtr->close();
                m_clientConnected = false;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Cpl::Io::Tcp::Win32::AsyncListener:: Stream EOS, accepting new connection ...") );
            }
        }

        // Wait on the 'accept'
        SOCKET             newfd;
        struct sockaddr_in client_addr;
        int                client_len = sizeof( client_addr );
        if ( (newfd=accept( m_fd, (struct sockaddr *) &client_addr, &client_len )) == INVALID_SOCKET )
        {
            // Most likely the error is because there is No incoming connection,
            // so we will try again later.  If it something else - we will still
            // again later in-case it is something that is 'recoverable'
            return;
        }

        // Enable SO_KEEPALIVE so we know when the client terminated the TCP session
        BOOL bOptVal = TRUE;
        int  bOptLen = sizeof( BOOL );
        if ( setsockopt( newfd, SOL_SOCKET, SO_KEEPALIVE, (char*) &bOptVal, bOptLen ) == SOCKET_ERROR )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Cpl::Io::Tcp::Win32::AsyncListener::setsockopt() - Failed enable SO_KEEPALIVE. errCode=%X.", WSAGetLastError()) );
        }

        // Set the new client socket to non-blocking
        u_long iMode = 1;
        ioctlsocket( newfd, FIONBIO, &iMode );

        // Create a Descriptor for the accepted connection and pass it to the client
        Cpl::Io::Descriptor streamFd( (void*) newfd );
        if ( m_clientConnected || !m_clientPtr->newConnection( streamFd, inet_ntoa( client_addr.sin_addr ) ) )
        {
            closesocket( newfd );       // Connection refused
        }
        else
        {
            m_clientConnected = true;   // Connection accepted
        }

    }
}

void AsyncListener::terminate() noexcept
{
    if ( m_fd != INVALID_SOCKET )
    {
        closesocket( m_fd );
        m_fd = INVALID_SOCKET;
    }
    if ( m_clientConnected )
    {
        if ( m_clientPtr )
        {
            m_clientPtr->close();
        }
        m_clientConnected = false;
    }
    m_state     = STATE_NOT_STARTED;
    m_clientPtr = nullptr;
}




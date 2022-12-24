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

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include "AsyncConnector.h"
#include "Cpl/Text/FString.h"
#include "Cpl/System/Trace.h"


using namespace Cpl::Io::Tcp::Win32;

#define SECT_ "Cpl::Io::Tcp::Win32"

#define STATE_IDLE					0
#define STATE_CONNECTED				1
#define STATE_PENDING_CONNECTION	2



/////////////////////////////////
AsyncConnector::AsyncConnector()
    : m_fd( INVALID_SOCKET )
    , m_clientPtr( nullptr )
    , m_state( STATE_IDLE )
{
}

AsyncConnector::~AsyncConnector()
{
    terminate();
}

void AsyncConnector::notifyConnected( SOCKET sock )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Cpl::Io::Tcp::Win32::AsyncConnector: - Connected") );
    Cpl::Io::Descriptor newfd( sock );
    if ( m_clientPtr->newConnection( newfd ) )
    {
        m_clientConnected = true;
        m_state           = STATE_CONNECTED;
    }
    
    // Connection refused
    else
    {
        terminate();   // Note: Updates my internal state to: STATE_IDLE
    }
}

void AsyncConnector::notifyError( Client::Error_T error, int wsaLastError )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Cpl::Io::Tcp::Win32::AsyncConnector: - Connection failed: %d (wsaCode=%X).", error, wsaLastError) );
    m_clientPtr->connectionFailed( error );
    terminate();                // Note: Updates my internal state to: STATE_IDLE
}

///////////////////////////////////////////////////
bool AsyncConnector::establish( Client&     client,
                                const char* remoteHostName,
                                int         portNumToConnectTo )
{
    // Ignore if connected or connection in progress
    if ( m_state == STATE_IDLE )
    {
        // Cache the client reference
        m_clientPtr       = &client;
        m_clientConnected = false;

        // Resolve the server address and port
        struct addrinfo hints;

        Cpl::Text::FString<5> port( portNumToConnectTo );
        ZeroMemory( &hints, sizeof( hints ) );
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        int err = getaddrinfo( remoteHostName, port, &hints, &m_remoteAddrPtr );
        if ( err != 0 )
        {
            notifyError( Client::eERROR, err );                 // Note: Sets my state to IDLE after cleaning-up
            return false;
        }

        // Create a SOCKET for connecting to server
        m_fd = socket( m_remoteAddrPtr->ai_family, m_remoteAddrPtr->ai_socktype, m_remoteAddrPtr->ai_protocol );
        if ( m_fd == INVALID_SOCKET )
        {
            int lastError = WSAGetLastError();
            freeaddrinfo( m_remoteAddrPtr );
            notifyError( Client::eERROR, lastError );   // Note: Sets my state to IDLE after cleaning-up
            return false;
        }

        // Set the new socket to non-blocking
        u_long iMode = 1;
        ioctlsocket( m_fd, FIONBIO, &iMode );

        // Begin the connection request
        m_connectCalled = false;
        m_state         = STATE_PENDING_CONNECTION;
        poll();
        return true;
    }

    // If I get here I am not in the state to state a new connection or an error occurred
    return false;
}


void AsyncConnector::poll() noexcept
{
    // Waiting for connection request to succeed
    if ( m_state == STATE_PENDING_CONNECTION )
    {
        // Make the initial call to connect()
        if ( !m_connectCalled )
        {
            m_connectCalled = true;

            // Start the connection request 
            int err     = connect( m_fd, m_remoteAddrPtr->ai_addr, (int) m_remoteAddrPtr->ai_addrlen );
            int lastErr = WSAGetLastError();
            if ( !err )
            {
                notifyConnected( m_fd );	// Note: Updates my internal state to: STATE_CONNECTED
            }

            // This is the expect path for connecting with a non-blocking socket for the call to connect
            else if ( err == SOCKET_ERROR && lastErr == WSAEWOULDBLOCK )
            {
                // Nothing required -->use select() later to poll the state of the request
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Cpl::Io::Tcp::Win32::AsyncConnector.connect(): - waiting on request") );
            }

            // Try the next possible address for the remote host
            else
            {
                nextAddress( lastErr );     // Note: If there is no 'next' address -->will clean up and set the state to: STATE_IDLE
            }
        }

        // Use select() to monitor the state of the connection request
        else
        {
            // Poll the state of request
            timeval noTimeout ={ 0, };
            fd_set  writeFds;
            FD_ZERO( &writeFds );
            FD_SET( m_fd, &writeFds );
            int nfds    = select( 1, 0, &writeFds, 0, &noTimeout );
            int lastErr = WSAGetLastError();
            if ( nfds == SOCKET_ERROR )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Cpl::Io::Tcp::Win32::AsyncConnector: - select failed: wsaCode=%X.", lastErr) );
                nextAddress( lastErr );     // Note: If there is no 'next' address -->will clean up and set the state to: STATE_IDLE
            }
            else if ( nfds == 1 )
            {
                notifyConnected( m_fd );	// Note: Updates my internal state to: STATE_CONNECTED
            }
        }
    }
}

void AsyncConnector::nextAddress( int wsaLastError )
{
    // TODO: The non-blocking behavior of the whole 'next address' needs to be TESTED more!!!!

    // Clean-up
    closesocket( m_fd );
    m_fd = INVALID_SOCKET;

    // Try the 'next' remote address if there is one
    m_remoteAddrPtr = m_remoteAddrPtr->ai_next;
    if ( m_remoteAddrPtr != 0 )
    {
        m_connectCalled = false;
    }

    // No more addresses -->fail the request
    else
    {
        notifyError( Client::eREFUSED, wsaLastError );   // Note: Sets my state to IDLE after cleaning-up
    }
}

void AsyncConnector::terminate() noexcept
{
    if ( m_fd != INVALID_SOCKET )
    {
        // If I am connected -->use the client reference to close the socket
        if ( m_clientConnected && m_clientPtr )
        {
            m_clientPtr->close();
        }
        
        // Connection in-progress -->close the socket directly
        else
        {
            closesocket( m_fd );
        }
        m_fd = INVALID_SOCKET;
    }

    m_state           = STATE_IDLE;
    m_clientPtr       = nullptr;
    m_clientConnected = false;
}
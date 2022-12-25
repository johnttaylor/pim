#ifndef Cpl_Io_Tcp_Win32_AsyncConnector_h_
#define Cpl_Io_Tcp_Win32_AsyncConnector_h_
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
#include "Cpl/Io/Tcp/AsyncConnector.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Tcp {
///
namespace Win32 {


/** This class implements the Asynchronous Connector.
 */
class AsyncConnector : public Cpl::Io::Tcp::AsyncConnector
{
public:
    /// Constructor
    AsyncConnector();

    /// Destructor
    ~AsyncConnector();

public:
    /// See Cpl::Io::Tcp::AsyncConnector
    bool establish( Client&     client,
                    const char* remoteHostName,
                    int         portNumToConnectTo );

    /// See Cpl::Io::Tcp::AsyncConnector
    void poll() noexcept;

    /// See Cpl::Io::Tcp::AsyncConnector
    void terminate() noexcept;

protected:
    /// Helper method that is used to notify the client that the connection has been established
    void notifyConnected( SOCKET sock );

    /// Helper method that is used to notify the client that the connection request failed
    void notifyError( Client::Error_T error, int wsaLastError );

    /// Helper method to try the 'next' address for the remote host
    void nextAddress( int wsaLastError );

protected:
    /// socket for the connection
    SOCKET              m_fd;

    /// Client
    Client*             m_clientPtr;

    /// Current address to try
    struct addrinfo*    m_remoteAddrPtr;

    /// Connecting state
    int                 m_state;

    /// Track the 1st call to connect();
    bool                m_connectCalled;

    /// Track if the client is connected
    bool                m_clientConnected;
};


};      // end namespaces
};
};
};
#endif  // end header latch

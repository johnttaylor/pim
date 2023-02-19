#ifndef Cpl_Io_Tcp_lwIP_Pico2_Connector_h_
#define Cpl_Io_Tcp_lwIP_Pico2_Connector_h_
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

#include "Cpl/Io/Tcp/AsyncConnector.h"
#include "Cpl/Io/Tcp/lwIP/Picow/Private_.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Tcp {
///
namespace lwIP {
///
namespace Picow {


/** This class implements the Asynchronous Connector.

    The implementation IS thread safe WHEN building with PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1

*/
class AsyncConnector: public Cpl::Io::Tcp::AsyncConnector

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
    /** Callback method when connection.  This method can/will-be called from
        an ISR context
     */
    static err_t lwIPCb_connected( void* arg, struct tcp_pcb* newpcb, err_t err );

protected:
    /// Remote Host address
    ip_addr_t       m_remoteAddr;

    /// Client
    Client*         m_clientPtr;

    /// Socket instance for the Stream.  Note: At any given time there is at most only one stream 'active' from a Connector instance
    Socket_T        m_connectionFd;
};


};      // end namespaces
};
};
};
};
#endif  // end header latch

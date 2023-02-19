#ifndef Cpl_Io_Tcp_lwIP_Picow_AsyncListener_h_
#define Cpl_Io_Tcp_lwIP_Picow_AsyncListener_h_
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

#include "Cpl/Io/Tcp/AsyncListener.h"
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


/** This class implements the Asynchronous Listener.

    The implementation IS thread safe WHEN building with PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1
 */
class AsyncListener : public Cpl::Io::Tcp::AsyncListener

{
public:
    /// Constructor
    AsyncListener();

    /// Destructor
    ~AsyncListener();

public:
    /// Cpl::Io::Tcp::AsyncListener
    bool startListening( Client& client,
                         int     portNumToListenOn ) noexcept;

    /// Cpl::Io::Tcp::AsyncListener
    void terminate() noexcept;

public:
    /** This method must be called periodically to service the listen/connection
        status
     */
    void poll() noexcept;

protected:
    /** Callback method to accept a connection.  This method can/will-be called from
        an ISR context
     */
    static err_t lwIPCb_accept( void* arg, struct tcp_pcb* newpcb, err_t err );

protected:
    /// connection I am listen on
    struct tcp_pcb* m_listenerPcb;

    /// Client
    Client*         m_clientPtr;

    /// Socket instance for the Stream.  Note: At any given time there is at most only one stream 'active' from a Listener instance
    Socket_T        m_connectionFd;
};


};      // end namespaces
};
};
};
};
#endif  // end header latch

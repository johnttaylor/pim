#ifndef Cpl_Io_Tcp_Win32_AsyncListener_h_
#define Cpl_Io_Tcp_Win32_AsyncListener_h_
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
#include "colony_config.h"
#include "Cpl/Io/Tcp/AsyncListener.h"

/** This value is number of retries that is performed when attempting
    to bind to the listening port.
 */
#ifndef OPTION_CPL_IO_TCP_WIN32_BIND_RETRIES
#define OPTION_CPL_IO_TCP_WIN32_BIND_RETRIES        5
#endif

 /** This value is time, in milliseconds between retries during the binding
     process.
  */
#ifndef OPTION_CPL_IO_TCP_WIN32_BIND_RETRY_WAIT
#define OPTION_CPL_IO_TCP_WIN32_BIND_RETRY_WAIT     (10*1000)
#endif

///
namespace Cpl {
///
namespace Io {
///
namespace Tcp {
///
namespace Win32 {


/** This class implements the Asynchronous Listener.  
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
    /// socket I am listen on
    SOCKET          m_fd;

    /// Client
    Client*         m_clientPtr;

    /// Time marker
    unsigned long   m_timeMarker;

    /// Port Number to listen on
    int             m_portNum;

    /// Listening state
    int             m_state;

    /// Retry counter
    unsigned        m_retryCounter;

    /// Track if the client is connected
    bool            m_clientConnected;
};


};      // end namespaces
};
};
};
#endif  // end header latch

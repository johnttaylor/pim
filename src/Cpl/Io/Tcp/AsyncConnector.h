#ifndef Cpl_Io_Tcp_Async_Connector_h_
#define Cpl_Io_Tcp_Async_Connector_h_
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

#include "Cpl/Io/Tcp/InputOutput.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Tcp {


/** This abstract class defines the interface for establishing/requesting
    a SIMPLE socket connection, i.e. make a "client connection".   When the
    connection is accepted by the remote host, the connector notifies the client. 
    
    In keeping with the non-blocking semantics a poll() function has been
    defined that must be periodically called.

    The Connector allows only one connection at a time.  After a connection has
    been established, any calls to establish() will be ignore.  Once the existing/
    previous connection is closed, the application can establish() again.
 
    The interface is NOT thread safe. This includes the client callback functions, 
    i.e. no guarantees on what thread the callback functions executes in.
 */
class AsyncConnector
{
public:
    /** This class defines the callback mechanism used for accepting incoming
        TCP connections.
     */
    class Client: public Cpl::Io::Tcp::InputOutput
    {
    public:
        /// Possible error codes when attempting to establish a connection
        enum Error_T {
            eERROR,         //!< Error occurred
            eREFUSED,       //!< Connection request was refused by the remote Host
        };

    public:
        /** This method is a callback method that is called when the remote host
            has accepted in connection request. It is up the client to
            determine if the application will accept or reject the TCP 
            connection.  If the client rejects the connection, it needs to 
            return false, else returns true.

            When the client accepts the connection, it is required to call
            its Cpl::Io::Tcp::InputOutput.activate() method with 'newFd'

            Note: Caution: This method can be called from an ISR context, it
                  all depends on the platform implementation
         */
        virtual bool newConnection( Cpl::Io::Descriptor newFd ) noexcept = 0;

        /** This method is a callback method that is called when an error occurred
            when making the connection request.

            Note: Caution: This method can be called from an ISR context, it
                  all depends on the platform implementation
         */
        virtual void connectionFailed( Error_T errorCode ) noexcept = 0;
    };

public:
    /** Requests a client connection to the specified remote Host.  The method
        returns true if the connection process is started. The method returns
        false if an error occurred or there is a connection request in progress
        or there is active connection.
     */
    virtual bool establish( Client&     client, 
                            const char* remoteHostName, 
                            int         portNumToConnectTo ) = 0;

    /** This method must be called periodically to service the connection
        status
    */
    virtual void poll() noexcept = 0;

    /** Aborts any connection in progress and/or will CLOSE the active 
        connection.  A new connection can be requested by calling establish().
     */
    virtual void terminate() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~AsyncConnector() {}
};


};      // end namespaces
};
};
#endif  // end header latch

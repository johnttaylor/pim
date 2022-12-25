#ifndef Cpl_Io_Tcp_AsyncListener_h_
#define Cpl_Io_Tcp_AsyncListener_h_
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


/** This abstract class defines the interface for a SIMPLE socket listener. A
    socket listener 'listens' for potential TCP/IP socket connections.  When a
    request for a connection comes in, the listener notifies the client.  
    
    In keeping with the non-blocking semantics a poll() function has been
    defined that must be periodically called.

    The listener accepts only one connection at a time.  After a connection has
    been established, the listener will accept a new connection once the existing/
    previous connection is closed.

    The interface is NOT thread safe. This includes the client callback function, 
    i.e. no guarantees on what thread the callback function executes in.
 */
class AsyncListener 

{
public:
    /** This class defines the callback mechanism used for accepting incoming
        TCP connections.
     */
    class Client: public Cpl::Io::Tcp::InputOutput
    {
    public:
        /** This method is a callback method that is called when the listener
            has accepted in incoming socket request.  It is up the client to
            determine if the application will accept or reject the TCP 
            connection.  If the client rejects the connection, it needs to 
            return false, else returns true.

            When the client accepts the connection, it is required to call
            its Cpl::Io::Tcp::InputOutput.activate() method with 'newFd'

            Note: Caution: This method can be called from an ISR context, it
                  all depends on the platform implementation
         */
        virtual bool newConnection( Cpl::Io::Descriptor newFd, const char* rawConnectionInfo ) noexcept = 0;


    public:
        /// Virtual destructor
        virtual ~Client() {}
    };



public:
    /** Starts the Listener listening.  If an error occurred then false is
        is returned; else true is returned.
     */
    virtual bool startListening( Client& client, 
                                 int     portNumToListenOn ) noexcept = 0;

    /** This method must be called periodically to service the listen/connection
        status
    */
    virtual void poll() noexcept = 0;

    /** Shuts down the Listener and/or will CLOSE the active connection. The
        listener can be restarted by calling startListening()
     */
    virtual void terminate() noexcept = 0;


public:
    /// Virtual destructor
    virtual ~AsyncListener() {}
};


};      // end namespaces
};
};
#endif  // end header latch

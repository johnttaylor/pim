#ifndef Cpl_Itc_PostApi_h_
#define Cpl_Itc_PostApi_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Itc/Message.h"


///
namespace Cpl {
///
namespace Itc {



/** This abstract class represents the interface used to send
    messages to a mailbox. In a client-server relationship
    this interface is always invoked by the client.
 */

class PostApi
{
public:
    /** This operation is called by clients, which wish to send a message
        to the owner of this mailbox. The message is threaded into the
        mailbox queue for the mailbox owner to receive, and the operation
        returns immediately. The client relinquishes ownership of the memory
        until the message's returnToSender() function is invoked.
     */
    virtual void post( Message& msg ) noexcept=0;

    /** This operation is called by clients which wish to send a message
        to the owner of this mailbox, and then block on the client's thread
        semaphore. First, the message is threaded into the mailbox queue
        for the mailbox owner to receive. Next, the client waits on its
        thread semaphore until the semaphore is signaled.
     */
    virtual void postSync( Message& msg ) noexcept=0;


public:
    /// Virtual destructor
    virtual ~PostApi() {}
};

};      // end namespaces
};
#endif  // end header latch


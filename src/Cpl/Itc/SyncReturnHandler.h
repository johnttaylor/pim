#ifndef Cpl_Itc_SyncReturnHandler_h_
#define Cpl_Itc_SyncReturnHandler_h_
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


#include "Cpl/Itc/ReturnHandler.h"
#include "Cpl/System/Signable.h"

///
namespace Cpl {
///
namespace Itc {

/** This class implements a synchronous ReturnHandler. The assumption is that
    the client sends a message to a server mailbox, and then blocks on the
    thread's semaphore. When the server thread invokes the rts() method, the
    thread/semaphore is signaled, and thus the client is notified that it has
    ownership of the message and that the server has completed processing the
    message.

    The expected typical usage is to created a SyncReturnHandler on the stack
    in the client's thread just before the mailbox.postSync() method is called.
 */
class SyncReturnHandler : public ReturnHandler
{
private:
    /** Reference to the client thread's thread semaphore which is used
        to notify the client that the server has completed processing
        the message.
     */
    Cpl::System::Signable& m_waiter;

public:
    /** The constructor initializes the handler with a reference to the
        thread semaphore of the current thread.
     */
    SyncReturnHandler();

public:
    /// See ReturnHandler
    void rts() noexcept;
};


};      // end namespaces
};
#endif  // end header latch

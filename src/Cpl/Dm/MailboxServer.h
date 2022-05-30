#ifndef Cpl_Dm_MailboxServer_h_
#define Cpl_Dm_MailboxServer_h_
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

#include "Cpl/Dm/EventLoop.h"
#include "Cpl/Itc/Mailbox.h"


///
namespace Cpl {
///
namespace Dm {

/** This class extends the Cpl::Dm::EventLoop and Cpl::Itc:Mailbox classes
    to support the asynchronous change notification generated from Model Points
    with ITC messaging.

    The order of processing is:

       1. Event Flags are processed.  Events are processed in LSb order.
       2. The timers and their callbacks (if any timers have expired) are
          processed.
       3. A single Model Point Change notification (if there is one pending) is
          processed.
       4. A single ITC message (it there is on pending) is dispatched.
       5. The loop is repeated until there are no expired timers, no event
          flags, no MP change notifications, and no ITC messages - at which 
          point the thread blocks and wait for any of the above asynchronous 
          actions to wake up the thread.

 */
class MailboxServer : public Cpl::Dm::EventLoop, public Cpl::Itc::Mailbox
{
public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Cpl::Timer::Local Timers.
     */
    MailboxServer( unsigned long                       timingTickInMsec = OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
                   Cpl::System::SharedEventHandlerApi* eventHandler     = 0 ) noexcept;

public:
    /// See Cpl::System::Runnable
    void appRun();
};

};      // end namespaces
};
#endif  // end header latch
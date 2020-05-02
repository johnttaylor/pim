#ifndef Cpl_Dm_EventLoop_h_
#define Cpl_Dm_EventLoop_h_
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

#include "Cpl/System/EventLoop.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Container/DList.h"
#include "Cpl/Dm/SubscriberApi.h"
#include "Cpl/Dm/NotificationApi_.h"


///
namespace Cpl {
///
namespace Dm {

/** This class extends the Cpl::System::EventLoop class to support the
    asynchronous change notification generated from Model Points.

    The order of processing is:

       1. Event Flags are processed.  Events are processed in LSb order.
       2. The timers and their callbacks (if any timers have expired) are
          processed.
       3. A single Model Point Change notification (if there is one pending) is
          processed.
       4. The loop is repeated until there are no expired timers, no event
          flags, and no MP change notifications - at which point the thread 
          blocks and wait for any of the above asynchronous actions to wake up 
          the thread.

 */
class EventLoop : public Cpl::System::EventLoop, public NotificationApi_
{
protected:
    /// List of pending Model Point Change Notifications
    Cpl::Container::DList<SubscriberApi>   m_pendingMpNotifications;


public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Cpl::Timer::Local Timers.

        NOTE: A value of zero for 'timingTickInMsec' will disable support
              for Local Timers.
     */
    EventLoop( unsigned long timingTickInMsec = OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD ) noexcept;

public:
    /// See Cpl::System::Runnable
    void appRun();


public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is used add a new 'change notification' to its list
        of pending change notifications.  Calling this method when the
        subscriber is already registered for change notification will cause
        a FATAL ERROR.

        This method IS thread safe.

        NOTE: The requirements and/or semantics of Model Point subscription is
              that Subscriptions, Notifications, and Cancel-of-Subscriptions
              all happen in a SINGLE thread and that thread is the 'Subscribers'
              thread.
     */
    void addPendingChangingNotification_( SubscriberApi& subscriber ) noexcept;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
    by other classes in the Cpl::Dm namespace.  The Application should
    NEVER call this method.

    This method is used remove a pending 'change notification' from its list
    of pending change notifications.  It is okay to call this method even if
    the Subscriber is not current registered for change notifications.

     This method IS thread safe.

     NOTE: The requirements and/or semantics of Model Point subscription is
           that Subscriptions, Notifications, and Cancel-of-Subscriptions
           all happen in a SINGLE thread and that thread is the 'Subscribers'
           thread.
     */
    void removePendingChangingNotification_( SubscriberApi& subscriber ) noexcept;

protected:
    /// This helper method processes pending change notifications
    virtual void processChangeNotifications() noexcept;
};

};      // end namespaces
};
#endif  // end header latch
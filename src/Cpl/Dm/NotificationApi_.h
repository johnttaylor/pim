#ifndef Cpl_Dm_NotificationApi_h_
#define Cpl_Dm_NotificationApi_h_
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

#include "Cpl/Dm/SubscriberApi.h"


///
namespace Cpl {
///
namespace Dm {

/** This class has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

    This class defines the interface to register for Model Point Change notifications.
 */
class NotificationApi_ 
{
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
    virtual void addPendingChangingNotification_( SubscriberApi& subscriber ) noexcept = 0;
    
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
    virtual void removePendingChangingNotification_( SubscriberApi& subscriber ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~NotificationApi_() {}
};

};      // end namespaces
};
#endif  // end header latch
#ifndef Cpl_Dm_SubscriberApi_h_
#define Cpl_Dm_SubscriberApi_h_
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

#include "Cpl/Container/Item.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Dm {

/// Forward reference to a Model point -->used to avoid circular dependencies
class ModelPoint;

/// Forward reference to the Data Model Mailbox server -->used to avoid circular dependencies
class NotificationApi_;


/** This abstract class defines the Subscriber interface - for change
    notifications - to a Model Points data/state
 */
class SubscriberApi : public Cpl::Container::ExtendedItem
{
public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is the client's callback function for a MP change
        notification.  This method is called in as part of the asynchronous
        notification mechanism of the subscriber's Mailbox Server, i.e. executes 
        in the thread associated m_mailBoxHdl
     */
    virtual void genericModelPointChanged_( ModelPoint& modelPointThatChanged ) noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method returns a pointer to the Subscriber's mailbox
      */
    virtual NotificationApi_* getNotificationApi_() const noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is use to set the Subscriber's Model Point reference
      */
    virtual void setModelPoint_( ModelPoint* modelPoint ) noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is use to get the Subscriber's Model Point reference.

        Note: If this method is called BEFORE the setModelPoint() method is
              called then a Fatal Error will be generated.
      */
    virtual ModelPoint* getModelPoint_() noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is use to get the Subscriber's internal state
      */
    virtual int getState_() const noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is use to set the Subscriber's internal state
      */
    virtual void setState_( int newState ) noexcept = 0;

   /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
       by other classes in the Cpl::Dm namespace.  The Application should
       NEVER call this method.

       This method is use to get the Subscriber's sequence number
     */
    virtual uint16_t getSequenceNumber_() const noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is use to set the Subscriber's sequence number
      */
    virtual void setSequenceNumber_( uint16_t newSeqNumber ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~SubscriberApi() {}
};



};      // end namespaces
};
#endif  // end header latch

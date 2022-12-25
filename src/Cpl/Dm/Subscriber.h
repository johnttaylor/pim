#ifndef Cpl_Dm_Subscriber_h_
#define Cpl_Dm_Subscriber_h_
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


#include "Cpl/Dm/SubscriberApi.h"
#include "Cpl/Dm/ModelPoint.h"
#include "Cpl/Dm/EventLoop.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Dm {



/** This mostly concrete class defines the Subscriber interface - for change
    notifications - to a Model Points data/state
 */
class SubscriberBase : public SubscriberApi
{
protected:
    /// Internal state of the subscriber.  Note: The state is actual managed by the Model Point
    int                             m_state;

    /// Pointer to the Model Point the instance is subscribed to
    ModelPoint*                     m_point;

    /// Reference to subscriber's EventFlag/Mailbox server
    Cpl::Dm::EventLoop&             m_eventLoopHdl;

    /// Sequence number of the subscriber
    uint16_t                        m_seqNumber;

public:
    /// Constructor
    SubscriberBase( Cpl::Dm::EventLoop& myEventLoop );

public:
    /// See Cpl::Dm::SubscriberApi
    NotificationApi_* getNotificationApi_() const noexcept;

    /// See Cpl::Dm::SubscriberApi
    void setModelPoint_( ModelPoint* modelPoint ) noexcept;

    /// See Cpl::Dm::SubscriberApi
    ModelPoint* getModelPoint_() noexcept;

    /// See Cpl::Dm::SubscriberApi
    int getState_() const noexcept;

    /// See Cpl::Dm::SubscriberApi
    void setState_( int newState ) noexcept;

    /// See Cpl::Dm::SubscriberApi
    uint16_t getSequenceNumber_() const noexcept;

    /// See Cpl::Dm::SubscriberApi
    void setSequenceNumber_( uint16_t newSeqNumber ) noexcept;
};

/////////////////////////////////////////////////////////////////////////////

/** This template class defines a type safe Subscriber

    Template Arguments:
        MP      - The concrete Model Point Type
 */
template <class MP>
class Subscriber : public SubscriberBase
{
public:
    /// Type safe change notification.  See Cpl::Dm::SubscriberApi
    virtual void modelPointChanged( MP& modelPointThatChanged, SubscriberApi& clientObserver ) noexcept = 0;

public:
    /// Constructor
    Subscriber( Cpl::Dm::EventLoop& myEventLoop ):SubscriberBase(myEventLoop) {}

protected:
    /// See Cpl::Dm::SubscriberApi
    void genericModelPointChanged_( ModelPoint& modelPointThatChanged, SubscriberApi& clientObserver ) noexcept { modelPointChanged( *((MP*) &modelPointThatChanged), clientObserver ); }
};



};      // end namespaces
};
#endif  // end header latch

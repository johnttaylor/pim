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

#include "EventLoop.h"
#include "SubscriberApi.h"
#include "ModelPoint.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/GlobalLock.h"


#define SECT_ "Cpl::Dm"


///
using namespace Cpl::Dm;

/////////////////////
EventLoop::EventLoop( unsigned long                       timingTickInMsec,
                      Cpl::System::SharedEventHandlerApi* eventHandler ) noexcept
    :Cpl::System::EventLoop( timingTickInMsec, eventHandler )
{
}


/////////////////////
void EventLoop::appRun()
{
    startEventLoop();
    bool run = true;
    while ( run )
    {
        run = waitAndProcessEvents();
        if ( run )
        {
            processChangeNotifications();
        }
    }
    stopEventLoop();
}

void EventLoop::processChangeNotifications() noexcept
{
    // Get the next pending change notification
    Cpl::System::GlobalLock::begin();
    SubscriberApi* subscriberPtr = m_pendingMpNotifications.get();
    Cpl::System::GlobalLock::end();

    // Execute - at MOST one - the change notification callback
    if ( subscriberPtr )
    {
        // Get the model point that changed
        ModelPoint* mpPtr = subscriberPtr->getModelPoint_();
        CPL_SYSTEM_ASSERT( mpPtr != 0 );   // NOTE: getModelPoint_() is guaranteed to return a valid pointer, but just in case...
        ModelPoint& modelPoint = *mpPtr;

        // Update the subscriber's state
        modelPoint.processSubscriptionEvent_( *subscriberPtr, ModelPoint::eNOTIFYING );

        // Execute the callback
        subscriberPtr->genericModelPointChanged_( modelPoint );

        // Update the subscriber's state
        modelPoint.processSubscriptionEvent_( *subscriberPtr, ModelPoint::eNOTIFY_COMPLETE );
    }
}

void EventLoop::addPendingChangingNotification_( SubscriberApi& subscriber ) noexcept
{
    // Add the notification to my list and send myself an Event to wake up the mailbox
    Cpl::System::GlobalLock::begin();
    m_pendingMpNotifications.put( subscriber );
    Cpl::System::GlobalLock::end();
    signal();
}

void EventLoop::removePendingChangingNotification_( SubscriberApi& subscriber ) noexcept
{
    // Remove the subscriber from the notification
    Cpl::System::GlobalLock::begin();
    m_pendingMpNotifications.remove( subscriber );
    Cpl::System::GlobalLock::end();
}


#ifndef Cpl_Dm_SubscriberComposer_h_
#define Cpl_Dm_SubscriberComposer_h_
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

#include "Cpl/Dm/Subscriber.h"

///
namespace Cpl {
///
namespace Dm {


/** This template class is a composer pattern/class that manages the callback
    function for a Model Point's Subscribers/Observers change notification

    A Composer is a structural pattern that may be used to employ composition
    when implementing an interface rather than using multiple inheritance. This
    allows a single concrete object to receive the change notifications callbacks 
    from many Model Points.

    Template Arguments:
        CONTEXT - The class that implements the Callback function
        MP       - The concrete Model Point type.
 */
template <class CONTEXT, class MP>
class SubscriberComposer : public Subscriber<MP>
{
public:
    /** Define a callback method function for the Change Notification callback (See
        Cpl::Dm::Subscriber<MP>::modelPointChanged for additional details)
     */
    typedef void (CONTEXT::*NotificationFunc_T)(MP& modelPointThatChanged, SubscriberApi& clientObserver );


protected:
    /// Class the implement the callback
    CONTEXT&                    m_context;

    /// Method (in my Context) to call for the change notification
    NotificationFunc_T          m_notificationCb;


public:
    /// Constructor
    SubscriberComposer( Cpl::Dm::EventLoop&     myEventLoop,
                        CONTEXT&                context,
                        NotificationFunc_T      notifyCallback );


public:
    /// See Cpl::Dm::Subscriber<MP>
    void modelPointChanged( MP& modelPointThatChanged, SubscriberApi &clientObserver ) noexcept;

};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template <class CONTEXT, class MP>
Cpl::Dm::SubscriberComposer<CONTEXT, MP>::SubscriberComposer( Cpl::Dm::EventLoop&     myEventLoop,
                                                              CONTEXT&                context,
                                                              NotificationFunc_T      notifyCallback )
    : Subscriber<MP>( myEventLoop )
    , m_context( context )
    , m_notificationCb( notifyCallback )
{
}

/////////////////
template <class CONTEXT, class MP>
void Cpl::Dm::SubscriberComposer<CONTEXT, MP>::modelPointChanged( MP& modelPointThatChanged, SubscriberApi& clientObserver ) noexcept
{
    // Notify context
    return (m_context.*m_notificationCb)(modelPointThatChanged, clientObserver);
}


};      // end namespaces
};
#endif  // end header latch

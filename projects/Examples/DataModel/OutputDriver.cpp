/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "OutputDriver.h"
#include "Cpl/System/Trace.h"

#define SECT_   "OutputDriver"

///////////////////////////////
OutputDriver::OutputDriver( Cpl::Dm::MailboxServer& myMbox,
                            MpAlarm&                mpHighAlarm,
                            MpAlarm&                mpLowAlarm )
    : Cpl::Itc::CloseSync( myMbox )
    , m_mpHiAlarm( mpHighAlarm )
    , m_mpLoAlarm( mpLowAlarm )
    , m_observerHiAlarmMp( myMbox, *this, &OutputDriver::alarmMpChanged )
    , m_observerLoAlarmMp( myMbox, *this, &OutputDriver::alarmMpChanged )
    , m_opened( false )
{
}

///////////////////////////////

// Note: This method runs in the 'Driver' thread
void OutputDriver::request( Cpl::Itc::OpenRequest::OpenMsg& msg )
{
    if ( !m_opened )
    {
        // Housekeeping
        m_opened = true;

        // Register for a change notification.  Note: An 'immediate' callback
        // will occur to sync the driver with the MP's values.
        // NOTE: The registration and the callback execute in the SAME thread
        m_mpHiAlarm.attach( m_observerHiAlarmMp );
        m_mpLoAlarm.attach( m_observerLoAlarmMp );
    }

    // The request method is an ITC call -->the server is required to 'return the message' to the client
    msg.returnToSender();
}

// Note: This method runs in the 'Driver' thread
void OutputDriver::request( Cpl::Itc::CloseRequest::CloseMsg& msg )
{
    if ( m_opened )
    {
        m_opened = false;

        // Unregister for the change notifications. 
        // NOTE: The un-registration MUST occur in the SAME thread as the registration
        m_mpHiAlarm.detach( m_observerHiAlarmMp );
        m_mpLoAlarm.detach( m_observerLoAlarmMp );
    }

    // The request method is an ITC call -->the server is required to 'return the message' to the client
    msg.returnToSender();
}

///////////////////////////////

// Note: This method runs in the 'Driver' thread
void OutputDriver::alarmMpChanged( MpAlarm& modelPointThatChanged, Cpl::Dm::SubscriberApi& clientObserver ) noexcept
{
    // The 'readnAndSync()' method is used to ensure that my observer is 
    // properly synchronized with the model point AT THE TIME I read the MP's 
    // value.
    Alarm_T alarm;
    bool mpIsValid = modelPointThatChanged.readAndSync( alarm, clientObserver );

    // The alarm is asserted if the MP is valid
    if ( mpIsValid )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("%s RASIED. Signal=%lu, Acknowledged=%s",
                                       modelPointThatChanged.getName(),
                                       alarm.signal,
                                       alarm.acknowledged ? "YES" : "no") );
    }

    // The alarm is de-asserted 
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("%s lowered.",
                                       modelPointThatChanged.getName()) );
    }
}




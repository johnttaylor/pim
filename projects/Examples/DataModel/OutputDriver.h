#ifndef OutputDriver_h_
#define OutputDriver_h_
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
/** @file */

#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "MpAlarm.h"


/** This concrete class is an dumb-downed simulation of an output driver that
    asserts/de-asserts discrete GPIO signals when the application's High and
    Low alarms are raised/lowered. Since the example is intended to run on a 
    desktop PC - the output signals are printf (using the CPL Trace engine)
    to the TShell console.

    An asserted Alarm outputs a High (1) signal. 
    A de-asserted Alarm outputs a Low (0) signal.
 */
class OutputDriver : public Cpl::Itc::CloseSync
{

public:
    /// Constructor.
    OutputDriver( Cpl::Dm::MailboxServer& myMbox,
                  MpAlarm&                mpHighAlarm,
                  MpAlarm&                mpLowAlarm );


public:
    /** This ITC method is used to initialize the driver while executing
        in the 'driver' thread.  See Cpl::Itc::OpenRequest
     */
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg );

    /** This ITC method is used to shutdown the driver while executing
        in the 'driver' thread.  See Cpl::Itc::OpenRequest
     */
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg );


protected:
    /** Change notification method for when the Alarm MPs change state.
        Note: A Single callback function is used for both Alarms.
     */
    void alarmMpChanged( MpAlarm& modelPointThatChanged, Cpl::Dm::SubscriberApi& clientObserver ) noexcept;

protected:
    /// High Alarm MP to monitor
    MpAlarm&                m_mpHiAlarm;

    /// Low Alarm MP to monitor
    MpAlarm&                m_mpLoAlarm;

    /// Observer instance for monitoring the High Alarm MP
    Cpl::Dm::SubscriberComposer<OutputDriver, MpAlarm>  m_observerHiAlarmMp;

    /// Observer instance for monitoring the Low Alarm MP
    Cpl::Dm::SubscriberComposer<OutputDriver, MpAlarm>  m_observerLoAlarmMp;

    /// My open state
    bool                    m_opened;
};


#endif  // end header latch

#ifndef Algorithm_h_
#define Algorithm_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Itc/CloseSync.h"
#include "Cpl/System/Timer.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "MpAlarm.h"
#include "MpMetrics.h"

/** This concrete class is representative of processing that is done
    on the Application's input and generates outputs per the Application 
    requirements. The Algorithm module executes in the Application thread. 
    The Algorithm class does the following:

    - Calculates the minimum, maximum, and a running average of its input value.  
      These metrics are stored in the MpMetrics model point.

    - Generates a 'High Alarm' when the current input value goes above the value 
      of 995.  The Alarm is lowered once the current value drops below 5.

    - Generates a 'Low Alarm' when the current input value goes below the value 
      of 5.  The Alarm is lowered once the current value goes above 995.

    The algorithm executes at 10Hz.
 */
class Algorithm : public Cpl::Itc::CloseSync, public Cpl::System::Timer
{

public:
    /** Constructor. In this example references to all of model points that 
        the Algorithm classes operates on are provided in the constructor. 
        Depending on the nature of the application and its design criteria an
        alternative would be to have the Algorithm class directly access the
        needed MP by the instance names (i.e. include "ModelPoints.h" in 
        the .cpp file to gain access to the MP instance).  Note: This 'short-cut'
        should NEVER be used for driver or middle-ware classes - only 
        application specific classes.
     */
    Algorithm( Cpl::Dm::MailboxServer&  myMbox, 
               Cpl::Dm::Mp::Uint32&     mpInputSignal,
               MpAlarm&                 mpHiAlarm,
               MpAlarm&                 mpLoAlarm,
               Cpl::Dm::Mp::Uint32&     mpHiAlarmCounts,
               Cpl::Dm::Mp::Uint32&     mpLoAlarmCounts,
               MpMetrics&               mpMetrics );


public:
    /** This ITC method is used to initialize the Algorithm while executing
        in the 'application' thread.  See Cpl::Itc::OpenRequest
     */
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg );

    /** This ITC method is used to shutdown the Algorithm while executing
        in the 'application' thread.  See Cpl::Itc::OpenRequest
     */
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg );


protected:
    /// My 10Hz Timer callback
    void expired( void ) noexcept;

protected:
    /// The input signal MP 
    Cpl::Dm::Mp::Uint32&    m_mpSample;

    /// High Alarm MP 
    MpAlarm&                m_mpHiAlarm;

    /// Low Alarm MP 
    MpAlarm&                m_mpLoAlarm;

    /// Metrics MP
    MpMetrics&              m_mpMetrics;

    /// High Alarm Counts MP 
    Cpl::Dm::Mp::Uint32&    m_mpHiAlarmCounts;

    /// Low Alarm Counts MP 
    Cpl::Dm::Mp::Uint32&    m_mpLoAlarmCounts;

    /// My open state
    bool                    m_opened;
};

#endif  // end header latch

#ifndef Algorithm_h_
#define Algorithm_h_
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
      of 998.  The Alarm is lowered once the current value drops below 2.

    - Generates a 'Low Alarm' when the current input value goes below the value 
      of 2.  The Alarm is lowered once the current value goes above 998.

    The algorithm executes at 10Hz.
 */
class Algorithm : public Cpl::Itc::CloseSync, public Cpl::System::Timer
{

public:
    /// Constructor.
    Algorithm( Cpl::Dm::MailboxServer&  myMbox, 
               Cpl::Dm::Mp::Uint32&     mpInputSignal,
               MpAlarm&                 mpHiAlarm,
               MpAlarm&                 mpLoAlarm,
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

    /// My open state
    bool                    m_opened;
};

#endif  // end header latch

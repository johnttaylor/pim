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

#include "colony_config.h"
#include "Algorithm.h"
#include "Main.h"
#include <stdlib.h>
#include <time.h>


#ifndef OPTION_ALGORITHM_SAMPLE_TIME_MS
#define OPTION_ALGORITHM_SAMPLE_TIME_MS                 100     // 10Hz
#endif

#ifndef OPTION_ALGORITHM_HIALARM_RAISE_THRESHOLD      
#define OPTION_ALGORITHM_HIALARM_RAISE_THRESHOLD        995
#endif
#ifndef OPTION_ALGORITHM_HIALARM_LOWER_THRESHOLD      
#define OPTION_ALGORITHM_HIALARM_LOWER_THRESHOLD        5
#endif

#ifndef OPTION_ALGORITHM_LOALARM_RAISE_THRESHOLD      
#define OPTION_ALGORITHM_LOALARM_RAISE_THRESHOLD        5
#endif
#ifndef OPTION_ALGORITHM_LOALARM_LOWER_THRESHOLD      
#define OPTION_ALGORITHM_LOALARM_LOWER_THRESHOLD        995
#endif


///////////////////////////////
Algorithm::Algorithm( Cpl::Dm::MailboxServer&  myMbox,
                      Cpl::Dm::Mp::Uint32&     mpInputSignal,
                      MpAlarm&                 mpHiAlarm,
                      MpAlarm&                 mpLoAlarm,
                      Cpl::Dm::Mp::Uint32&     mpHiAlarmCounts,
                      Cpl::Dm::Mp::Uint32&     mpLoAlarmCounts,
                      MpMetrics&               mpMetrics )
    : Cpl::Itc::CloseSync( myMbox )
    , Cpl::System::Timer( myMbox )
    , m_mpSample( mpInputSignal )
    , m_mpHiAlarm( mpHiAlarm )
    , m_mpLoAlarm( mpLoAlarm )
    , m_mpMetrics( mpMetrics )
    , m_mpHiAlarmCounts( mpHiAlarmCounts )
    , m_mpLoAlarmCounts( mpLoAlarmCounts )
    , m_opened( false )
{
}

///////////////////////////////

// Note: This method runs in the 'Application' thread
void Algorithm::request( Cpl::Itc::OpenRequest::OpenMsg& msg )
{
    if ( !m_opened )
    {
        // Housekeeping
        m_opened = true;
        
        // Ensure I start with NO alarms asserted
        m_mpHiAlarm.setInvalid();
        m_mpLoAlarm.setInvalid();

        // Go do the processing
        expired();
    }

    // The request method is an ITC call -->the server is required to 'return the message' to the client
    msg.returnToSender();
}

// Note: This method runs in the 'Application' thread
void Algorithm::request( Cpl::Itc::CloseRequest::CloseMsg& msg )
{
    if ( m_opened )
    {
        m_opened = false;
        Timer::stop();
    }

    // The request method is an ITC call -->the server is required to 'return the message' to the client
    msg.returnToSender();
}



///////////////////////////////

// Note: This method runs in the 'Application' thread
void Algorithm::expired( void ) noexcept
{
    // Get the current input signal (do nothing if there is no valid sample)
    uint32_t inSignal;
    if ( m_mpSample.read( inSignal ) )
    {
        // Collect/generate metrics
        m_mpMetrics.newSample( inSignal );

        // Raise the HI alarm (note: MP invalid is the 'de-asserted state')
        if ( m_mpHiAlarm.isNotValid() == true )
        {
            if ( inSignal >= OPTION_ALGORITHM_HIALARM_RAISE_THRESHOLD )
            {
                m_mpHiAlarm.raise( inSignal );  
                m_mpHiAlarmCounts.increment();
            }
        }

        // Lower the HI alarm
        else
        {
            if ( inSignal <= OPTION_ALGORITHM_HIALARM_LOWER_THRESHOLD )
            {
                m_mpHiAlarm.lower();
            }
        }

        // Raise the LO alarm (note: MP invalid is the 'de-asserted state')
        if ( m_mpLoAlarm.isNotValid() == true )
        {
            if ( inSignal <= OPTION_ALGORITHM_LOALARM_RAISE_THRESHOLD )
            {
                m_mpLoAlarm.raise( inSignal );  
                m_mpLoAlarmCounts.increment();
            }
        }

        // Lower the LO alarm
        else
        {
            if ( inSignal >= OPTION_ALGORITHM_LOALARM_LOWER_THRESHOLD )
            {
                m_mpLoAlarm.lower();
            }
        }

    }

    // Restart my processing timer
    Timer::start( OPTION_ALGORITHM_SAMPLE_TIME_MS );
}


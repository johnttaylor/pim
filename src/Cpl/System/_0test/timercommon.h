#ifndef TimerCommon_h_
#define TimerCommon_h_
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

#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Timer.h"
#include "Cpl/System/EventLoop.h"


#define SECT_                       "_0test"

#ifndef NUM_SEQ_ 
#define NUM_SEQ_                    3
#endif
#define TOLERANCE_                  2
#define DELAY_                      2000

#define START_EVENT                 0

#define FAST_TICKSRC_MS_PER_TICK    1
#define SLOW_TICKSRC_MS_PER_TICK    10

#define ROSE_T1                     10      // Tick Hz 1000 (1ms)  
#define ROSE_T2                     23      // Tick Hz 1000 (1ms)  
#define DAISY_T1                    11      // Tick Hz 1000 (1ms)  
#define DAISY_T2                    47      // Tick Hz 1000 (1ms)  
#define OAK_T1                      60      // Tick Hz 1000 (1ms)  
#define OAK_T2                      125     // Tick Hz 1000 (1ms)  
#define PINE_T1                     50      // Tick Hz 1000 (1ms)
#define PINE_T2                     300     // Tick Hz 1000 (1ms)
#define APPLE_T1                    50      // Tick Hz 20   (50ms)
#define APPLE_T2                    300     // Tick Hz 20   (50ms)
#define ORANGE_T1                   60      // Tick Hz 20   (50ms)
#define ORANGE_T2                   125     // Tick Hz 20   (50ms)



#define MAX_COUNT_(ttime, tdur )          (ttime/tdur)




////////////////////////////////////////////////////////////////////////////////
class AppObject
{
public:
    ///
    const char*                             m_name;

    ///
    unsigned long                           m_timer1Duration;
    ///
    Cpl::System::TimerComposer<AppObject>   m_timer1;
    ///
    unsigned long                           m_count1;
    ///
    unsigned long                           m_startTime1;
    ///
    unsigned long                           m_minTime1;
    ///
    unsigned long                           m_maxTime1;
    ///
    unsigned long                           m_sumTime1;

    ///
    unsigned long                           m_timer2Duration;
    ///
    Cpl::System::TimerComposer<AppObject>   m_timer2;
    ///
    unsigned long                           m_count2;

    ///
    unsigned long                           m_timeMark1;
    ///
    unsigned long                           m_timeMark2;
    ///
    unsigned long                           m_deltaTime1;
    ///
    unsigned long                           m_deltaTime2;
    ///
    unsigned long                           m_startTime2;
    ///
    unsigned long                           m_minTime2;
    ///
    unsigned long                           m_maxTime2;
    ///
    unsigned long                           m_sumTime2;


    /// Constructor
    AppObject( const char* name, Cpl::System::TimerManager& timingSource, unsigned long timer1Duration, unsigned long timer2Duration )
        :m_name( name ),
        m_timer1Duration( timer1Duration ),
        m_timer1( timingSource, *this, &AppObject::timer1Expired ),
        m_timer2Duration( timer2Duration ),
        m_timer2( timingSource, *this, &AppObject::timer2Expired )
    {
    }

public:
    ///
    void start()
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting timers (%lu, %lu) for: %s", m_timer1Duration, m_timer2Duration, m_name) );
        m_count1   = 0;
        m_minTime1 = (unsigned long) (-1);
        m_maxTime1 = 0;
        m_sumTime1 = 0;
        m_count2   = 0;
        m_minTime2 = (unsigned long) (-1);
        m_maxTime2 = 0;
        m_sumTime2 = 0;

        m_timeMark1 = m_startTime1 = Cpl::System::ElapsedTime::milliseconds();
        m_timer1.start( m_timer1Duration );
        m_timeMark2 = m_startTime2 = Cpl::System::ElapsedTime::milliseconds();
        m_timer2.start( m_timer2Duration );
    }

    ///
    void stop()
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Stopping AppObject: %s....", m_name) );

        m_timer1.stop();
        m_deltaTime1 = Cpl::System::ElapsedTime::deltaMilliseconds( m_timeMark1 );
        m_timer2.stop();
        m_deltaTime2 = Cpl::System::ElapsedTime::deltaMilliseconds( m_timeMark1 );
    }

public:
    ///
    void timer1Expired( void )
    {
        // Capture stats
        unsigned elasped = Cpl::System::ElapsedTime::deltaMilliseconds( m_startTime1 );
        if ( elasped < m_minTime1 )
        {
            m_minTime1 = elasped;
        }
        if ( elasped > m_maxTime1 )
        {
            m_maxTime1 = elasped;
        }
        m_sumTime1 += elasped;

        m_count1++;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("(%s)::Timer 1::Expired.  Total count=%lu (%lu)", m_name, m_count1, m_timer1Duration) );
        m_startTime1 = Cpl::System::ElapsedTime::milliseconds();
        m_timer1.start( m_timer1Duration );
    }

    void timer2Expired( void )
    {
        // Capture stats
        unsigned elasped = Cpl::System::ElapsedTime::deltaMilliseconds( m_startTime2 );
        if ( elasped < m_minTime2 )
        {
            m_minTime2 = elasped;
        }
        if ( elasped > m_maxTime2 )
        {
            m_maxTime2 = elasped;
        }
        m_sumTime2 += elasped;

        m_count2++;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("(%s)::Timer 2 ::Expired.  Total count=%lu (%lu)", m_name, m_count2, m_timer2Duration) );
        m_startTime2 = Cpl::System::ElapsedTime::milliseconds();
        m_timer2.start( m_timer2Duration );
    }

public:
    void displayTimer1( unsigned long maxCount )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("%s#1: delta=%lu, dur=%lu --> cnt (%lu) <=? %lu. min=%lu, max=%lu, avg=%.2f", m_name, m_deltaTime1, m_timer1Duration, m_count1, maxCount, m_minTime1, m_maxTime1, m_sumTime1 / (double) m_count1) );
    }

    void displayTimer2( unsigned long maxCount )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("%s#2: delta=%lu, dur=%lu --> cnt (%lu) <=? %lu. min=%lu, max=%lu, avg=%.2f", m_name, m_deltaTime2, m_timer2Duration, m_count2, maxCount, m_minTime2, m_maxTime2, m_sumTime2 / (double) m_count2) );
    }

};


////////////////////////////////////////////////////////////////////////////////
class Master : public Cpl::System::EventLoop, public Cpl::System::Timer
{
public:
    ///
    unsigned long           m_delayMsecs;
    ///
    AppObject               m_appObj1;
    ///
    AppObject               m_appObj2;
    ///
    Cpl::System::Signable&  m_waiter;

public:
    ///
    Master( unsigned long timerResolution, unsigned long delayMsecs,
            const char* nameObj1, unsigned long timer1DurationObj1, unsigned long timer2DurationObj1,
            const char* nameObj2, unsigned long timer1DurationObj2, unsigned long timer2DurationObj2,
            Cpl::System::Signable& waiter )
        :Cpl::System::EventLoop( timerResolution ),
        Cpl::System::Timer( * ((Cpl::System::TimerManager*)this) ),
        m_delayMsecs( delayMsecs ),
        m_appObj1( nameObj1, *this, timer1DurationObj1, timer2DurationObj1 ),
        m_appObj2( nameObj2, *this, timer1DurationObj2, timer2DurationObj2 ),
        m_waiter( waiter )
    {
    }

    void expired( void ) noexcept
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("----- Stopping Timer Sequence....") );
        m_appObj1.stop();
        m_appObj2.stop();
        m_waiter.signal();
    }


public:
    ///
    void processEventFlag( uint8_t eventNumber ) noexcept
    {
        switch ( eventNumber )
        {
            case START_EVENT:
                CPL_SYSTEM_TRACE_MSG( SECT_, ("----- Starting Timer Sequence....") );
                start( m_delayMsecs );
                m_appObj1.start();
                m_appObj2.start();
                break;

            default:
                CPL_SYSTEM_TRACE_MSG( SECT_, ("----- UNSUPPORTED EVENT FLAG WAS SET: %lu", eventNumber) );
                break;
        }

    };
};





#endif // end header latch
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2017-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "colony_config.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/Tls.h"
#include "Cpl/System/Trace.h"

#ifndef OPTION_0TEST_HW_BASIC2_STACK_SIZE
#define OPTION_0TEST_HW_BASIC2_STACK_SIZE       (512*3)
#endif

#define SECT_       "_0test"

extern void runtests( void );

/// 
using namespace Cpl::System;


static Mutex myLock_;
static float testGlobal_;

static bool modifyGlobal_( float op1, float op2, float expected )
{
    Mutex::ScopeBlock guard( myLock_ );

    bool result = true;
    testGlobal_ = (op1 * 2.0) / (op2 + op1);
    if ( testGlobal_ < (expected - 0.0001) || testGlobal_ >( expected + 0.0001 ) )
        result = false;

    return result;
}


////////////////////////////////////////////////////////////////////////////////
namespace {



class MyRunnable : public Runnable
{
public:
    ///
    uint8_t     m_ledNum;
    ///
    Tls&        m_tlsKey;
    ///
    size_t      m_tlsCounter;

public:
    ///
    MyRunnable( Tls& tlsKey, uint8_t ledNum )
        :m_ledNum( ledNum ),
        m_tlsKey( tlsKey ),
        m_tlsCounter( ledNum )
    {
    }

public:
    ///
    void appRun()
    {
        if ( m_tlsKey.get() != 0 )
        {
            FatalError::logf( "(%s) Bad initial TLS value (%p)", Thread::myName(), m_tlsKey.get() );
        }

        for ( ;;)
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "Top of MyRunnable::appRun()" ) );
            m_tlsKey.set( (void*) m_tlsCounter );
            toggleLED();
            Thread::wait();
            toggleLED();
            Thread::wait();
            if ( m_tlsKey.get() != (void*) m_tlsCounter )
                {
                FatalError::logf( "(%s) Bad TLS value (%p) - should be (%p)", Thread::myName(), m_tlsKey.get(), m_tlsCounter );
                }
            m_tlsCounter++;
        }
    }


    void toggleLED()
    {
        if ( m_ledNum == 1 )
        {
            Bsp_Api_toggle_debug1();
        } else
        {
            Bsp_Api_toggle_debug2();
        }
    }
};

class MyRunnable2 : public Runnable
{
public:
    ///
    Thread&                     m_ledThread;
    ///
    uint16_t                    m_onTime_ms;
    ///
    uint16_t                    m_offTime_ms;
    ///
    ElapsedTime::Precision_T    m_ptime;
    ///
    unsigned long               m_msec;

public:
    MyRunnable2( Thread& ledThread, uint16_t onTime_ms, uint16_t offTime_ms )
        :m_ledThread( ledThread ),
        m_onTime_ms( onTime_ms ),
        m_offTime_ms( offTime_ms )
    {
    }

public:
    void appRun()
    {
        unsigned long sleepTime      = m_onTime_ms + m_offTime_ms;
        m_ptime                      = ElapsedTime::precision();
        m_msec                       = ElapsedTime::milliseconds();

        for ( ;;)
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Top of MyRunnable2::appRun()") );

            Api::sleep( m_onTime_ms );
            m_ledThread.signal();
            Api::sleep( m_offTime_ms );
            m_ledThread.signal();


            ElapsedTime::Precision_T ptime     = ElapsedTime::precision();
            unsigned long            msec      = ElapsedTime::milliseconds();
            unsigned long            deltaM    = ElapsedTime::deltaMilliseconds( m_msec, msec );
            ElapsedTime::Precision_T deltaP    = ElapsedTime::deltaPrecision( m_ptime, ptime );
            unsigned long            flatten   = deltaP.m_seconds * 1000 + deltaP.m_thousandths;

            //CPL_SYSTEM_TRACE_MSG( SECT_, ("msec=%u, ptime.s=%u, ptime.msec=%u, deltaM=%u, flatten=%u, sleepTime=%u", msec, ptime.m_seconds, ptime.m_thousandths, deltaM, flatten, sleepTime) );
 
            if ( flatten < sleepTime )
            {
                FatalError::logf( "Elapsed Precision_T delta is wrong. flatten=%lu, sleepTime=%lu", flatten, sleepTime );
            }

            if ( deltaM < sleepTime )
            {
                FatalError::logf( "Elapsed msec delta is wrong. deltaM=%lu, sleepTime=%lu", deltaM, sleepTime );
            }

            if ( flatten < deltaM - 1 || flatten > deltaM + 1 )
            {
                FatalError::logf( "Precision time is not insync with milliseconds time, flatten=%lu, deltaM=%lu", flatten, deltaM );
            }

            m_ptime = ptime;
            m_msec  = msec;
        }
    }
};


class MyRunnable3 : public Runnable
{
public:
    ///
    float m_op1;
    ///
    float m_op2;
    ///
    float m_expected;

public:
    ///
    MyRunnable3( float op1, float op2 )
        :m_op1( op1 ),
        m_op2( op2 ),
        m_expected( (op1 * 2.0) / (op2 + op1) )
    {
    }


public:
    ///
    void appRun()
    {
        for ( ;;)
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Top of MyRunnable3::appRun()") );

            if ( modifyGlobal_( m_op1, m_op2, m_expected ) == false )
            {
                FatalError::logf( "Mutex failed. Thread=%s", Thread::myName() );
            }

            Api::sleep( 300 );
        }
    }
};

};  // end namespace


////////////////////////////////////////////////////////////////////////////////


void runtests( void )
{
    static bool created_threads = false;

    CPL_SYSTEM_TRACE_MSG( SECT_, ("running tests...") );

    if ( !created_threads )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Creating threads...") );

        created_threads = true;
        Tls* tlsKey = new Cpl::System::Tls();

        MyRunnable*  appleLed       = new MyRunnable( *tlsKey, 1 );
        Thread*      appleLedThread = Thread::create( *appleLed, "AppleLED", CPL_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );
        MyRunnable2* appleTimer     = new MyRunnable2( *appleLedThread, 1000, 1000 );
        Thread::create( *appleTimer, "AppleTimer", CPL_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );

        MyRunnable*  orangeLed       = new MyRunnable( *tlsKey, 2 );
        Thread*      orangeLedThread = Thread::create( *orangeLed, "OrangeLED", CPL_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );
        MyRunnable2* orangeTimer     = new MyRunnable2( *orangeLedThread, 750, 250 );
        Thread::create( *orangeTimer, "OrangeTimer", CPL_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );

        MyRunnable3* t1 = new MyRunnable3( 3.14159, 3 );
        Thread::create( *t1, "T1", CPL_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );
        MyRunnable3* t2 = new MyRunnable3( 2.71828, 7 );
        Thread::create( *t2, "T2", CPL_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );
        MyRunnable3* t3 = new MyRunnable3( 64.0, 128.0 );
        Thread::create( *t3, "T3", CPL_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );
    }


}

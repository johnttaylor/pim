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

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "common.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/Itc/PeriodicScheduler.h"


/// 
using namespace Cpl::Itc;

#define MY_EVENT_NUMBER     8
#define MY_EVENT_MASK       0x00000300

////////////////////////////////////////////////////////////////////////////////
/* The test app consists of 3 threads:
   - Two client threads, one contains a viewer, the other contains a writer
   - One model thread that 'owns' the data being viewed/written
*/

static unsigned  oakCount_;
static void oakProcessInterval( Cpl::System::ElapsedTime::Precision_T currentTick, Cpl::System::ElapsedTime::Precision_T currentInterval, void* context )
{
    oakCount_++;
}

static Cpl::System::PeriodicScheduler::Interval_T intervals_[] =
{
    { oakProcessInterval, { 0,100 }, nullptr },
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

static unsigned startLoopCount_;
static void loopStart( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    startLoopCount_++;
}

static unsigned endLoopCount_;
static void loopEnd( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    endLoopCount_++;
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "mvc", "[mvc]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();


    MyMailboxServer   modelMbox( MY_EVENT_MASK );
    Model             myModel( modelMbox );
    ViewRequest::SAP  modelViewSAP( myModel, modelMbox );

    PeriodicScheduler viewerMbox( intervals_, loopStart, loopEnd );
    Viewer            myViewer( 0, viewerMbox, modelViewSAP );

    Master            masterRun( NUM_SEQ_, NUM_WRITES_, myViewer, myModel, myModel, Cpl::System::Thread::getCurrent() );

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( viewerMbox, "Viewer" );
    Cpl::System::Thread* t2 = Cpl::System::Thread::create( modelMbox, "Model" );
    Cpl::System::Thread* t3 = Cpl::System::Thread::create( masterRun, "MASTER" );

    Cpl::System::ElapsedTime::Precision_T schedStartTime = Cpl::System::ElapsedTime::precision();

    unsigned long startTime = Cpl::System::ElapsedTime::milliseconds();

    // Allow time for the threads to start
    Cpl::System::Api::sleep( 300 );
    REQUIRE( startLoopCount_ == 1 );
    REQUIRE( endLoopCount_ == 0 );
    
    // Test default signal behavior 
    //Cpl::System::Api::sleep( 50 );
    viewerMbox.notify( MY_EVENT_NUMBER );


    // Validate result of each sequence
    int i;
    for ( i=0; i < NUM_SEQ_; i++ )
    {
        modelMbox.notify( MY_EVENT_NUMBER );
        modelMbox.notify( MY_EVENT_NUMBER + 1 );
        modelMbox.notify( MY_EVENT_NUMBER + 2 );
        Cpl::System::Thread::wait();
        Cpl::System::Api::sleep( 50 );
        REQUIRE( myModel.m_value == (NUM_WRITES_ - 1) * ATOMIC_MODIFY_ );
        REQUIRE( myViewer.m_attachRspMsg.getPayload().m_value == (NUM_WRITES_ - 1) * ATOMIC_MODIFY_ );
        REQUIRE( myViewer.m_ownAttachMsg == true );
        REQUIRE( myViewer.m_ownDetachMsg == true );
        REQUIRE( myViewer.m_pendingCloseMsgPtr == 0 );
        REQUIRE( myViewer.m_opened == false );
        REQUIRE( modelMbox.m_sigCount == (i + 1) * 2 );
        REQUIRE( modelMbox.m_sigCountUnexpected == i + 1 );
        t3->signal();
    }


    // Wait for all of the sequences to complete
    Cpl::System::Thread::wait();

    // Shutdown threads
    viewerMbox.pleaseStop();
    modelMbox.pleaseStop();
    Cpl::System::Api::sleep( 100 ); // allow time for threads to stop
    REQUIRE( t1->isRunning() == false );
    REQUIRE( t2->isRunning() == false );
    REQUIRE( t3->isRunning() == false );

    REQUIRE( startLoopCount_ == 1 );
    REQUIRE( endLoopCount_ == 1 );

    unsigned elapsedTime = Cpl::System::ElapsedTime::deltaMilliseconds( startTime );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Viewer Timer count=%lu, max possible value=%lu", myViewer.m_timerExpiredCount, elapsedTime / TIMER_DELAY) );
    REQUIRE( ((myViewer.m_timerExpiredCount > 1) && (myViewer.m_timerExpiredCount < elapsedTime / TIMER_DELAY)) );

    // Get test end time
    Cpl::System::ElapsedTime::Precision_T deltaTime = Cpl::System::ElapsedTime::deltaPrecision( schedStartTime );
    uint64_t flatDelta = deltaTime.asFlatTime();
    unsigned long maxExpectedCount = (unsigned long) (flatDelta / 100);
    REQUIRE( oakCount_ <= maxExpectedCount );
    REQUIRE( oakCount_ > maxExpectedCount / 2 );    // Require a minimum count -->but since timing is not guaranteed leave a wide margin of error

    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Thread::destroy( *t2 );
    Cpl::System::Thread::destroy( *t3 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

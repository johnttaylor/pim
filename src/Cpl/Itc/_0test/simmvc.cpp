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
#include "Cpl/System/ElapsedTime.h"
#include "common.h"


#define MY_EVENT_NUMBER     8
#define MY_EVENT_MASK       0x00000100

/// 
using namespace Cpl::Itc;

////////////////////////////////////////////////////////////////////////////////
/* The test app consists of 3 threads:
   - Two client threads, one contains a viewer, the other contains a writer
   - One model thread that 'owns' the data being viewed/written
*/

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "simmvc", "[simmvc]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();


    MyMailboxServer   modelMbox( MY_EVENT_MASK );
    Model             myModel( modelMbox );
    ViewRequest::SAP  modelViewSAP( myModel, modelMbox );

    MailboxServer     viewerMbox;
    Viewer            myViewer( 0, viewerMbox, modelViewSAP );

    Master            masterRun( NUM_SEQ_, NUM_WRITES_, myViewer, myModel, myModel, Cpl::System::Thread::getCurrent(), false );

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( viewerMbox, "Viewer" );
    Cpl::System::Thread* t2 = Cpl::System::Thread::create( modelMbox, "Model" );
    Cpl::System::Thread* t3 = Cpl::System::Thread::create( masterRun, "MASTER" );
    Cpl::System::Api::sleep( 50 ); // Allow time for threads to actually spin-up

    // Test the default signal handler 
    viewerMbox.notify( MY_EVENT_NUMBER );

    unsigned long startTime = Cpl::System::ElapsedTime::milliseconds();

    // Validate result of each sequence
    int i;
    for ( i=0; i < NUM_SEQ_; i++ )
    {
        modelMbox.notify( MY_EVENT_NUMBER );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("@@ TICK SOURCE: Starting sequence# %d...", i + 1) );
        bool signaled = false;
        for ( int i=0; i < 50; i++ )
        {
            Cpl::System::SimTick::advance( 100 );
            Cpl::System::Api::sleepInRealTime( 100 );
            if ( Cpl::System::Thread::tryWait() )
            {
                signaled = true;
                break;
            }
        }
        CPL_SYSTEM_TRACE_MSG( SECT_, ("  @@ TICK SOURCE: pause before checking result for seq# %d, signaled=%d.  Seq completed at sim tick count of: %lu", i + 1, signaled, Cpl::System::SimTick::current()) );
        REQUIRE( signaled );
        REQUIRE( myModel.m_value == (NUM_WRITES_ - 1) * ATOMIC_MODIFY_ );
        REQUIRE( myViewer.m_attachRspMsg.getPayload().m_value == (NUM_WRITES_ - 1) * ATOMIC_MODIFY_ );
        REQUIRE( myViewer.m_ownAttachMsg == true );
        REQUIRE( myViewer.m_ownDetachMsg == true );
        REQUIRE( myViewer.m_pendingCloseMsgPtr == 0 );
        REQUIRE( myViewer.m_opened == false );
        REQUIRE( modelMbox.m_sigCount == i + 1 );
        t3->signal();
    }


    // Wait for all of the sequences to complete
    Cpl::System::SimTick::advance( 10 );
    Cpl::System::Thread::wait();

    // Shutdown threads
    viewerMbox.pleaseStop();
    modelMbox.pleaseStop();
    Cpl::System::SimTick::advance( 50 );
    Cpl::System::Api::sleep( 50 ); // allow time for threads to stop
    REQUIRE( t1->isRunning() == false );
    REQUIRE( t2->isRunning() == false );
    REQUIRE( t3->isRunning() == false );

    unsigned elapsedTime = Cpl::System::ElapsedTime::deltaMilliseconds( startTime );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Viewer Timer count=%lu, max possible value=%lu", myViewer.m_timerExpiredCount, elapsedTime / TIMER_DELAY) );
    REQUIRE( ((myViewer.m_timerExpiredCount > 1) && (myViewer.m_timerExpiredCount < elapsedTime / TIMER_DELAY)) );

    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Thread::destroy( *t2 );
    Cpl::System::Thread::destroy( *t3 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

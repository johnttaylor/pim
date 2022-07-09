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

#include "Cpl/Itc/_0test/common.h"
#include "Bsp/Api.h"

#define REQUIRE(a,b)  if ( !(a) ) Cpl::System::FatalError::logf(b)

#define MY_EVENT_NUMBER     8
#define MY_EVENT_MASK       0x00000100

/// 
using namespace Cpl::Itc;

////////////////////////////////////////////////////////////////////////////////
namespace {

class TestUnit
{
public:
    ///
    MyMailboxServer         m_modelMbox;
    ///
    Model                   m_myModel;
    ///
    ViewRequest::SAP        m_modelViewSAP;
    ///
    MailboxServer           m_viewerMbox;
    ///
    Viewer                  m_myViewer;

public:
    ///
    TestUnit()
        :m_modelMbox( MY_EVENT_MASK ),
        m_myModel( m_modelMbox ),
        m_modelViewSAP( m_myModel, m_modelMbox ),
        m_viewerMbox(),
        m_myViewer( 0, m_viewerMbox, m_modelViewSAP )
    {
    }
};


class Checker : public Cpl::System::Runnable
{

public:
    ///
    Checker()
    {
    }

public:
    ///
    void appRun()
    {
        CPL_SYSTEM_TRACE_FUNC( SECT_ );

        for ( ;;)
        {
            Bsp_Api_turnOn_debug1();

            TestUnit*            testPtr         = new(std::nothrow) TestUnit();
            Master*              masterRunPtr    = new(std::nothrow) Master( NUM_SEQ_, NUM_WRITES_, testPtr->m_myViewer, testPtr->m_myModel, testPtr->m_myModel, Cpl::System::Thread::getCurrent() );
            Cpl::System::Thread* viewerThreadPtr = Cpl::System::Thread::create( testPtr->m_viewerMbox, "Viewer" );
            Cpl::System::Thread* modelThreadPtr  = Cpl::System::Thread::create( testPtr->m_modelMbox, "Model" );
            Cpl::System::Thread* masterThreadPtr = Cpl::System::Thread::create( *masterRunPtr, "MASTER" );

            CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting the sequence....") );
            int i;
            for ( i=0; i < NUM_SEQ_; i++ )
            {
                Bsp_Api_toggle_debug2();
                testPtr->m_modelMbox.notify( MY_EVENT_NUMBER );
                Cpl::System::Thread::wait();
                Cpl::System::Api::sleep( 50 );
                REQUIRE( testPtr->m_myModel.m_value == (NUM_WRITES_ - 1) * ATOMIC_MODIFY_, "Fail: 1" );
                REQUIRE( testPtr->m_myViewer.m_attachRspMsg.getPayload().m_value == (NUM_WRITES_ - 1)*ATOMIC_MODIFY_, "Fail: 2" );
                REQUIRE( testPtr->m_myViewer.m_ownAttachMsg == true, "Fail: 3" );
                REQUIRE( testPtr->m_myViewer.m_ownDetachMsg == true, "Fail: 4" );
                REQUIRE( testPtr->m_myViewer.m_pendingCloseMsgPtr == 0, "Fail: 5" );
                REQUIRE( testPtr->m_myViewer.m_opened == false, "Fail: 6" );
                REQUIRE( testPtr->m_modelMbox.m_sigCount == i + 1, "Fail: 7" );
                REQUIRE( testPtr->m_modelMbox.m_sigCountUnexpected == 0, "Fail: 8" );
                masterThreadPtr->signal();
            }

            // Wait for all of the sequences to complete
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Sequence Complete: Waiting for all threads to finish") );
            Bsp_Api_turnOff_debug2();
            Cpl::System::Thread::wait();

            // Shutdown threads
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Shutting down threads...") );
            Bsp_Api_turnOff_debug1();
            testPtr->m_viewerMbox.pleaseStop();
            testPtr->m_modelMbox.pleaseStop();
            Cpl::System::Api::sleep( 50 ); // allow time for threads to stop
            REQUIRE( viewerThreadPtr->isRunning() == false, "Fail: 8" );
            REQUIRE( modelThreadPtr->isRunning() == false, "Fail: 9" );
            REQUIRE( masterThreadPtr->isRunning() == false, "Fail: 10" );

            Cpl::System::Thread::destroy( *viewerThreadPtr );
            Cpl::System::Thread::destroy( *modelThreadPtr );
            Cpl::System::Thread::destroy( *masterThreadPtr );
            delete masterRunPtr;
            delete testPtr;

            // Test done -->flash LED to indicate successful test (i.e. not stuck in the FatalError handler)
            CPL_SYSTEM_TRACE_MSG( SECT_, ("All threads to destroyed!") );
            for ( i=0; i < 4 * 2; i++ )
            {
                Bsp_Api_toggle_debug1();
                Cpl::System::Api::sleep( 250 );
            }
        }
    }
};

};  // end namespace


////////////////////////////////////////////////////////////////////////////////

static Checker checker_;


////////////////////////////////////////////////////////////////////////////////
void runtests( void )
{
    Cpl::System::Thread::create( checker_, "CHECKER" );

    // Start the scheduler
    Cpl::System::Api::enableScheduling();
}

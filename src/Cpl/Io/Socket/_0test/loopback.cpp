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
/** @file */

#include "loopback.h"
#include "Catch/catch.hpp"
#include "Catch/helpers.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Itc/MailboxServer.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Io/Socket/ListenerClientSync.h"
#include "Cpl/Io/Socket/InputOutput.h"
#include "Cpl/Io/Socket/Connector.h"


///
using namespace Cpl::Io::Socket;

static Listener*  listenerPtr_;
static Connector* connectorPtr_;

#define SECT_       "_0test"

#define PORT_NUM_   5002


///////////////////
void initialize_loopback( Cpl::Io::Socket::Listener& listener, Cpl::Io::Socket::Connector& connector )
{
    listenerPtr_  = &listener;
    connectorPtr_ = &connector;
}


///////////////////
namespace
{

/// Note: The interaction between a Reader and the LoopBackClient is not truly thread-safe - but it is good enough for the unittest.
class Reader : public Cpl::System::Runnable
{
public:
    ///
    bool        m_run;
    ///
    bool*       m_doneFlagPtr;
    ///
    InputOutput m_stream;
    ///
    Cpl::System::Thread* m_myThreadPtr;

public:
    ///
    Reader() :m_run( true ), m_doneFlagPtr( 0 ), m_myThreadPtr( 0 ) {}


public:
    ///
    void startReading( bool& doneFlag )
    {
        doneFlag      = false;
        m_doneFlagPtr = &doneFlag;
        m_myThreadPtr->signal();
    }

    ///
    void terminate()
    {
        m_run = false;
        m_stream.close();
        m_myThreadPtr->signal();
    }

public:
    ///
    void setThreadOfExecution_( Cpl::System::Thread* myThreadPtr ) { m_myThreadPtr = myThreadPtr; }

    ///
    void appRun()
    {
        while ( m_run )
        {
            Cpl::System::Thread::wait();

            for ( ;;)
            {
                Cpl::Text::FString<256> buffer;
                if ( !m_stream.read( buffer ) )
                {
                    CPL_SYSTEM_TRACE_MSG( SECT_, ("READER: Read failed") );
                    break;
                }
                CPL_SYSTEM_TRACE_MSG( SECT_, ("READER: input [%s]", buffer.getString() ) );

                if ( !m_stream.write( buffer ) )
                {
                    CPL_SYSTEM_TRACE_MSG( SECT_, ("READER: Write failed") );
                    break;
                }
                m_stream.flush();
                CPL_SYSTEM_TRACE_MSG( SECT_, ("READER: echoed [%s]", buffer.getString()) );
            }

            CPL_SYSTEM_TRACE_MSG( SECT_, ("READER: Exited Loopback loop -->wait for next 'startReader' (m_run=%d)", m_run) );
            m_stream.close();
            if ( m_doneFlagPtr )
            {
                *m_doneFlagPtr = true;
            }
        }
    }


};

class LoopBackClient : public ListenerClientSync
{
public:
    ///
    bool    m_loop1Free;
    ///
    Reader& m_reader1;
    ///
    bool    m_loop2Free;
    ///
    Reader& m_reader2;


public:
    ///
    LoopBackClient( Cpl::Itc::PostApi& myMbox, Reader& reader1, Reader& reader2 )
        :ListenerClientSync( myMbox ),
        m_loop1Free( true ),
        m_reader1( reader1 ),
        m_loop2Free( true ),
        m_reader2( reader2 )
    {
    }

public:
    /// Request: NewConnection
    void request( NewConnectionMsg& msg )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Incoming connection from: %s (fd=%p)", msg.getPayload().m_rawConnectionInfo, msg.getPayload().m_acceptedFd) );
        if ( m_loop1Free )
        {
            msg.getPayload().m_accepted = true;
            m_reader1.m_stream.activate( msg.getPayload().m_acceptedFd );
            m_reader1.startReading( m_loop1Free );
        }
        else if ( m_loop2Free )
        {
            msg.getPayload().m_accepted = true;
            m_reader2.m_stream.activate( msg.getPayload().m_acceptedFd );
            m_reader2.startReading( m_loop2Free );
        }
        else
        {
            // Note: The default for msg payload is 'm_accpeted:=false'
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Connection rejected") );
        }

        msg.returnToSender();
    }
};

}; // end anonymous namespace


#define TEXT1   "Hello World"
#define TEXT2   "hello world"
#define TEXT3   "Really Long string.................................................................................................................okay maybe not so long"


///////////////////
TEST_CASE( "loopback", "[loopback]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Reader reader1;
    Reader reader2;

    Cpl::Itc::MailboxServer testApplication;
    LoopBackClient          myClient( testApplication, reader1, reader2 );

    Cpl::System::Thread* t1 = Cpl::System::Thread::create( *listenerPtr_, "Listener" );
    Cpl::System::Thread* t2 = Cpl::System::Thread::create( reader1, "Reader1" );
    Cpl::System::Thread* t3 = Cpl::System::Thread::create( reader2, "Reader2" );
    Cpl::System::Thread* t4 = Cpl::System::Thread::create( testApplication, "TestApp" );
    Cpl::System::Api::sleep( 250 ); // Wait to ensure all threads have started

    // Start listener
    listenerPtr_->startListening( myClient, PORT_NUM_ );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Listening on port %d 2min....", PORT_NUM_) );
    Cpl::System::Api::sleep( 50 );

    // Connect
    Cpl::Io::Descriptor clientFd;
    REQUIRE( connectorPtr_->establish( "localhost", PORT_NUM_ + 1, clientFd ) != Connector::eSUCCESS );
    REQUIRE( connectorPtr_->establish( "localhost", PORT_NUM_, clientFd ) == Connector::eSUCCESS );
    InputOutput clientStream( clientFd );
    Cpl::Io::Descriptor client2Fd;
    REQUIRE( connectorPtr_->establish( "localhost", PORT_NUM_, client2Fd ) == Connector::eSUCCESS );
    InputOutput client2Stream( client2Fd );

    // Loop back
    Cpl::Text::FString<512> buffer1;
    REQUIRE( clientStream.write( TEXT1 ) );
    minWaitOnStreamData( clientStream, 10, 1000 );
    REQUIRE( clientStream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT1 );

    Cpl::Text::FString<512> buffer2;
    REQUIRE( client2Stream.write( TEXT2 ) );
    minWaitOnStreamData( client2Stream, 10, 1000 );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT2 );

    REQUIRE( clientStream.write( TEXT3 ) );
    minWaitOnStreamData( clientStream, 10, 1000 );
    REQUIRE( clientStream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT3 );

    REQUIRE( client2Stream.write( TEXT3 ) );
    minWaitOnStreamData( client2Stream, 10, 1000 );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT3 );

    REQUIRE( clientStream.write( TEXT2 ) );
    minWaitOnStreamData( clientStream, 10, 1000 );
    REQUIRE( clientStream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT2 );

    REQUIRE( client2Stream.write( TEXT1 ) );
    minWaitOnStreamData( client2Stream, 10, 1000 );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT1 );

    clientStream.close();

    Cpl::Io::Descriptor client3Fd;
    REQUIRE( connectorPtr_->establish( "localhost", PORT_NUM_, client3Fd ) == Connector::eSUCCESS );
    InputOutput client3Stream( client3Fd );

    REQUIRE( client3Stream.write( TEXT3 ) );
    minWaitOnStreamData( client3Stream, 10, 1000 );
    REQUIRE( client3Stream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT3 );

    REQUIRE( client2Stream.write( TEXT3 ) );
    REQUIRE( minWaitOnStreamData( client2Stream ) );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT3 );

    client2Stream.close();
    client3Stream.close();


    // Clean-up
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Test done.  Cleaning up...") );
    listenerPtr_->terminate();
    reader1.terminate();
    reader2.terminate();
    testApplication.pleaseStop();

    // Allow time for threads to stop
    Cpl::System::Api::sleep( 250 );
    REQUIRE( t1->isRunning() == false );
    REQUIRE( t2->isRunning() == false );
    REQUIRE( t3->isRunning() == false );
    REQUIRE( t4->isRunning() == false );

    Cpl::System::Thread::destroy( *t1 );
    Cpl::System::Thread::destroy( *t2 );
    Cpl::System::Thread::destroy( *t3 );
    Cpl::System::Thread::destroy( *t4 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


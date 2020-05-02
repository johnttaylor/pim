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

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/BareMetal/Thread.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/Dm/Subscriber.h"

using namespace Cpl::Dm;

////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static StaticInfo       info_mp_apple_( "APPLE1" );
static Mp::Uint32       mp_apple_( modelDb_, info_mp_apple_ );

static StaticInfo       info_mp_orange_( "ORANGE1" );
static Mp::Uint32       mp_orange_( modelDb_, info_mp_orange_ );

static StaticInfo       info_mp_cherry_( "CHERRY1" );
static Mp::Uint32       mp_cherry_( modelDb_, info_mp_cherry_ );

static StaticInfo       info_mp_plum_( "PLUM1" );
static Mp::Uint32       mp_plum_( modelDb_, info_mp_plum_ );

class EventFlagProcessing : public EventLoop
{
public:
    ///
    EventFlagProcessing() {};

    ///
    void processEventFlag( uint8_t eventNumber ) noexcept
    {
        static int count = 0;
        count++;
        mp_apple_.write( count );
        mp_orange_.write( count );
        mp_cherry_.write( count );
        mp_plum_.write( count );
    }
};
class TestSubscriber : public Mp::Uint32::Observer
{
public:
    int m_count;
    int m_exitCount;
    Mp::Uint32& m_myMp;
    EventLoop& my_eventLoop;
    bool m_last;

    ///
    TestSubscriber( EventLoop& myEventLoop, int exitCount, Mp::Uint32& myModelPoint, bool last )
        :Mp::Uint32::Observer( myEventLoop )
        , m_count( 0 )
        , m_exitCount( exitCount )
        , m_myMp( myModelPoint )
        , my_eventLoop( myEventLoop )
        , m_last( last )
    {
    }

    /// Type safe change notification.  See Cpl::Dm::SubscriberApi
    void modelPointChanged( Mp::Uint32& modelPointThatChanged ) noexcept
    {
        m_count++;

        if ( m_count == m_exitCount )
        {
            m_myMp.detach( *this );
            if ( m_last )
            {
                Cpl::System::Thread::myRunnable().pleaseStop();
            }
        }

        // Trigger an update to the MP 
        if ( m_last )
        {
            my_eventLoop.notify(1);
        }
    }
};


static EventFlagProcessing eventLoop_;

static TestSubscriber   apple_observer( eventLoop_, 1, mp_apple_,  false );
static TestSubscriber   orange_observer( eventLoop_, 3, mp_orange_,  false );
static TestSubscriber   cherry_observer( eventLoop_, 2, mp_cherry_,  false );
static TestSubscriber   plum_observer( eventLoop_, 4, mp_plum_,  true );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "bare-metal notifications" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "Changes" )
    {

        Cpl::System::Runnable& prev = Cpl::System::BareMetal::Thread::setRunnable( eventLoop_ );

        mp_apple_.attach( apple_observer );
        mp_orange_.attach( orange_observer );
        mp_cherry_.attach( cherry_observer );
        mp_plum_.attach( plum_observer );

        eventLoop_.notify(1);
        eventLoop_.run();
        REQUIRE( apple_observer.m_count == 1 );
        REQUIRE( orange_observer.m_count == 3 );
        REQUIRE( cherry_observer.m_count == 2 );
        REQUIRE( plum_observer.m_count == 4 );

        Cpl::System::BareMetal::Thread::setRunnable( prev );
    }


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


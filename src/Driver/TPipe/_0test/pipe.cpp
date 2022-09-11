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
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Driver/TPipe/Maker.h"
#include "Driver/TPipe/RxFrameHandler.h"
#include "Cpl/Dm/PeriodicScheduler.h"
#include "Cpl/Text/FString.h"
#include <string.h>

using namespace Driver::TPipe;

///////////////////////////////////////////////////////////////////////////////
// Receive handlers
class MyRxHandler : public RxFrameHandler
{
public:
    int m_rxCount;

public:
    MyRxHandler( Cpl::Container::Map<RxFrameHandlerApi>& handlerList, const char* verb ) noexcept
        : RxFrameHandler( handlerList, verb )
        , m_rxCount( 0 )
    {
    }

public:
    void execute( char* decodedFrameText ) noexcept
    {
        m_rxCount++;

        const char* myVerb = getVerb();
        REQUIRE( strncmp( decodedFrameText, myVerb, strlen( myVerb ) ) == 0 );
    }
};

static Cpl::Container::Map<Driver::TPipe::RxFrameHandlerApi> frameHandlerList_( "ignoreThisParameter_usedToSelecStaticContructor" );
static MyRxHandler rxApple_( frameHandlerList_, "apple" );
static MyRxHandler rxOrange_( frameHandlerList_, "orange" );
static MyRxHandler rxCherry_( frameHandlerList_, "cherry" );


////////////////////////////////////////////////////////////////////////////////
// Create the periodic scheduler that the UUT to executes in

static Cpl::System::PeriodicScheduler::Interval_T intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

static Pipe* uutPtr_;
static Cpl::Io::Input*  testInfdPtr_;
static Cpl::Io::Output* testOutfdPtr_;
static void tpipePoll( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    REQUIRE( uutPtr_ );
    uutPtr_->poll();
}

static void tpipeStart( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    REQUIRE( uutPtr_ );
    REQUIRE( testInfdPtr_ );
    REQUIRE( testOutfdPtr_ );
    uutPtr_->start( *testInfdPtr_, *testOutfdPtr_ );
}

static void tpipeEnd( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    REQUIRE( uutPtr_ );
    uutPtr_->stop();
}

static Cpl::Dm::PeriodicScheduler t1Mbox_( intervals_, 
                                           tpipeStart, 
                                           tpipeEnd, 
                                           nullptr, 
                                           Cpl::System::ElapsedTime::precision, 
                                           tpipePoll );

#include "Cpl/Io/Ram/InputOutput.h"

////////////////////////////////////////////////////////////////////////////////
#define TX_TEXT_1_RAW       "hello world"
#define TX_TEXT_1_FRAMED    "^" TX_TEXT_1_RAW ";"
#define TX_TEXT_2_RAW       "hello`;world"
#define TX_TEXT_2_FRAMED    "^hello```;world;"

#define MAX_FRAME_SIZE      20

static uint8_t inStreamMemory_[MAX_FRAME_SIZE * 2];
static uint8_t outStreamMemory_[MAX_FRAME_SIZE * 2];

TEST_CASE( "pipe" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Create the UUT
    Maker uutMaker( frameHandlerList_, MAX_FRAME_SIZE );
    uutPtr_ = &uutMaker.getPipeProcessor();

    // Create my streams
    Cpl::Io::Ram::InputOutput inStream( inStreamMemory_, sizeof( inStreamMemory_ ) );
    Cpl::Io::Ram::InputOutput outStream( outStreamMemory_, sizeof( outStreamMemory_ ) );
    testInfdPtr_  = &inStream;
    testOutfdPtr_ = &outStream;

    // Start my test thread
    Cpl::System::Thread* t1 = Cpl::System::Thread::create( t1Mbox_, "T1" );
    Cpl::System::Api::sleep( 300 ); // Allow time for the thread to spin up and initialize the uut.


    // Transmit test
    uutPtr_->sendCommand( TX_TEXT_1_RAW, strlen( TX_TEXT_1_RAW ) );
    Cpl::Text::FString<MAX_FRAME_SIZE> buf;
    REQUIRE( outStream.read( buf ) );
    REQUIRE( buf == TX_TEXT_1_FRAMED );
    uutPtr_->sendCommand( TX_TEXT_2_RAW, strlen( TX_TEXT_2_RAW ) );
    REQUIRE( outStream.read( buf ) );
    REQUIRE( buf == TX_TEXT_2_FRAMED );
    uutPtr_->sendRawCommand( TX_TEXT_2_RAW, strlen( TX_TEXT_2_RAW ) );
    REQUIRE( outStream.read( buf ) );
    REQUIRE( buf == TX_TEXT_2_RAW );

    // Receive tests
    REQUIRE( inStream.write( "apple was here" ) );
    Cpl::System::Api::sleep( 300 ); 
    REQUIRE( rxApple_.m_rxCount == 0 );
    REQUIRE( inStream.write( "^apple was here" ) );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( rxApple_.m_rxCount == 0 );
    REQUIRE( inStream.write( ";" ) );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( rxApple_.m_rxCount == 1 );
    
    REQUIRE( uutPtr_->getUnknownFrameCount() == 0 );
    REQUIRE( inStream.write( "^apple was here;^orange was too;^cherry went along;^plum was left home;" ) );
    Cpl::System::Api::sleep( 600 );
    REQUIRE( rxApple_.m_rxCount == 2 );
    REQUIRE( rxOrange_.m_rxCount == 1 );
    REQUIRE( rxCherry_.m_rxCount == 1 );
    REQUIRE( uutPtr_->getUnknownFrameCount() == 1 );

    REQUIRE( inStream.write( "^apple-was here;" ) );
    Cpl::System::Api::sleep( 300 );
    REQUIRE( rxApple_.m_rxCount == 2 );
    REQUIRE( uutPtr_->getUnknownFrameCount() == 2 );

    // Shutdown threads
    t1Mbox_.pleaseStop();
    Cpl::System::Api::sleep( 100 ); // allow time for threads to stop
    REQUIRE( t1->isRunning() == false );
    Cpl::System::Thread::destroy( *t1 );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
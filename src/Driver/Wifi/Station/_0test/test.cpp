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

#include "test.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Itc/PeriodicScheduler.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/Io/Tcp/InputOutput.h"
#include "Cpl/TShell/PolledMaker.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/TShell/Cmd/Threads.h"
#include "Driver/Wifi/Station/Connection.h"
#include "Driver/Wifi/Station/TShell/Wifi.h"

#define SECT_   "_0test"

#ifndef OPTION_MAGIC_QUIT_PHRASE
#define OPTION_MAGIC_QUIT_PHRASE        "q!"
#endif

using namespace Cpl::Io::Tcp;

class ListenerClient : public AsyncListener::Client
{
public:
    bool   m_connected;
    size_t m_totalReadBytes;
public:
    ListenerClient()
        :m_connected( false )
    {
    }

public:
    bool newConnection( Cpl::Io::Descriptor newFd, const char* rawConnectionInfo ) noexcept
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Accepted incoming connection. Remote Host: %s", rawConnectionInfo) );
        activate( newFd );
        m_connected      = true;
        m_totalReadBytes = 0;
        return true;
    }

public:
    void consumeInput()
    {
        if ( m_connected )
        {
            int  bytesRead;
            char inBuf[128];
            if ( available() )
            {
                if ( read( inBuf, sizeof( inBuf ), bytesRead ) )
                {
                    m_totalReadBytes += bytesRead;
                    if ( bytesRead > 0 )
                    {
                        CPL_SYSTEM_TRACE_MSG( SECT_, ("LIST: Bytes in: %d [%.*s]", bytesRead, bytesRead, inBuf) );
                        int bytesWritten;
                        if ( write( inBuf, bytesRead, bytesWritten ) )
                        {
                            CPL_SYSTEM_TRACE_MSG( SECT_, ("LIST:   echoed: %d", bytesWritten) );
                        }
                        else
                        {
                            CPL_SYSTEM_TRACE_MSG( SECT_, ("LIST: WRITE FAILED") );
                            m_connected = false;
                        }

                        if ( strncmp( inBuf, OPTION_MAGIC_QUIT_PHRASE, strlen( OPTION_MAGIC_QUIT_PHRASE ) ) == 0 )
                        {
                            CPL_SYSTEM_TRACE_MSG( SECT_, ("LIST: Received QUIT command -->closing the stream") );
                            m_connected = false;
                            close();
                        }
                    }

                    CPL_SYSTEM_TRACE_MSG( SECT_, ("LIST: Bytes in: %d (%d)", bytesRead, m_totalReadBytes) );
                }
                else
                {
                    CPL_SYSTEM_TRACE_MSG( SECT_, ("LIST: READ FAILED") );
                    m_connected = false;
                }
            }
        }
    }
};


//////////////////////////////////////////////
// Create the Command/Debug Console and populate with commands
static Cpl::Container::Map<Cpl::TShell::Command>    cmdlist_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static Cpl::TShell::PolledMaker                     cmdProcessor_( cmdlist_ );
static Cpl::TShell::Cmd::Help	                    helpCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Bye	                    byeCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Trace	                    traceCmd_( cmdlist_ );
static Cpl::TShell::Cmd::TPrint	                    tprintCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Threads                    threadsCmd_( cmdlist_ );
static Driver::Wifi::Station::TShell::Wifi          wifiCmd_( cmdlist_ );

static int                           portNum_;
static Cpl::Io::Tcp::AsyncListener*  listenerPtr_;
static ListenerClient                listenClient_;
static Cpl::Io::Input*               consoleInputFdPtr_;
static Cpl::Io::Output*              consoleOutputFdPtr_;



static void idleTime_( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    listenerPtr_->poll();
    listenClient_.consumeInput();
    Driver::Wifi::Station::Connection::poll();
    cmdProcessor_.getCommandProcessor().poll();

    static bool connected = false;
    if ( Driver::Wifi::Station::Connection::isConnected() && connected == false )
    {
        connected = true;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Connected to the WIFI network") );
    }
    else if ( Driver::Wifi::Station::Connection::isConnected() == false && connected == true )
    {
        // I have to brute force close the input-stream because it does
        // not detect the network is down till it transmits - and since it
        // never transmit until it receives data -->we have to 'manually' close
        // the socket
        listenClient_.close();

        connected = false;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("WIFI network disconnected") );
    }
}

static Cpl::System::PeriodicScheduler::Interval_T intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS // Create an empty set of intervals - actual intervals not used for the test
};

static void beginThreadProcessing( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    listenerPtr_->startListening( listenClient_, portNum_ );
    cmdProcessor_.getCommandProcessor().start( *consoleInputFdPtr_, *consoleOutputFdPtr_ ); // Note: I don't need to set the 'blocking flag' because the processor knows it is non-blocking processor
}
static void endThreadProcssing( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    listenerPtr_->terminate();
}


static Cpl::Itc::PeriodicScheduler scheduler_( intervals_,
                                               beginThreadProcessing, endThreadProcssing,
                                               nullptr,
                                               Cpl::System::ElapsedTime::precision,
                                               idleTime_ );

////////////////////////////////////////////////////////////////////////////////

int runTest( Cpl::Io::Input&              consoleInputFd,
             Cpl::Io::Output&             consoleOutputFd,
             Cpl::Io::Tcp::AsyncListener& listener,
             int                          portNum )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Enter TCP test: Echo Server") );

    // Housekeeping
    portNum_            = portNum;
    listenerPtr_        = &listener;
    consoleInputFdPtr_  = &consoleInputFd;
    consoleOutputFdPtr_ = &consoleOutputFd;

    // Initialize the Station WIFI connection
    Driver::Wifi::Station::Connection::initiailize();

    // Create thread for the test to run in
    Cpl::System::Thread::create( scheduler_, "WIFI" );

    // Start scheduling
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Scheduling Enabled!") );

    // The test assumes that enableScheduling() never returns -->so make sure we don't return
    for ( ;;)
    {
        Cpl::System::Api::sleep( 1000 );
    }
}



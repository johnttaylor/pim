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

#include "server.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Itc/PeriodicScheduler.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/Io/Tcp/InputOutput.h"

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
static int                           portNum_;
static Cpl::Io::Tcp::AsyncListener*  listenerPtr_;
static ListenerClient                listenClient_;


static void tcpScan_( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    listenerPtr_->poll();
    listenClient_.consumeInput();
}

static Cpl::System::PeriodicScheduler::Interval_T intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS // Create an empty set of intervals - actual intervals not used for the test
};

static void beginThreadProcessing( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    listenerPtr_->startListening( listenClient_, portNum_ );
}
static void endThreadProcssing( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    listenerPtr_->terminate();
}


static Cpl::Itc::PeriodicScheduler scheduler_( intervals_,
                                               beginThreadProcessing, endThreadProcssing,
                                               nullptr,
                                               Cpl::System::ElapsedTime::precision,
                                               tcpScan_ );


////////////////////////////////////////////////////////////////////////////////

int runTest( Cpl::Io::Tcp::AsyncListener& listener, int portNum )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Enter TCP test: Echo Server") );

    // Cache the UUTs
    portNum_      = portNum;
    listenerPtr_  = &listener;

    // Create thread for the test to run in
    Cpl::System::Thread::create( scheduler_, "TEST" );

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



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
#include "Cpl/Text/FString.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Io/AtomicOutput.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"


#define SECT_     "_0test"
/// 
using namespace Cpl::Io;



////////////////////////////////////////////////////////////////////////////////
namespace {

class MyRunnable : public Cpl::System::Runnable
{
public:
    ///
    const char*                 m_msg;
    ///
    Cpl::System::Thread&        m_masterThread;
    ///
    int                         m_maxLoops;
    ///
    unsigned long               m_delay;
    ///
    AtomicOutput<MyRunnable>    m_out;

public:
    ///
    MyRunnable( Cpl::System::Thread& masterThread, Output& fd, Cpl::System::Mutex& lock, int maxLoops, unsigned long delay, const char* msg )
        :m_msg( msg ),
        m_masterThread( masterThread ),
        m_maxLoops( maxLoops ),
        m_delay( delay ),
        m_out( fd, lock )
    {
    }

public:
    /// 
    bool many( Output& fd )
    {
        Cpl::Text::FString<256> buffer;
        bool         io = false;

        if ( fd.write( buffer, "1: %s", m_msg ) )
        {
            if ( fd.write( buffer, "2: %s", m_msg ) )
            {
                if ( fd.write( buffer, "3: %s", m_msg ) )
                {
                    io = true;
                }
            }
        }

        return io;
    }

    ///
    void appRun()
    {
        Cpl::Text::FString<256> buffer;

        // Wait till I'm told to start    
        Cpl::System::Thread::wait();

        int i;
        for ( i=0; i < m_maxLoops; i++ )
        {
            m_out.write( buffer, "** %s", m_msg );
            m_out.requestOutputs( *this, &MyRunnable::many );
            Cpl::System::Api::sleep( m_delay );
        }

        m_masterThread.signal();
    }

};

}; // end namespace

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "atomic", "[atomic]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Cpl::Io::Stdio::StdOut fd;
    Cpl::System::Mutex     lock;

    MyRunnable appleRun( Cpl::System::Thread::getCurrent(), fd, lock, 10, 3, "Apple:  very long string for more chance of interleaving 123456789 122456789 abcdef\n" );
    MyRunnable orangeRun( Cpl::System::Thread::getCurrent(), fd, lock, 5, 7, "Orange: very long string for more chance of interleaving 123456789 122456789 abcdef\n" );
    MyRunnable cherryRun( Cpl::System::Thread::getCurrent(), fd, lock, 13, 2, "Cherry: very long string for more chance of interleaving 123456789 122456789 abcdef\n" );

    Cpl::System::Thread* appleThreadPtr   = Cpl::System::Thread::create( appleRun, "APPLE" );
    Cpl::System::Thread* orangeThreadPtr  = Cpl::System::Thread::create( orangeRun, "ORANGE" );
    Cpl::System::Thread* cherryThreadPtr  = Cpl::System::Thread::create( cherryRun, "CHERRY" );

    // Wait till all threads have spun up
    Cpl::System::Api::sleep( 100 );

    // Start all threads running
    orangeThreadPtr->signal();
    appleThreadPtr->signal();
    cherryThreadPtr->signal();

    // Wait for all threads to finish
    Cpl::System::Thread::wait();
    Cpl::System::Thread::wait();
    Cpl::System::Thread::wait();

    // Destroy all threads
    Cpl::System::Thread::destroy( *orangeThreadPtr );
    Cpl::System::Thread::destroy( *appleThreadPtr );
    Cpl::System::Thread::destroy( *cherryThreadPtr );


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

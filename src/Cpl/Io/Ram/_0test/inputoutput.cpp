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
#include "Cpl/Io/Ram/InputOutput.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FString.h"



#define SECT_     "_0test"

/// 
using namespace Cpl::Io::Ram;



///////////////////
namespace {

/// 
class Writer : public Cpl::System::Runnable
{
public:
    bool		 m_success;
    int          m_writeOperations;
    const char*	 m_stringToWrite;
    bool         m_run;
    bool*        m_doneFlagPtr;
    Cpl::System::Thread*  m_myThreadPtr;
    Cpl::Io::InputOutput& m_stream;

public:
    ///
    Writer( Cpl::Io::InputOutput& fd, const char* writeString ) :m_writeOperations( 0 ), m_stringToWrite( writeString ), m_run( true ), m_doneFlagPtr( 0 ), m_myThreadPtr( 0 ), m_stream( fd ) {}


public:
    ///
    void startWriting( bool& doneFlag )
    {
        doneFlag      = false;
        m_doneFlagPtr = &doneFlag;
        m_myThreadPtr->signal();
    }
    /// 
    void terminate()
    {
        m_run = false;
        m_myThreadPtr->signal();
    }

public:
    ///
    void setThreadOfExecution_( Cpl::System::Thread* myThreadPtr ) { m_myThreadPtr = myThreadPtr; }

    /// 
    void appRun()
    {
        m_success = true;
        while ( m_run && m_success )
        {
            Cpl::System::Thread::wait();
            if ( m_run && m_success )   // check again
            {
                m_writeOperations++;
                m_success = m_stream.write( m_stringToWrite );
                if ( m_doneFlagPtr )
                {
                    *m_doneFlagPtr = true;
                }
            }
        }
    }
};

/// 
class Reader : public Cpl::System::Runnable
{
public:
    bool		 m_success;
    int          m_readOperations;
    char*		 m_dstBuffer;
    unsigned	 m_bytesToRead;
    unsigned     m_bytesRead;
    bool         m_run;
    bool*        m_doneFlagPtr;
    Cpl::System::Thread*  m_myThreadPtr;
    Cpl::Io::InputOutput& m_stream;

public:
    ///
    Reader( Cpl::Io::InputOutput& fd, char* dst, unsigned numBytesToRead ) :m_readOperations( 0 ), m_dstBuffer( dst ), m_bytesToRead( numBytesToRead ), m_run( true ), m_doneFlagPtr( 0 ), m_myThreadPtr( 0 ), m_stream( fd ) {}


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
        m_myThreadPtr->signal();
    }
public:
    ///
    void setThreadOfExecution_( Cpl::System::Thread* myThreadPtr ) { m_myThreadPtr = myThreadPtr; }

    ///
    void appRun()
    {
        m_success  = true;
        while ( m_run && m_success )
        {
            Cpl::System::Thread::wait();
            if ( m_run && m_success )   // check again
            {
                m_bytesRead        = 0;
                int bytesRemaining = m_bytesToRead;
                int bytesRead;
                char buf[100] ={ 0, };
                memset( m_dstBuffer, 0, m_bytesToRead + 1 );
                m_readOperations++;

                while ( bytesRemaining )
                {
                    if ( !m_stream.read( buf, bytesRemaining, bytesRead ) )
                    {
                        m_success = false;
                        break;
                    }
                    strncat( m_dstBuffer, buf, bytesRead );
                    m_bytesRead    += bytesRead;
                    bytesRemaining -= bytesRead;
                }

                if ( m_doneFlagPtr )
                {
                    *m_doneFlagPtr = true;
                }
            }
        }
    }
};

};	// end anonymous namespace

////////////////////////////////////////////////////////////////////////////////

#define BUFFER_SIZE 16

#define TEST_STRING1    "hello world"
#define TEST_STRING2	"I am doing fine"
#define TEST_STRING3    "That's great"

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "ram" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Cpl::Text::FString<BUFFER_SIZE> tmpString;
    uint8_t memory[BUFFER_SIZE];
    InputOutput uut( memory, BUFFER_SIZE );

    SECTION( "single thread" )
    {
        bool result = uut.available();
        REQUIRE( result == false );
        result = uut.isEos();
        REQUIRE( result == false );

        result = uut.write( TEST_STRING1 );
        REQUIRE( result == true );
        result = uut.available();
        REQUIRE( result == true );
        result = uut.read( tmpString );
        REQUIRE( result == true );

        uut.close();
        result = uut.available();
        REQUIRE( result == false );
        result = uut.read( tmpString );
        REQUIRE( result == false );
        result = uut.write( TEST_STRING1 );
        REQUIRE( result == false );
        result = uut.isEos();
        REQUIRE( result == true );
    }

    SECTION( "single writer, many readers" )
    {
        Writer writer( uut, TEST_STRING2 );
        char reader1Buf[100];
        char reader2Buf[100];
        char reader3Buf[100];
        Reader reader1( uut, reader1Buf, strlen( TEST_STRING2 ) );
        Reader reader2( uut, reader2Buf, strlen( TEST_STRING2 ) );
        Reader reader3( uut, reader3Buf, strlen( TEST_STRING2 ) );

        Cpl::System::Thread* w1 = Cpl::System::Thread::create( writer, "Writer" );
        Cpl::System::Thread* r1 = Cpl::System::Thread::create( reader1, "Reader1" );
        Cpl::System::Thread* r2 = Cpl::System::Thread::create( reader2, "Reader2" );
        Cpl::System::Thread* r3 = Cpl::System::Thread::create( reader3, "Reader3" );
        Cpl::System::Api::sleep( 500 ); // Wait to ensure all threads have started

        // Start my readers
        bool reader1Done;
        bool reader2Done;
        bool reader3Done;
        reader1.startReading( reader1Done );
        reader2.startReading( reader2Done );
        reader3.startReading( reader3Done );
        Cpl::System::Api::sleep( 500 ); // Wait to ensure all reader loops have started
        REQUIRE( reader1Done == false );
        REQUIRE( reader2Done == false );
        REQUIRE( reader3Done == false );

        // Start my write
        bool writerDone;
        writer.startWriting( writerDone );
        Cpl::System::Api::sleep( 500 ); // Wait to ensure write is done
        REQUIRE( writerDone == true );
        REQUIRE( writer.m_success == true );
        REQUIRE( writer.m_writeOperations == 1 );

        // Did I read the data?
        REQUIRE( reader1.m_bytesRead + reader2.m_bytesRead + reader3.m_bytesRead == strlen( TEST_STRING2 ) );
        REQUIRE( reader1.m_readOperations == 1 );
        REQUIRE( reader2.m_readOperations == 1 );
        REQUIRE( reader3.m_readOperations == 1 );

        // Shutdown the threads
        writer.terminate();
        reader1.terminate();
        reader2.terminate();
        reader3.terminate();
        uut.close(); // This will unblock all clients

        // Allow time for threads to stop
        Cpl::System::Api::sleep( 500 );
        REQUIRE( w1->isRunning() == false );
        REQUIRE( r1->isRunning() == false );
        REQUIRE( r2->isRunning() == false );
        REQUIRE( r3->isRunning() == false );

        Cpl::System::Thread::destroy( *w1 );
        Cpl::System::Thread::destroy( *r1 );
        Cpl::System::Thread::destroy( *r2 );
        Cpl::System::Thread::destroy( *r3 );
    }

    SECTION( "single reader, many writers" )
    {
        char reader1Buf[100];
        Reader reader( uut, reader1Buf, strlen( TEST_STRING3 ) * 3 );
        Writer writer1( uut, TEST_STRING3 );
        Writer writer2( uut, TEST_STRING3 );
        Writer writer3( uut, TEST_STRING3 );

        Cpl::System::Thread* r1 = Cpl::System::Thread::create( reader, "Reader" );
        Cpl::System::Thread* w1 = Cpl::System::Thread::create( writer1, "Writer1" );
        Cpl::System::Thread* w2 = Cpl::System::Thread::create( writer2, "Writer2" );
        Cpl::System::Thread* w3 = Cpl::System::Thread::create( writer3, "Writer3" );
        Cpl::System::Api::sleep( 500 ); // Wait to ensure all threads have started


        // start my reader
        bool readerDone;
        reader.startReading( readerDone );

        // Start my writers
        bool writer1Done;
        bool writer2Done;
        bool writer3Done;
        writer1.startWriting( writer1Done );
        writer2.startWriting( writer2Done );
        writer3.startWriting( writer3Done );
        Cpl::System::Api::sleep( 500 ); // Wait to ensure all writer have completed
        REQUIRE( writer1Done == true );
        REQUIRE( writer2Done == true );
        REQUIRE( writer3Done == true );
        REQUIRE( writer1.m_success == true );
        REQUIRE( writer2.m_success == true );
        REQUIRE( writer3.m_success == true );
        REQUIRE( writer1.m_writeOperations == 1 );
        REQUIRE( writer2.m_writeOperations == 1 );
        REQUIRE( writer3.m_writeOperations == 1 );


        // Did I read the data?
        REQUIRE( reader.m_bytesRead == strlen( TEST_STRING3 ) * 3 );
        REQUIRE( reader.m_readOperations >= 1 );

        // Shutdown the threads
        reader.terminate();
        writer1.terminate();
        writer2.terminate();
        writer3.terminate();
        uut.close(); // This will unblock all clients

        // Allow time for threads to stop
        Cpl::System::Api::sleep( 500 );
        REQUIRE( r1->isRunning() == false );
        REQUIRE( w1->isRunning() == false );
        REQUIRE( w2->isRunning() == false );
        REQUIRE( w3->isRunning() == false );

        Cpl::System::Thread::destroy( *r1 );
        Cpl::System::Thread::destroy( *w1 );
        Cpl::System::Thread::destroy( *w2 );
        Cpl::System::Thread::destroy( *w3 );
    }

    uut.close();
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


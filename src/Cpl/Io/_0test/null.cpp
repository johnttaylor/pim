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
#include "Cpl/Io/Null.h"
#include "Cpl/Io/LineWriter.h"
#include "Cpl/Io/LineReader.h"
#include "Cpl/Io/AtomicOutput.h"
#include "Cpl/Io/TeeOutput.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Container/SList.h"



#define SECT_     "_0test"

/// 
using namespace Cpl::Io;


////////////////////////////////////////////////////////////////////////////////
/// Use anonymous namespace to make my class local-to-the-file in scope
namespace {

class MyContext
{
public:
    int m_count;

public:
    MyContext():m_count( 0 ) {}

public:
    bool testOutputs( Output& fd )
    {
        m_count++;
        return fd.write( "Hello" );
        return fd.write( " World" );
    }
};
}; // end namespace

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "null", "[null]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //     
    Null fd;
    char dummyChar = 29;

    REQUIRE( fd.read( dummyChar ) == false );
    REQUIRE( dummyChar == 29 );

    Cpl::Text::FString<10> buffer( "bob" );
    REQUIRE( fd.read( buffer ) == false );
    REQUIRE( buffer == "" );

    int myBuffer[10] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 };
    int bytesRead    = 1;
    REQUIRE( fd.read( myBuffer, sizeof( myBuffer ), bytesRead ) == false );
    REQUIRE( bytesRead == 0 );
    REQUIRE( fd.available() == false );

    //
    int bytesWritten;
    REQUIRE( fd.write( 'a' ) );
    REQUIRE( fd.write( "bob's your uncle" ) );
    REQUIRE( fd.write( buffer ) );
    REQUIRE( fd.write( buffer, "Hello %s", "World" ) );
    REQUIRE( buffer == "Hello Worl" );
    REQUIRE( fd.write( myBuffer, sizeof( myBuffer ) ) );
    REQUIRE( fd.write( myBuffer, sizeof( myBuffer ), bytesWritten ) );
    REQUIRE( (size_t) bytesWritten == sizeof( myBuffer ) );
    fd.flush();
    fd.close();
    REQUIRE( fd.write( buffer ) == false ); // Fails because close() was called!


    // Ensure the diamond inheritance/Container Item stuff works
    Null apple;
    Null orange;
    Null cherry;

    CPL_SYSTEM_TRACE_MSG( SECT_, ("apple=%p,  inPtr=%p, outPtr=%p, inOutPtr=%p", &apple, (Input*) &apple, (Output*) &apple, (InputOutput*) &apple) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("orange=%p, inPtr=%p, outPtr=%p, inOutPtr=%p", &orange, (Input*) &orange, (Output*) &orange, (InputOutput*) &orange) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("cherry=%p, inPtr=%p, outPtr=%p, inOutPtr=%p", &cherry, (Input*) &cherry, (Output*) &cherry, (InputOutput*) &cherry) );

    Cpl::Container::SList<Input>        inlist;
    Cpl::Container::SList<Output>       outlist;
    Cpl::Container::SList<InputOutput>  iolist;

    inlist.put( apple );
    outlist.put( orange );
    iolist.put( cherry );

    // Should generate fatal errors
    inlist.put( cherry );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );
    outlist.put( apple );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );
    iolist.put( orange );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );

    // Empty the list
    REQUIRE( inlist.get() != 0 );
    REQUIRE( inlist.get() == 0 );
    REQUIRE( outlist.get() != 0 );
    REQUIRE( outlist.get() == 0 );
    REQUIRE( iolist.get() != 0 );
    REQUIRE( iolist.get() == 0 );

    // Try different lists
    iolist.put( apple );
    inlist.put( orange );
    outlist.put( cherry );

    // Empty the list
    REQUIRE( inlist.get() != 0 );
    REQUIRE( inlist.get() == 0 );
    REQUIRE( outlist.get() != 0 );
    REQUIRE( outlist.get() == 0 );
    REQUIRE( iolist.get() != 0 );
    REQUIRE( iolist.get() == 0 );


    // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
    Null fd2;
    LineWriter writer( fd2 );
    REQUIRE( writer.println( "Hello World" ) );
    LineReader reader( fd2 );
    REQUIRE( reader.readln( buffer ) == false );

    // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
    Cpl::System::Mutex      mylock;
    MyContext               testContext;
    AtomicOutput<MyContext> atomicfd( fd2, mylock );
    REQUIRE( atomicfd.write( "hello world" ) );
    REQUIRE( atomicfd.requestOutputs( testContext, &MyContext::testOutputs ) );
    REQUIRE( testContext.m_count == 1 );
    REQUIRE( atomicfd.requestOutputs( testContext, &MyContext::testOutputs ) );
    REQUIRE( testContext.m_count == 2 );

    // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
    TeeOutput many( apple, cherry );
    REQUIRE( many.write( "[Hello (apple, cherry)]" ) );
    many.add( orange );
    REQUIRE( many.write( "[World! (apple, cherry, orange)]" ) );
    REQUIRE( many.remove( apple ) );
    REQUIRE( many.write( "[Goodbye! (cherry, orange)]" ) );
    REQUIRE( many.remove( apple ) == false );
    REQUIRE( many.remove( orange ) );
    REQUIRE( many.remove( cherry ) );
    REQUIRE( many.remove( fd ) == false );
    REQUIRE( many.write( "[One more try! (none)]" ) );
    many.close();
    REQUIRE( many.write( "[Should fail!]" ) == false );


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "close", "[close]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    //     
    Null fd;
    char dummyChar = 29;

    fd.close();
    REQUIRE( fd.read( dummyChar ) == false );
    REQUIRE( dummyChar == 29 );

    //
    REQUIRE( fd.write( 'a' ) == false );

    // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
    Null fd2;
    fd2.close();
    LineWriter writer( fd2 );
    REQUIRE( writer.println( "Hello World" ) == false );
    LineReader reader( fd2 );
    Cpl::Text::FString<10> buffer( "bob" );
    REQUIRE( reader.readln( buffer ) == false );

    // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
    Cpl::System::Mutex      mylock;
    MyContext               testContext;
    AtomicOutput<MyContext> atomicfd( fd2, mylock );
    REQUIRE( atomicfd.write( "hello world" ) == false );
    REQUIRE( atomicfd.requestOutputs( testContext, &MyContext::testOutputs ) == false );
    REQUIRE( testContext.m_count == 1 );
    REQUIRE( atomicfd.requestOutputs( testContext, &MyContext::testOutputs ) == false );
    REQUIRE( testContext.m_count == 2 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "TeeOutput", "[TeeOutput]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "basic" )
    {
        // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
        Null apple;
        Null cherry;
        Null orange;
        apple.close();
        cherry.close();
        TeeOutput many( apple, cherry );
        many.add( orange );
        REQUIRE( many.write( "[World! (apple, cherry, orange)]" ) == false );
        REQUIRE( many.firstFailed() == &apple );
        REQUIRE( many.nextFailed( apple ) == &cherry );
        REQUIRE( many.remove( apple ) );
        REQUIRE( many.write( "[Goodbye! (cherry, orange)]" ) == true );
        REQUIRE( many.remove( cherry ) );
        REQUIRE( many.firstFailed() == 0 );
        REQUIRE( many.write( "[One more try! (none)]" ) == true );
        many.close();
        REQUIRE( many.write( "[Should fail!]" ) == false );
    }

    SECTION( "basic2" )
    {
        // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
        Null apple;
        Null cherry;
        Null orange;
        apple.close();
        cherry.close();
        TeeOutput many( apple, cherry );
        many.add( orange );
        REQUIRE( many.write( "[World! (apple, cherry, orange)]" ) == false );
        many.flush();
        REQUIRE( many.removeAndGetNextFailed(apple) == &cherry );
        REQUIRE( many.write( "[Goodbye! (cherry, orange)]" ) == true );
        many.close();
        REQUIRE( many.write( "[Should fail!]" ) == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

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
#include "Driver/DIO/Out.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Bool.h"


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static Cpl::Dm::Mp::Bool         mp_out_( modelDb_, "out" );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Out" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    bool state;
    mp_out_.setInvalid();

    SECTION( "nominal" )
    {
        Driver::DIO::Out uut( mp_out_ );
        REQUIRE( mp_out_.isNotValid() );
        REQUIRE( uut.start( true ) );
        REQUIRE( uut.getOutput() == true );
        REQUIRE( mp_out_.read( state ) );
        REQUIRE( state == true );
        uut.assertOutput();
        REQUIRE( uut.getOutput() == true );
        REQUIRE( mp_out_.read( state ) );
        REQUIRE( state == true );
        uut.deassertOutput();
        REQUIRE( uut.getOutput() == false );
        REQUIRE( mp_out_.read( state ) );
        REQUIRE( state == false );
        uut.stop();
        REQUIRE( mp_out_.isNotValid() );
    }

    SECTION( "inverted" )
    {
        Driver::DIO::Out uut( mp_out_, false );
        REQUIRE( mp_out_.isNotValid() );
        REQUIRE( uut.start( true ) );
        REQUIRE( uut.getOutput() == true );
        REQUIRE( mp_out_.read( state ) );
        REQUIRE( state == false );
        uut.assertOutput();
        REQUIRE( uut.getOutput() == true );
        REQUIRE( mp_out_.read( state ) );
        REQUIRE( state == false );
        uut.deassertOutput();
        REQUIRE( uut.getOutput() == false );
        REQUIRE( mp_out_.read( state ) );
        REQUIRE( state == true );
        uut.stop();
        REQUIRE( mp_out_.isNotValid() );
    }

    SECTION( "errors" )
    {
        Driver::DIO::Out uut( mp_out_ );
        mp_out_.write( 1 );  // Set MP valid
        uut.stop();
        REQUIRE( mp_out_.isNotValid() == false ); // If the stop() call worked - the MP would be invalid

        REQUIRE( uut.start( false ) );
        REQUIRE( mp_out_.read( state ) );
        REQUIRE( state == false );
        REQUIRE( uut.start( true ) == false );
        REQUIRE( mp_out_.read( state ) );
        REQUIRE( state == false );

        uut.assertOutput();
        REQUIRE( uut.getOutput() == true );
        REQUIRE( mp_out_.read( state ) );
        REQUIRE( state == true );
        uut.stop();
        REQUIRE( uut.getOutput() == false );    // Returns false when the driver is not started
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

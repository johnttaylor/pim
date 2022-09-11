/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "common.h"
#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Storm/Dm/MpFileNameString.h"
#include "Cpl/Itc/CloseSync.h"
#include <string.h>

#define STRCMP(s1,s2)       (strcmp(s1,s2)==0)
#define MAX_STR_LENG        1024
#define SECT_               "_0test"

#define MY_UUT_DATA_SIZE    OPTION_STORM_DM_MAX_FILE_NAME

#define INITIAL_VALUE       "Hello World"

using namespace Storm::Dm;


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static MpFileNameString       mp_apple_( modelDb_, "APPLE" );
static MpFileNameString       mp_orange_( modelDb_, "ORANGE" );

// Don't let the Runnable object go out of scope before its thread has actually terminated!
static Cpl::Dm::MailboxServer         t1Mbox_;



////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "MpFileNameString" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "gets" )
    {
        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == MY_UUT_DATA_SIZE + 1 );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == MY_UUT_DATA_SIZE + 1 + sizeof( bool ) );

        const char* mpType = mp_apple_.getTypeAsText();
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "typeText: [%s]", mpType ) );
        REQUIRE( strcmp( mpType, "Storm::Dm::MpFileNameString" ) == 0 );
    }

    SECTION( "read/write" )
    {
        Cpl::Text::FString<128> valString;
        mp_apple_.write( "bob" );
        REQUIRE( mp_apple_.read( valString ) );
        REQUIRE( strcmp( valString, "bob" ) == 0 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

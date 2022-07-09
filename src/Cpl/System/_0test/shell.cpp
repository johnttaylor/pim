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
#include "Cpl/System/Trace.h"
#include "Cpl/System/Shell.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"


/** NOTE: The test requires that the application define the following
          symbols in colony_config.h for the test to work.  The values
          of the symbols are based on the actual native OS.
 */
#ifndef MY_DIR_COMMAND
#define MY_DIR_COMMAND  "dir"
#endif
#ifndef MY_BAD_COMMAND
#define MY_BAD_COMMAND  "not-a-valid-command"
#endif





/// 
using namespace Cpl::System;

#define SECT_   "_0test"



/** OKAY - SMALL HACK -->testing BOTH supported and not-supporter use cases by
           using #if/#else .
 */
#if CPL_SYSTEM_SHELL_SUPPORTED_ == 0 
#define ANSWER_IS_AVAILABLE             false
#define ANSWER_EXECUTE_CMD              -1
#define ANSWER_EXECUTE_NOT_BAD_CMD      0

#else
#define ANSWER_IS_AVAILABLE             true
#define ANSWER_EXECUTE_CMD              0
#define ANSWER_EXECUTE_NOT_BAD_CMD      0

#endif  // end CPL_SYSTEM_SHELL_SUPPORTED_ == 0

 ////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "shell", "[shell]" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Shutdown_TS::clearAndUseCounter();


    REQUIRE( Shell::isAvailable() == ANSWER_IS_AVAILABLE );

    REQUIRE( Shell::execute( MY_DIR_COMMAND ) == ANSWER_EXECUTE_CMD );
    REQUIRE( Shell::execute( MY_DIR_COMMAND, false ) == ANSWER_EXECUTE_CMD );
    REQUIRE( Shell::execute( MY_DIR_COMMAND, false, false ) == ANSWER_EXECUTE_CMD );
    REQUIRE( Shell::execute( MY_DIR_COMMAND, true, false ) == ANSWER_EXECUTE_CMD );

    REQUIRE( Shell::execute( MY_BAD_COMMAND ) != ANSWER_EXECUTE_NOT_BAD_CMD );
    REQUIRE( Shell::execute( MY_BAD_COMMAND, false ) != ANSWER_EXECUTE_NOT_BAD_CMD );
    REQUIRE( Shell::execute( MY_BAD_COMMAND, false, false ) != ANSWER_EXECUTE_NOT_BAD_CMD );
    REQUIRE( Shell::execute( MY_BAD_COMMAND, true, false ) != ANSWER_EXECUTE_NOT_BAD_CMD );

    REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}

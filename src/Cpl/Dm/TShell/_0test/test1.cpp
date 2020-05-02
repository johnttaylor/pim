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

#include "colony_config.h"
#include "statics.h"


#include "Cpl/TShell/Stdio.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Shutdown.h"
#include <stdio.h>

/// 
extern void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );


////////////////////////////////////////////////////////////////////////////////
static Cpl::TShell::Stdio shell_( cmdProcessor_ );


void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
	// Start the shell
	shell_.launch( infd, outfd );

	// Create thread for my mock-application to run in
	Cpl::System::Thread::create( mockApp, "APP-BOB" );

	// Start the scheduler
	Cpl::System::Api::enableScheduling();

	// Give time for the commands to run
	Cpl::System::Api::sleep( 3 * 1000 );
	int32_t value;
	int8_t valid = mp_apple_.read( value );
	printf( "valid=%d\n", valid );
	CPL_SYSTEM_ASSERT( value == 111 );
	CPL_SYSTEM_ASSERT( Cpl::Dm::ModelPoint::IS_VALID( valid ) );
	valid = mp_orange_.read( value );
	printf( "valid=%d\n", valid );
	CPL_SYSTEM_ASSERT( value == 0x100 );
	CPL_SYSTEM_ASSERT( Cpl::Dm::ModelPoint::IS_VALID( valid ) );
	Cpl::Text::FString<128> valString;
	valid = mp_plum_.read( valString );
	printf( "valid=%d\n", valid );
	CPL_SYSTEM_ASSERT( valString == "Hello Mr. Plum" );
	CPL_SYSTEM_ASSERT( Cpl::Dm::ModelPoint::IS_VALID( valid ) );
	CPL_SYSTEM_ASSERT( mp_plum_.isLocked() );
	
	// Exit the application with a 'pass' result
	Cpl::System::Shutdown::success();
}



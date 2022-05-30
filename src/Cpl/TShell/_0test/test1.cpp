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
}



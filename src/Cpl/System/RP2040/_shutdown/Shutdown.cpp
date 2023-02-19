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


#include "Cpl/System/Shutdown.h"
#include "hardware/watchdog.h"

/// 
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////
static int shutdown_application_( int exit_code )
{
    watchdog_enable( 1, 1 );
    while ( 1 );
}

int Shutdown::success( void )
{
    return shutdown_application_( notifyShutdownHandlers_( OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE ) );
}

int Shutdown::failure( int exit_code )
{
    return shutdown_application_( notifyShutdownHandlers_( exit_code ) );
}



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


#include "Cpl/System/Shutdown.h"
#include "Bsp/Api.h"
#include "FreeRTOS.h"
#include "task.h"


/// 
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////
static int shutdown_application_( int exit_code )
{
    // Check if the scheduler has even started (or is still running)
    if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
    {
        vTaskEndScheduler(); // Note: This only works a very few platforms (at this time only x86 Real Mode PC)!
    }

    // If the scheduler is not running (typical case is it hasn't been started) -->then lock up in forever loop
    else
    {
        Bsp_Api_disableIrqs();
        for ( ;;);
    }

    return exit_code;
}

int Shutdown::success( void )
{
    return shutdown_application_( notifyShutdownHandlers_( OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE ) );
}

int Shutdown::failure( int exit_code )
{
    return shutdown_application_( notifyShutdownHandlers_( exit_code ) );
}



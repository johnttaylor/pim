#ifndef app_h
#define app_h
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
/** @file

    This file defines the top level interface(s) to the platform independent
    application.

 */

#include "Cpl/Io/Input.h"
#include "Cpl/Io/Output.h"
#include "Cpl/Container/Map.h"
#include "Cpl/TShell/Command.h"
#include "Cpl/Dm/PeriodicScheduler.h"

 /*-------------------------------------------------------*/
 /// Width, in pixels, of the display
#define MY_APP_DISPLAY_WIDTH        240

/// Height, in pixels, of the display
#define MY_APP_DISPLAY_HEIGHT       135

/// Default Trace section
#define MY_APP_TRACE_SECTION        "app"


/*-------------------------------------------------------*/
/// Let the platform specific code create the Console stream
extern Cpl::Io::Input* g_consoleInputFdPtr;

/// Let the platform specific code create the Console stream
extern Cpl::Io::Output* g_consoleOutputFdPtr;

/*-------------------------------------------------------*/
/// Expose the TShell command list to allow individual platform to add there own commands
extern Cpl::Container::Map<Cpl::TShell::Command>    g_cmdlist;

/// Expose the runnable object for the UI thread to allow the creation of static UI class
extern Cpl::Dm::PeriodicScheduler*                  g_uiRunnablePtr;

/*-------------------------------------------------------*/
/// Entry function to the application
void runApplication() noexcept;


#endif // end header latch

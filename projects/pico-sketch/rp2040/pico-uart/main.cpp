/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Driver/PicoDisplay/RP2040/Api.h"     // Must be first #include (because of the Pimoroni/Pico SDK)
#include "../../app.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/RP2040/Console.h"
#include "Cpl/TShell/Cmd/Threads.h"



/// Allocate memory for the console stream pointers
Cpl::Io::Input*  g_consoleInputFdPtr;
Cpl::Io::Output* g_consoleOutputFdPtr;

/// Generic thread TShell command
static Cpl::TShell::Cmd::Threads threadsCmd_( g_cmdlist );

/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board
    Bsp_Api_initialize();
    Driver::PicoDisplay::RP2040::initialize();

    // Start the Console/Trace output: Accepting the default UART Config parameters, e.g. 115200, 8N1
    Cpl::System::RP2040::startConsole();
    Cpl::System::RP2040::getConsoleStream().write( "\n**** APPLICATION START-UP *****\n" );
    Cpl::System::RP2040::getConsoleStream().write( "Cpl and the Bsp have been initialized.\n" );

    // Set the stream of the console
    g_consoleInputFdPtr  = &(Cpl::System::RP2040::getConsoleStream());
    g_consoleOutputFdPtr = &(Cpl::System::RP2040::getConsoleStream());

    // Start the application
    runApplication();         // This method should never return
    return 0;
}

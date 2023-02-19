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

#include "Bsp/Api.h"
#include "Storm/Thermostat/Main/Main.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/RP2040/Console.h"
#include "Cpl/TShell/Cmd/Threads.h"
#include "Driver/PicoDisplay/RP2040/Api.h"


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

    // Start the application
    runTheApplication( Cpl::System::RP2040::getConsoleStream(), Cpl::System::RP2040::getConsoleStream() );    
    return 0;
}

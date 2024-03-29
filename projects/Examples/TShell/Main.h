#ifndef Main_h_
#define Main_h_
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

    This file constructs all of the module/instances and performs the start-up 
    and shutdown sequencing for the Application
 */

#include "Cpl/Io/Input.h"
#include "Cpl/Io/Output.h"
#include "Cpl/Container/Map.h"
#include "Cpl/TShell/Command.h"
#include "Cpl/Io/Socket/Listener.h"

enum MyShellOption_T
{
    SHELL_BLOCKING,
    SHELL_SOCKETS,
    SHELL_NON_BLOCKING
};


/** This method launches the start-up sequence, then runs the application, followed
    by the shutdown sequence.  
    
    The 'shellType' determines what 'flavor' of TShell to create.  
    
    The 'infd' and 'outfd' are the file descriptors to be used for the 
    TShell/Command-Line-Interface WHEN 'shellType' != SHELL_SOCKETS 
    
    The 'listener' argument provide a platform specific socket listener to
    use for the TShell file descriptors WHEN 'shellType1 == SHELL_SOCKETS. The
    'portNum' argument specifies which port number to listen on.

    
    NOTES:
        o The caller of method MUST be executing in Cpl::System::Thread
        o This method ONLY returns once the shutdown sequence has
          been completed.  
        o This method assumes that its thread is DEDICATED for performing the 
          start-up/shutdown sequences. Once the start-up sequence has been
          completed, this thread stays blocked/in-the-waiting state until the 
          Cpl::System::Shutdown interface is called to exit the application. 
          All "shutdown handlers" will execute in this thread.
 */
int runTheApplication( MyShellOption_T              shellType,
                       Cpl::Io::Input&              infd, 
                       Cpl::Io::Output&             outfd,
                       Cpl::Io::Socket::Listener&   listener,
                       int                          portNum );


/** Expose the TShell command list to facilitate creating platform specific 
    TShell commands.
 */
extern Cpl::Container::Map<Cpl::TShell::Command>    g_cmdlist;


#endif  // end header latch

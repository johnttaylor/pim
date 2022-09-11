#ifndef Cpl_System_Shell_h_
#define Cpl_System_Shell_h_
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
/** @file */


#include "colony_map.h"

/// 
namespace Cpl {
/// 
namespace System {


/** This class defines methods for interfacing with the Platform native OS to
    execute a native OS system/shell command.  Support for this interface is
    target/platform specific, i.e. support for this interface is OPTIONAL. If
    the target does not support this interface, then the 'execute' method
    does NOTHING (and returns -1); and the method isAvailable() return false.

 */
class Shell
{
public:
    /** Executes the System Shell command as specified by 'cmdstring'.  If
        noEchoStdOut is set to true then the standard output of the shell command
        is set to the system's NUL device.  If noEchoStdErr is set to true then
        the standard error output of the shell command is set to the system's
        NUL device. The method returns -1 if unable to execute the command,
        else returns the result code of the command executed.
     */
    static int execute( const char* cmdstring, bool noEchoStdOut = true, bool noEchoStdErr = true );

    /** Returns true if the system command shell is available.
     */
    static bool isAvailable();

};


/*
** COMPONENT SCOPED SYMBOLS.  The application should NEVER directly use/references these symbols.
*/
/// Defer the definition of the target specific 'null device' to the application's 'platform'
#define CPL_SYSTEM_SHELL_NULL_DEVICE_           CPL_SYSTEM_SHELL_NULL_DEVICE_x_MAP

/// Defer the decision to support/not-support this interface to the application's 'platform'
#define CPL_SYSTEM_SHELL_SUPPORTED_             CPL_SYSTEM_SHELL_SUPPORTED_x_MAP


};      // end namespaces
};
#endif  // end header latch


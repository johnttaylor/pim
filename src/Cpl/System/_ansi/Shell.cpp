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

#include <stdlib.h>
#include <string.h>
#include "Cpl/System/Shell.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/DString.h"


#define SECT_   "Cpl::System::Shell"


/////////////////////////////////////////////////////////////////////////////
/*
** Small HACK to make it easier to support the not-supported scenario
*/
#if CPL_SYSTEM_SHELL_SUPPORTED_ == 0 
bool Cpl::System::Shell::isAvailable()
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("isAvailable() -->Shell not supported.") );
    return false;
}

int Cpl::System::Shell::execute( const char* cmdstring, bool noEchoStdOut, bool noEchoStdErr )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("execute(\"%s\") -->Shell not supported.", cmdstring) );
    return -1;
}



/////////////////////////////////////////////////////////////////////////////
#else

// 
#define MY_STDOUT_NULL_     " 1>" CPL_SYSTEM_SHELL_NULL_DEVICE_
#define MY_STDERR_NULL_     " 2>" CPL_SYSTEM_SHELL_NULL_DEVICE_


/*
** Implement the Shell interface using the std C Library's system command
*/
bool Cpl::System::Shell::isAvailable()
{
    return system( 0 ) != 0; // The system() method returns non-zero when a valid command interpreter/shell is found/available
}


int Cpl::System::Shell::execute( const char* cmdstring, bool noEchoStdOut, bool noEchoStdErr )
{
    // Create a copy of command-string and allocate extra space for redirecting the output
    Cpl::Text::DString cmd( cmdstring, strlen( cmdstring ) + strlen( MY_STDOUT_NULL_ ) + strlen( MY_STDERR_NULL_ ) );

    // Redirect STDOUT to the null device when requested (abort the shell command if there is an error)
    if ( noEchoStdOut )
    {
        cmd += MY_STDOUT_NULL_;
        if ( cmd.truncated() )
        {
            return -1;
        }
    }

    // Redirect STDERR to the null device when requested (abort the shell command if there is an error)
    if ( noEchoStdErr )
    {
        cmd += MY_STDERR_NULL_;
        if ( cmd.truncated() )
        {
            return -1;
        }
    }

    // Execute the command
    CPL_SYSTEM_TRACE_MSG( SECT_, ("execute(\"%s\")", cmd()) );
    return system( cmd );
}



#endif // end CPL_SYSTEM_SHELL_SUPPORTED_ == 0



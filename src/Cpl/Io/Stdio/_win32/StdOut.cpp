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

#include "Cpl/Io/Stdio/StdOut.h"
#include <windows.h>

//
using namespace Cpl::Io::Stdio;

StdOut::StdOut()
    :Output_( (void*) GetStdHandle( STD_OUTPUT_HANDLE ) )
{
}

void StdOut::close()
{
    // Close the internal class handle/fd - but don't actually close the native stream
    if ( m_outFd.m_handlePtr != (void*) INVALID_HANDLE_VALUE)
    {
        m_outFd.m_handlePtr = (void*) INVALID_HANDLE_VALUE;
    }
}

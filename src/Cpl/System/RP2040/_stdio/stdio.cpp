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

#include "Cpl/System/RP2040/Stdio.h"
#include "Cpl/Io/Serial/RP2040/Stdio/Input.h"
#include "Cpl/Io/Serial/RP2040/Stdio/Output.h"

/// Create my streams
static Cpl::Io::Serial::RP2040::Stdio::Input  infd_;
static Cpl::Io::Serial::RP2040::Stdio::Output outfd_;

Cpl::Io::Input& Cpl::System::RP2040::getStdinStream()
{
    return infd_;
}
Cpl::Io::Output& Cpl::System::RP2040::getStdoutStream()
{
    return outfd_;
}

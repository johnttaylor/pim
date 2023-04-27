#ifndef Driver_Dio_TShell_InOutCmd_h
#define Driver_Dio_TShell_InOutCmd_h
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

#include "colony_config.h"
#include "Cpl/TShell/Cmd/Command.h"
#include "Driver/DIO/InOut.h"

/// Maximum supported number of Inputs/Outputs
#ifndef OPTION_DRIVER_DIO_IN_OUT_CMD_MAX_IN_PINS
#define OPTION_DRIVER_DIO_IN_OUT_CMD_MAX_IN_PINS        32
#endif

#ifndef OPTION_DRIVER_DIO_IN_OUT_CMD_MAX_OUT_PINS
#define OPTION_DRIVER_DIO_IN_OUT_CMD_MAX_OUT_PINS       32
#endif

///
namespace Driver {
///
namespace DIO {

/** This class implements a TShell command that exercises the RH/Temp
    driver. The class is ABSOLUTELY NOT THREAD SAFE!  Its intended use is to 
    support unit testing.
 */
class InOutCmd : public Cpl::TShell::Cmd::Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "inout";

    /// The command usage string
    static constexpr const char* usage = "inout\n"
                                         "inout start <numIn> [<in0Pin> <in0Blob> .. <inNPin> <inNBlob>] <numOut> [out0Pin out0Blob .. [outMPin outMBlob]\n"
                                         "inout stop\n" \
                                         "inout write <pin-idx> <bitValue>";


    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Exercises the InOut Driver.  The command with no arguments displays\n"
                                                "  the state of the driver as well as the current input values. The user is\n"
                                                "  required to manually start (and stop) the driver for proper operation.";
protected:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    InOutCmd( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
              Cpl::TShell::Security::Permission_T        minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;

public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;
};

};      // end namespaces
};
#endif  // end header latch

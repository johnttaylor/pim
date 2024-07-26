#ifndef Driver_Crypt_TShell_Random_h
#define Driver_Crypt_TShell_Random_h
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


///
namespace Driver {
///
namespace Crypto {
///
namespace TShell {

/** This class implements a  Shell command.  This command is only 'useful' if
    the application was built to use simulated time (see Cpl::System::SimTick.h).
 */
class Random : public Cpl::TShell::Cmd::Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "rnd";

    /// The command usage string
    static constexpr const char* usage = "rnd <numBytes>";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Generates a random number of <numBytes> long.";


protected:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    Random( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
            Cpl::TShell::Security::Permission_T        minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;

public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

};

}       // end namespaces
} 
} 
#endif  // end header latch

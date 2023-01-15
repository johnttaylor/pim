#ifndef Cpl_TShell_Cmd_Tick_h
#define Cpl_TShell_Cmd_Tick_h
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
namespace Cpl {
///
namespace TShell {
///
namespace Cmd {

/** This class implements a  Shell command.  This command is only 'useful' if
    the application was built to use simulated time (see Cpl::System::SimTick.h).
 */
class Tick : public Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "tick";

    /// The command usage string
    static constexpr const char* usage = "tick [+nn|@mm]";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Advances the simulated time by nn number of milliseconds and/or advances time\n" 
                                                "  to an absolute time at mm milliseconds.  When no arguments are use the current\n"
                                                "  simulated and real time is displayed.";


protected:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    Tick( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
          Security::Permission_T                     minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;

public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

};

};      // end namespaces
};
};
#endif  // end header latch

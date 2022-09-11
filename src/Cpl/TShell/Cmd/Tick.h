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

/** Command
                                    "         1         2         3         4         5         6         7         8"
                                    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define CPLTSHELLCMD_CMD_TICK_		"tick"
/// Usage
#define CPLTSHELLCMD_USAGE_TICK_    "tick [+nn|@mm]"

/// Detailed Help text
#ifndef CPLTSHELLCMD_DETAIL_TICK_
#define CPLTSHELLCMD_DETAIL_TICK_   "  Advances the simulated time by nn number of milliseconds and/or advances time\n" \
                                    "  to an absolute time at mm milliseconds.  When no arguments are use the current\n" \
                                    "  simulated and real time is displayed."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue


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
protected:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return CPLTSHELLCMD_USAGE_TICK_; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return CPLTSHELLCMD_DETAIL_TICK_; }


public:
    /// Constructor
    Tick( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept;

public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

};

};      // end namespaces
};
};
#endif  // end header latch

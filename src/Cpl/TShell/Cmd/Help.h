#ifndef Cpl_TShell_Cmd_Help_h
#define Cpl_TShell_Cmd_Help_h
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
#define CPLTSHELLCMD_CMD_HELP_	    "help"
/// Usage
#define CPLTSHELLCMD_USAGE_HELP_    "help [* | <cmd>]"

/// Detailed Help text
#ifndef CPLTSHELLCMD_DETAIL_HELP_
#define CPLTSHELLCMD_DETAIL_HELP_   "  Displays list of all supported commands and optionally their detailed help. If\n" \
                                    "  the second argument is command, then the detailed help for that command will\n"   \
                                    "  be displayed."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue


///
namespace Cpl {
///
namespace TShell {
///
namespace Cmd {



/** This class implements a  Shell command
 */
class Help : public Command
{
public:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return CPLTSHELLCMD_USAGE_HELP_; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return CPLTSHELLCMD_DETAIL_HELP_; }


public:
    /// Constructor
    Help( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

};

};      // end namespaces
};
};
#endif  // end header latch

#ifndef Cpl_TShell_Cmd_Bye_h
#define Cpl_TShell_Cmd_Bye_h
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
#define CPLTSHELLCMD_CMD_BYE_		"bye"
/// Usage
#define CPLTSHELLCMD_USAGE_BYE_     "bye [app [<exitcode>]]"

/// Detailed Help text
#ifndef CPLTSHELLCMD_DETAIL_BYE_
#define CPLTSHELLCMD_DETAIL_BYE_    "  Requests the  shell to exit. If the optional argument 'app' is specified\n" \
                                    "  then the application is exited with the specifed <exitcode>. The default\n" \
                                    "  <exitcode> is '0'."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue


///
namespace Cpl {
///
namespace TShell {
///
namespace Cmd {

/** This class implements a  Shell command
 */
class Bye : public Command
{
protected:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return CPLTSHELLCMD_USAGE_BYE_; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return CPLTSHELLCMD_DETAIL_BYE_; }


public:
    /// Constructor
    Bye( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept;

public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

};

};      // end namespaces
};
};
#endif  // end header latch

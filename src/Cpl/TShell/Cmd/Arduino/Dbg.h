#ifndef Cpl_TShell_Dac_Cmd_Arduino_Dbg_h
#define Cpl_TShell_Dac_Cmd_Arduino_Dbg_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014, 2015  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */


#include "colony_config.h"
#include "Cpl/TShell/Cmd/Command.h"

/** Usage
                                    "         1         2         3         4         5         6         7         8"
                                    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define CPLTSHELLDACMD_USAGE_DBG_     "dbg mem" 


/// Detailed Help text
#ifndef CPLTSHELLDACMD_DETAIL_DBG_
#define CPLTSHELLDACMD_DETAIL_DBG_    "  Shell command to invoke 'native Arduino' utilities/methods"

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue


///
namespace Cpl {
namespace TShell {
namespace Cmd {
namespace Arduino {



/** This class implements a DAC Shell command
 */
class Dbg : public Cpl::TShell::Cmd::Command
{
public:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return CPLTSHELLDACMD_USAGE_DBG_; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return CPLTSHELLDACMD_DETAIL_DBG_; }

public:
    /// Constructor
    Dbg( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Context_& context, char* rawCmdString, Cpl::Io::Output& outfd ) noexcept;

};

};      // end namespaces
};
};
};
#endif  // end header latch

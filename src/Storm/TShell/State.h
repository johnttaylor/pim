#ifndef Storm_TShell_State_h
#define Storm_TShell_State_h
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
/** @file */

#include "colony_config.h"
#include "Cpl/TShell/Cmd/Command.h"

/** Command
                                        "         1         2         3         4         5         6         7         8"
                                        "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define STORMTSHELLCMD_CMD_STATE_	    "state"
/// Usage
#define STORMTSHELLCMD_USAGE_STATE_     "state [usarlct]"

/// Detailed Help text
#ifndef STORMTSHELLCMD_DETAIL_STATE_    
#define STORMTSHELLCMD_DETAIL_STATE_    "  Displays state and/or system status information. The 'usarl' options can be\n" \
                                        "  mixed/matched.  No options displays all state.  Options are:\n" \
                                        "  u=User, s=System, a=Alarms, r=Relays, l=Load, c=CycleInfo, t=Times."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue


///
namespace Storm {
///
namespace TShell {

/** This class implements a  Shell command.  
 */
class State : public Cpl::TShell::Cmd::Command
{
protected:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return STORMTSHELLCMD_USAGE_STATE_; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return STORMTSHELLCMD_DETAIL_STATE_; }


public:
    /// Constructor
    State( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept;

public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

protected:
    /// Helper method
    bool outputUser( Cpl::TShell::Context_& context );

    /// Helper method
    bool outputSystem( Cpl::TShell::Context_& context );

    /// Helper method
    bool outputRelays( Cpl::TShell::Context_& context );

    /// Helper method
    bool outputLoad( Cpl::TShell::Context_& context );

    /// Helper method
    bool outputAlarms( Cpl::TShell::Context_& contex );

    /// Helper method
    bool outputCycle( Cpl::TShell::Context_& contex );
 
    /// Helper method
    bool outputTimes( Cpl::TShell::Context_& contex );
};

};      // end namespaces
};
#endif  // end header latch

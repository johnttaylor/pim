#ifndef Storm_TShell_WhiteBox_h
#define Storm_TShell_WhiteBox_h
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
#define STORMTSHELLCMD_CMD_WB_	        "wb"

/// Usage
#define STORMTSHELLCMD_USAGE_WB_        "wb\n" \
                                        "wb enot enable|disable\n" \
                                        "wb acc  true|false" 
/// Detailed Help text
#ifndef STORMTSHELLCMD_DETAIL_WB_    
#define STORMTSHELLCMD_DETAIL_WB_       "  Displays and set the various White Box testing model points and settings.  The\n" \
                                        "  abbreviations are:\n" \
                                        "    enot:= Equipment miNimum Off Time\n" \
                                        "    acc:=  Abort Current on/off Cycle"

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue


///
namespace Storm {
///
namespace TShell {

/** This class implements a  Shell command.  
 */
class WhiteBox : public Cpl::TShell::Cmd::Command
{
protected:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return STORMTSHELLCMD_USAGE_WB_; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return STORMTSHELLCMD_DETAIL_WB_; }


public:
    /// Constructor
    WhiteBox( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept;

public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;


};

};      // end namespaces
};
#endif  // end header latch

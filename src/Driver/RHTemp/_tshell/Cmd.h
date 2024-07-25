#ifndef Driver_RHTemp_TShell_Cmd_h
#define Driver_RHTemp_TShell_Cmd_h
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
#include "Driver/RHTemp/Api.h"


///
namespace Driver {
///
namespace RHTemp {

/** This class implements a TShell command that exercises the RH/Temp
    driver. The class is ABSOLUTELY NOT THREAD SAFE!  Its intended use is to 
    support unit testing.
 */
class TShellCmd : public Cpl::TShell::Cmd::Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "rhtemp";

    /// The command usage string
    static constexpr const char* usage = "rhtemp start|stop\n"
                                         "rhtemp sample\n"
                                         "rhtemp begin\n"
                                         "rhtemp read\n"
                                         "rhtemp heater on|off";


    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Exercises the RHTemp Driver.  The user is required to manually start (and\n"
                                                "  stop) the driver for proper operation.\n"
                                                "  'sample' performs a blocking sampling\n"
                                                "  'begin' starts an asynchronous sample\n"
                                                "  'read' returns the async result/state";
protected:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    TShellCmd( Driver::RHTemp::Api&                       driver,
               Cpl::Container::Map<Cpl::TShell::Command>& commandList,
               Cpl::TShell::Security::Permission_T        minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;

public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

protected:
    /// Driver instance under test
    Driver::RHTemp::Api&    m_driver;
};

};      // end namespaces
};
#endif  // end header latch

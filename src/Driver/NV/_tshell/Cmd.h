#ifndef Driver_NV_TShell_NV_h
#define Driver_NV_TShell_NV_h
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
#include "Driver/NV/Api.h"


///
namespace Driver {
///
namespace NV {

/** This class implements a TShell command that tests/directly-operates
    on the NV media.

    WARNING: When this command is used - then application can not rely
             on the NV contents.
 */
class Cmd : public Cpl::TShell::Cmd::Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "nv";

    /// The command usage string
    static constexpr const char* usage = "nv ERASE\n"
                                         "nv read <startOffSet> <len>\n"
                                         "nv write <startOffset> <bytes...>\n" 
                                         "nv test (aa|55|blank)";


    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Exercises and tests the NV media.  The supported tests are:\n"
                                                "    'aa'    - Writes (and verifies) 0xAA to all of the media\n"
                                                "    '55'    - Writes (and verifies) 0x55 to all of the media\n"
                                                "    'blank' - Verify if the media is 'erased'";
protected:
    /// See Cpl::TShell::Command
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    Cmd( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
         Driver::NV::Api&                           nvDriver,
         Cpl::TShell::Security::Permission_T        minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;

public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

protected:
    /// Helper method
    Cpl::TShell::Command::Result_T writeAll( uint8_t valueToWrite, Cpl::TShell::Context_& context, Cpl::Text::String& outtext );

    /// Helper method
    Cpl::TShell::Command::Result_T readAll( uint8_t execptedValue, Cpl::TShell::Context_& context, Cpl::Text::String& outtext );

protected:
    /// NV driver
    Driver::NV::Api& m_nvDriver;
};

}       // end namespaces
} 
#endif  // end header latch

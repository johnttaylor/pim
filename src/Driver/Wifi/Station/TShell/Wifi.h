#ifndef Driver_Wifi_Station_TShell_Dm_h
#define Driver_Wifi_Station_TShell_Dm_h
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
#include "Driver/Wifi/Station/Connection.h"




/** Command
                                                "         1         2         3         4         5         6         7         8"
                                                "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define DRIVERWIFISTATIONTSHELL_CMD_WIFI_	    "wifista"

/// Usage
#define DRIVERWIFISTATIONTSHELL_USAGE_WIFI_     "wifista\n" \
                                                "wifista <auth-method> <ssid> <passwd>}\n" \
                                                "wifista disconnect"

/// Detailed Help text
#ifndef DRIVERWIFISTATIONTSHELL_DETAIL_WIFI_
#define DRIVERWIFISTATIONTSHELL_DETAIL_WIFI_    "  Displays the current Wifi Station connection, connects and disconnect to a\n" \
                                                "  a WIFI network.  <auth-method> selection:\n"\
                                                "       0: No authorization required\n"\
                                                "       1: WPA authorization\n"\
                                                "       2: WPA2 authorization (preferred)\n"\
                                                "       3: eWPA2_MIXED_PSK"

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue



///
namespace Driver {
///
namespace Wifi {
///
namespace Station {
///
namespace TShell {



/** This class implements a TShell command.  Note: Up to 10 different
    instance of this command can be instantiated - but each instance MUST
    have a different database number.  The database number specified by
    specifying the actual command name, e.g. 'dm0' is database number 0,
    'dm1' is database number 1, etc.
 */
class Wifi : public Cpl::TShell::Cmd::Command
{
public:
    /// See Cpl::TShell::Command                                                               `
    const char* getUsage() const noexcept { return DRIVERWIFISTATIONTSHELL_USAGE_WIFI_; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return DRIVERWIFISTATIONTSHELL_DETAIL_WIFI_; }


public:
    /// Constructor
    Wifi( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

};

};      // end namespaces
};
};
};
#endif  // end header latch

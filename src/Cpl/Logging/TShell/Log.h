#ifndef Cpl_Logging_TShell_Log_h
#define Cpl_Logging_TShell_Log_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2023  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "colony_config.h"
#include "Cpl/TShell/Cmd/Command.h"
#include "Cpl/Persistent/IndexedEntryServer.h"
#include "Cpl/Logging/EntryData_T.h"

///
namespace Cpl {
///
namespace Logging {
///
namespace TShell {



/** This class implements a TShell command that operates on the persistently
    store "log file".  The implementation ASSUMES that the log entries are
    stored using the Cpl::Persistent framework.

    Note: The command/TShell is blocked during access to the persistent media.
 */
class Log : public Cpl::TShell::Cmd::Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "log";

    /// The command usage string
    static constexpr const char* usage = "log [*|<max>]\n"
                                         "log <nth> (*|<max>)\n"
                                         "log clear\n";

        /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                              1         2         3         4         5         6         7         8
                                                     12345678901234567890123456789012345678901234567890123456789012345678901234567890
         */
        static constexpr const char* detailedHelp = "  Displays and clear stored log records entries. Records are always displayed\n"
                                                    "  newest entry first.  The output format is:\n"
                                                    "  \"[<entrykey>] (<entryTimestamp>) <CATID>:<MSGID>. <text>\"\n"
                                                    "  - 'log' with no arguments displays the newest entry.\n"
                                                    "  - 'log *' displays all entries.\n"
                                                    "  - 'log <max>' displays at most <max> entries.\n"
                                                    "  - 'log <nth> *' displays all log entries starting with the <nth> record."
                                                    "  - 'log <nth> <max>' displays up to <max> entries starting with <nth> record.";


public:
    /// See Cpl::TShell::Command                                                               `
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    Log( Cpl::Container::Map<Cpl::TShell::Command>&                         commandList, 
         Cpl::Itc::PostApi&                                                 logEntryServerMailbox,
         Cpl::Persistent::IndexedEntryServer<Cpl::Logging::EntryData_T>&    logEntryServer,
         Cpl::TShell::Security::Permission_T                                minPermLevel=OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

protected:
    /// Helper method
    bool getEntries( Cpl::TShell::Context_& context, Cpl::Text::String& outtext, unsigned startIndex=0, unsigned maxEntries=0xFFFF ) noexcept;

    /// Helper method
    bool getLatestLogEntry( Cpl::Persistent::GetLatestRequest::Payload& payload ) noexcept;

    /// Helper method
    bool getPreviousLogEntry( Cpl::Persistent::GetPreviousRequest::Payload& payload ) noexcept;

protected:
    /// Mailbox of the thread that that the Log Server executes in
    Cpl::Itc::PostApi&                                                 m_logServerMailbox;

    /// Log Entry Server
    Cpl::Persistent::IndexedEntryServer<Cpl::Logging::EntryData_T>&    m_logServer;
};

};      // end namespaces
};
};
#endif  // end header latch

#ifndef Cpl_MApp_Cmd_h
#define Cpl_MApp_Cmd_h
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */

#include "colony_config.h"
#include "Cpl/TShell/Cmd/Command.h"
#include "Cpl/MApp/ManagerApi.h"


/** Maximum number of MApp instance that can be queried/listed
 */
#ifndef OPTION_CPL_MAPP_TSHELL_MAX_INSTANCES
#define OPTION_CPL_MAPP_TSHELL_MAX_INSTANCES        8
#endif

///
namespace Cpl {
///
namespace MApp {

/** The class implements a TShell command that interacts with the MApp Manager
    to start, stop, and list available MApp instances
 */
class Cmd : public Cpl::TShell::Cmd::Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "mapp";

    /// The command usage string
    static constexpr const char* usage = "mapp\n"
                                         "mapp start <mapp> [<args...]\n"
                                         "mapp stop <mapp>|ALL\n"
                                         "mapp ls";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Asynchronously starts/stops individual MApps (micro applications).  Issuing\n" 
                                                "  the command without arguments displays the list of running MApps";

public:
    /// See Cpl::TShell::Command                                                               `
    const char* getUsage() const noexcept { return usage; }

    /// See Cpl::TShell::Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// Constructor
    Cmd( Cpl::Container::Map<Cpl::TShell::Command>& commandList, Cpl::MApp::ManagerApi& mappManager ) noexcept;


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept;

protected:
    /// Handle to the MApp manager
    Cpl::MApp::ManagerApi&  m_mappManager;

    /// Array for holding pointers to queried MApp instances
    Cpl::MApp::MAppApi*     m_mappList[OPTION_CPL_MAPP_TSHELL_MAX_INSTANCES];
};

};      // end namespaces
};
#endif  // end header latch

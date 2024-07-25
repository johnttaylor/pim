/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/

#include "Cmd.h"
#include "Cpl/Text/strip.h"
#include "Cpl/System/Api.h"

///
using namespace Cpl::MApp;

#define OVERFLOW_MSG    "WARNING: Insufficient memory to list all MApp instances"

#define SUBCMD_START    "start"
#define SUBCMD_STOP     "stop"
#define SUBCMD_LIST     "ls"

///////////////////////////
Cmd::Cmd( Cpl::Container::Map<Cpl::TShell::Command>& commandList, Cpl::MApp::ManagerApi& mappManager ) noexcept
    : Cpl::TShell::Cmd::Command( commandList, verb )
    , m_mappManager( mappManager )
{
}

///////////////////////////
Cpl::TShell::Command::Result_T Cmd::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    const char*        subCmd  = Cpl::Text::stripSpace( Cpl::Text::stripNotSpace( cmdString ) );
    Cpl::Text::String& outtext = context.getOutputBuffer();
    bool               io      = true;

    // No arguments
    if ( *subCmd == '\0'  )
    {
        size_t numFound = 0;
        bool success = m_mappManager.getStartedMApps( m_mappList, OPTION_CPL_MAPP_TSHELL_MAX_INSTANCES, numFound );
        if ( numFound == 0 )
        {
            io &= context.writeFrame( "NO MApps are running." );
            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }

        io &= context.writeFrame( "RUNNING MApps:" );
        for ( size_t idx=0; idx < numFound; idx++ )
        {
            io &= context.writeFrame( m_mappList[idx]->getName() );
            io &= context.writeFrame( m_mappList[idx]->getDescription() );
            if ( idx + 1 < numFound )
            {
                io &= context.writeFrame( "" );
            }
        }

        if ( !success )
        {
            io &= context.writeFrame( OVERFLOW_MSG );
        }

        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // START command
    if ( strncmp( subCmd, SUBCMD_START, strlen( SUBCMD_START )) == 0 )
    {
        // Get MApp name
        const char* mappName = Cpl::Text::stripSpace( Cpl::Text::stripNotSpace( subCmd ) );
        if ( *mappName == '\0' )
        {
            outtext.format( "ERROR: missing one or more arguments. [%s]", cmdString );
            context.writeFrame( outtext );
            return Command::eERROR_MISSING_ARGS;
        }

        // Attempt to start the MApp
        // NOTE: The casting of the string pointers to non-const pointer is OKAY
        //       because the pointers are derived from 'cmdString' which is const 
        //       char*
        char*       endName  = (char*) Cpl::Text::stripNotSpace( mappName );
        const char* mappArgs = Cpl::Text::stripSpace( endName );
        *endName             = '\0';
        if ( !m_mappManager.startMApp( mappName, (char*) mappArgs ) )
        {
            outtext.format( "ERROR: The MApp [%s] was not found", mappName );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }
        
        return Command::eSUCCESS;
    }

    // STOP command
    if ( strncmp( subCmd, SUBCMD_STOP, strlen( SUBCMD_STOP ) ) == 0 )
    {
        // Get MApp name
        const char* mappName = Cpl::Text::stripSpace( Cpl::Text::stripNotSpace( subCmd ) );
        if ( *mappName == '\0' )
        {
            outtext.format( "ERROR: missing one or more arguments. [%s]", cmdString );
            context.writeFrame( outtext );
            return Command::eERROR_MISSING_ARGS;
        }

        // check for STOP ALL 
        if ( strncmp( mappName, "ALL", 3 ) == 0 )
        {
            m_mappManager.stopAllMApps();
        }

        // Attempt to stop the MApp
        else
        {
            char* endName  = (char*) Cpl::Text::stripNotSpace( mappName );
            *endName       = '\0';
            if ( !m_mappManager.stopMApp( mappName ) )
            {
                outtext.format( "ERROR: The MApp [%s] was not started", mappName );
                context.writeFrame( outtext );
                return Command::eERROR_INVALID_ARGS;
            }
        }

        return Command::eSUCCESS;
    }

    // LIST command
    if ( strncmp( subCmd, SUBCMD_LIST, strlen( SUBCMD_LIST ) ) == 0 )
    {
        size_t numFound = 0;
        bool success = m_mappManager.getAvailableMApps( m_mappList, OPTION_CPL_MAPP_TSHELL_MAX_INSTANCES, numFound );
        if ( numFound == 0 )
        {
            io &= context.writeFrame( "NO MApps available." );
            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }

        io &= context.writeFrame( "AVAILABLE MApps:" );
        for ( size_t idx=0; idx < numFound; idx++ )
        {
            io &= context.writeFrame( m_mappList[idx]->getName() );
            io &= context.writeFrame( m_mappList[idx]->getDescription() );
            io &= context.writeFrame( m_mappList[idx]->getUsage() );
            if ( idx + 1 < numFound )
            {
                io &= context.writeFrame( "" );
            }
        }

        if ( !success )
        {
            io &= context.writeFrame( OVERFLOW_MSG );
        }

        return io ? Command::eSUCCESS : Command::eERROR_IO;

    }

    // Unsupported subcommand
    context.writeFrame( "Unsupported sub-command or bad syntax" );
    return Command::eERROR_INVALID_CMD;
}


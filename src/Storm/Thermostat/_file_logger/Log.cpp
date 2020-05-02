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

#include "Log.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/System/ElapsedTime.h"
#include "ModelPoints.h"
#include <string.h>         

using namespace Storm::Thermostat;


///////////////////////////
Log::Log( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
    :Command( commandList, STORMTHERMOSTATMAINCMD_CMD_LOG_ )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Log::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    unsigned                        numParms = tokens.numParameters();
    bool                            io       = true;

    // Error checking
    if ( numParms > 3 )
    {
        return Cpl::TShell::Command::eERROR_EXTRA_ARGS;
    }

    // Display current status
    if ( numParms == 1 )
    {
        Cpl::Text::FString<OPTION_LOGGER_MAX_FILE_NAME> fname;
        if ( Cpl::Dm::ModelPoint::IS_VALID( mp_loggingFileName.read( fname ) ) == false )
        {
            io = context.writeFrame( "Logging disabled." );
        }
        else
        {
            outtext.format( "Logging ENABLED. Filename=%s", fname.getString() );
            io &= context.writeFrame( outtext.getString() );
        }
    }

    // Enable logging
    else if ( numParms == 3 && tokens.getParameter( 1 )[0] == 'e' )
    {
        mp_loggingFileName.write( tokens.getParameter(2) );
        outtext.format( "Logging ENABLED. Filename=%s", tokens.getParameter( 2 ) );
        io = context.writeFrame( outtext.getString() );
    }

    // Disable logging
    else if ( numParms == 2 && tokens.getParameter( 1 )[0] == 'd' )
    {
        mp_loggingFileName.setInvalid();
        io = context.writeFrame( "Logging disabled." );
    }

    //  Bad syntax
    else
    {
        return Cpl::TShell::Command::eERROR_BAD_SYNTAX;
    }

    // If I get here -->the command syntax was correct
    return io ? Command::eSUCCESS : Command::eERROR_IO;
}


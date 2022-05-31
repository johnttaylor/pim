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

#include "Cmd.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/System/ElapsedTime.h"
#include "Storm/Thermostat/ModelPoints.h"
#include "ModelPoints.h"
#include <string.h>         

using namespace Storm::Thermostat::SimHouse;


///////////////////////////
Cmd::Cmd( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
    :Command( commandList, STORMTHERMOSTATSIMHOUSECMD_CMD_HOUSE_ )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Cmd::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
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
        bool   enabled;
        float  odt;
        float  idt;
        if ( mp_houseSimEnabled.read( enabled ) == false ||
             mp_activeIdt.read( idt ) == false )
        {
            context.writeFrame( "ERROR: One or more Model Point inputs are invalid!" );
            return Cpl::TShell::Command::eERROR_FAILED;
        }
        if ( mp_outdoorTemp.read( odt ) )
        {
            outtext.format( "HouseSimEnabled=%s, odt=%0.02f, idt=%0.02f", enabled ? "YES" : "no", odt, idt );
        }
        else
        {
            outtext.format( "HouseSimEnabled=%s, odt=<n/a>, idt=%0.02f", enabled ? "YES" : "no", idt );
        }

        io &= context.writeFrame( outtext.getString() );
    }

    // Set new ODT
    else if ( numParms == 2 )
    {
        double odt = 0;
        if ( !Cpl::Text::a2d( odt, tokens.getParameter( 1 ) ) )
        {
            outtext.format( "Invalid 'odt' argument (%s)", tokens.getParameter( 1 ) );
            context.writeFrame( outtext );
            return Cpl::TShell::Command::eERROR_INVALID_ARGS;
        }

        mp_outdoorTemp.write( (float) odt );

        outtext.format( "odt=%0.02f", odt );
        io = context.writeFrame( outtext.getString() );
    }

    // Enable the simulator
    else if ( numParms == 3 && tokens.getParameter( 1 )[0] == 'e' )
    {
        double odt = 0;
        if ( !Cpl::Text::a2d( odt, tokens.getParameter( 2 ) ) )
        {
            outtext.format( "Invalid 'odt' argument (%s)", tokens.getParameter( 2 ) );
            context.writeFrame( outtext );
            return Cpl::TShell::Command::eERROR_INVALID_ARGS;
        }

        mp_outdoorTemp.write( (float) odt );
        mp_houseSimEnabled.write( true );
        io = context.writeFrame( "Cmd simulator ENABLED." );
    }

    // Disable the simulator
    else if ( numParms == 3 && tokens.getParameter( 1 )[0] == 'd' )
    {
        double idt = 0;
        if ( !Cpl::Text::a2d( idt, tokens.getParameter( 2 ) ) )
        {
            outtext.format( "Invalid 'idt' argument (%s)", tokens.getParameter( 2 ) );
            context.writeFrame( outtext );
            return Cpl::TShell::Command::eERROR_INVALID_ARGS;
        }

        mp_primaryRawIdt.write( (float) idt );
        mp_outdoorTemp.setInvalid();
        mp_houseSimEnabled.write( false );
        io = context.writeFrame( "Cmd simulator disabled." );
    }

    //  Bad syntax
    else
    {
        return Cpl::TShell::Command::eERROR_BAD_SYNTAX;
    }

    // If I get here -->the command syntax was correct
    return io ? Command::eSUCCESS : Command::eERROR_IO;
}


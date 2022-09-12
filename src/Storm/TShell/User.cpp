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

#include "User.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/System/ElapsedTime.h"
#include "Storm/Thermostat/ModelPoints.h"
#include "Storm/Type/ThermostatMode.h"
#include <string.h>         


///
using namespace Storm::TShell;

///////////////////////////
User::User( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
    :Command( commandList, STORMTSHELLCMD_CMD_USER_ )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T User::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    unsigned                        numParms = tokens.numParameters();
    bool                            io       = true;

    // Error checking
    if ( numParms > 4 )
    {
        return Cpl::TShell::Command::eERROR_EXTRA_ARGS;
    }
    else if ( numParms < 3 )
    {
        return Cpl::TShell::Command::eERROR_MISSING_ARGS;
    }

    if ( tokens.getParameter( 1 )[0] == 'm' && numParms == 3 )
    {
        auto maybeValue = Storm::Type::ThermostatMode::_from_string_nothrow( tokens.getParameter( 2 ) );
        if ( !maybeValue )
        {
            outtext.format( "Specified thermostat mode (%s) is not a valid setting", tokens.getParameter( 2 ) );
            context.writeFrame( outtext.getString() );
            return Cpl::TShell::Command::eERROR_INVALID_ARGS;
        }
        mp_userMode.write( *maybeValue );
        outtext.format( "New thermostat mode = %s", tokens.getParameter( 2 ) );
        io &= context.writeFrame( outtext.getString() );
    }

    else if ( tokens.getParameter( 1 )[0] == 'f' && numParms == 3 )
    {
        auto maybeValue = Storm::Type::FanMode::_from_string_nothrow( tokens.getParameter( 2 ) );
        if ( !maybeValue )
        {
            outtext.format( "Specified fan mode (%s) is not a valid setting", tokens.getParameter( 2 ) );
            context.writeFrame( outtext.getString() );
            return Cpl::TShell::Command::eERROR_INVALID_ARGS;
        }
        mp_fanMode.write( *maybeValue );
        outtext.format( "New fan mode = %s", tokens.getParameter( 2 ) );
        io &= context.writeFrame( outtext.getString() );
    }

    else if ( tokens.getParameter( 1 )[0] == 's' && numParms == 4 )
    {
        double cool, heat;
        if ( Cpl::Text::a2d( cool, tokens.getParameter( 2 ) ) == false ||
             Cpl::Text::a2d( heat, tokens.getParameter( 3 ) ) == false )
        {
            outtext.format( "Specified setpoints (c=%s, h=%s) is not a valid settings", tokens.getParameter( 2 ), tokens.getParameter( 3 ) );
            context.writeFrame( outtext.getString() );
            return Cpl::TShell::Command::eERROR_INVALID_ARGS;
        }
        mp_setpoints.write( (float) cool, (float) heat );

        // Read back the setpoint since MP enforces min/max/deltas
        float actualCool, actualHeat;
        mp_setpoints.read( actualCool, actualHeat );
        outtext.format( "CoolSetpt=%0.02f, HeatSetpt=%0.02f", actualCool, actualHeat );
        io &= context.writeFrame( outtext.getString() );
    }

    // Not a valid sub-command
    else
    {
        return Cpl::TShell::Command::eERROR_BAD_SYNTAX;
    }

    // If I get here -->the command syntax was correct
    return io ? Command::eSUCCESS : Command::eERROR_IO;
}



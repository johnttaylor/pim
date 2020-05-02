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

#include "WhiteBox.h"
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
WhiteBox::WhiteBox( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
    :Command( commandList, STORMTSHELLCMD_CMD_WB_ )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T WhiteBox::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
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

    Storm::Type::WhiteBox_T settings;
    if ( Cpl::Dm::ModelPoint::IS_VALID( mp_whiteBox.read( settings ) ) == false )
    {
        context.writeFrame( "The 'whiteBox' model point is NOT in the valid state" );
        return Cpl::TShell::Command::eERROR_FAILED;
    }

    // Display current settings
    if ( numParms == 1 )
    {
        outtext.format( "enot = %s (Equip Min Off Time)", settings.defeatEquipMinOffTime ? "DISABLED" : "enabled" );
        io &= context.writeFrame( outtext.getString() );
        outtext.format( "acc  = %s (Abort Current on/off Cycle)", settings.abortOnOffCycle ? "TRUE" : "false" );
        io &= context.writeFrame( outtext.getString() );
    }

    // enot (Equipment Minimum Off Time)
    else if ( tokens.getParameter( 1 )[0] == 'e' && numParms == 3 )
    {
        settings.defeatEquipMinOffTime = tokens.getParameter( 2 )[0] == 'd' ? true : false;
        mp_whiteBox.write( settings );
        outtext.format( "enot = %s (Equip Min Off Time)", settings.defeatEquipMinOffTime ? "DISABLED" : "enabled" );
        io &= context.writeFrame( outtext.getString() );
    }

    // acc (Abort Current on/off Cycle)
    else if ( tokens.getParameter( 1 )[0] == 'a' && numParms == 3 )
    {
        settings.abortOnOffCycle = tokens.getParameter( 2 )[0] == 't' ? true : false;
        mp_whiteBox.write( settings );
        outtext.format( "acc  = %s (Abort Current on/off Cycle)", settings.abortOnOffCycle ? "TRUE" : "false" );
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



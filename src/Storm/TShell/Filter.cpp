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

#include "Filter.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Storm/Thermostat/ModelPoints.h"
#include <string.h>         


///
using namespace Storm::TShell;

///////////////////////////
Filter::Filter( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
    :Command( commandList, STORMTSHELLCMD_CMD_FILTER_ )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Filter::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
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

    // Display current settings
    if ( numParms == 1 )
    {
        Storm::Dm::MpSimpleAlarm::Data          alert;
        uint32_t                                maxHours;
        Cpl::System::ElapsedTime::Precision_T   elaspedTime;
        if ( !mp_airFilterAlert.read( alert ) ||
             !mp_maxAirFilterHours.read( maxHours ) ||
             !mp_airFilterOperationTime.read( elaspedTime ) )
        {
            io &= context.writeFrame( "One more MP are invalid" );
        }
        else
        {
            if ( alert.active == false )
            {
                outtext.format( "alert.active   = false" );
                io &= context.writeFrame( outtext.getString() );
                outtext.format( "elapsed time   = " );
                Cpl::Text::formatPrecisionTimeStamp( outtext, elaspedTime, true, true );
                io &= context.writeFrame( outtext.getString() );
                outtext.format( "change time    = " );
                Cpl::Text::formatPrecisionTimeStamp( outtext, { maxHours * 60 * 60,0 }, true, true );
                io &= context.writeFrame( outtext.getString() );
            }
            else
            {
                outtext.format( "alert.active   = TRUE" );
                io &= context.writeFrame( outtext.getString() );
                outtext.format( "alert.acked    = %s", alert.acked ? "TRUE" : "false" );
                io &= context.writeFrame( outtext.getString() );
                outtext.format( "alert.critical = %s", alert.critical ? "TRUE" : "false" );
                io &= context.writeFrame( outtext.getString() );
                outtext.format( "change time    = " );
                Cpl::Text::formatPrecisionTimeStamp( outtext, { maxHours * 60 * 60,0 }, true, true );
                io &= context.writeFrame( outtext.getString() );
            }
        }
    }

    // Clear Alert
    else if ( numParms == 2 && tokens.getParameter( 1 )[0] == 'c' )
    {
        mp_airFilterAlert.setAlarm( false );
        io &= context.writeFrame( "Air Filter Alert cleared." );
    }

    // Acknowledge Alert
    else if ( numParms == 2 && tokens.getParameter( 1 )[0] == 'a' )
    {
        mp_airFilterAlert.acknowledgeAlarm();
        io &= context.writeFrame( "Air Filter Alert acknowledged." );
    }

    // Set change time
    else if ( numParms == 3 && tokens.getParameter( 1 )[0] == 's' )
    {
        unsigned long hours;
        if ( Cpl::Text::a2ul( hours, tokens.getParameter( 2 ) ) == false )
        {
            outtext.format( "Specified hours (c=%s) is not a valid settings", tokens.getParameter( 2 ) );
            context.writeFrame( outtext.getString() );
            return Cpl::TShell::Command::eERROR_INVALID_ARGS;
        }
        mp_maxAirFilterHours.write( hours );
        outtext.format( "Filter Change time (in fan operation hours) set to: %lu", hours );
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



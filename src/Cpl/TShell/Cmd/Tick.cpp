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

#include "Tick.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/System/ElapsedTime.h"
#include <string.h>         


///
using namespace Cpl::TShell::Cmd;

///////////////////////////
Tick::Tick( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
    :Command( commandList, CPLTSHELLCMD_CMD_TICK_ )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Tick::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    //Cpl::Text::String&              token    = context.getTokenBuffer();
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    unsigned                        numParms = tokens.numParameters();
    bool                            io       = true;

    // Error checking
    if ( numParms > 2 )
    {
        return Cpl::TShell::Command::eERROR_EXTRA_ARGS;
    }

    // Show current time (simulated and real)
    if ( numParms == 1 )
    {
        Cpl::System::ElapsedTime::Precision_T realTime = Cpl::System::ElapsedTime::precisionInRealTime();
        Cpl::System::ElapsedTime::Precision_T simTime  = Cpl::System::ElapsedTime::precision();
        outtext = "SimTime: ";
        Cpl::Text::formatPrecisionTimeStamp( outtext, simTime, true, true );
        outtext += ", RealTime: ";
        Cpl::Text::formatPrecisionTimeStamp( outtext, realTime, true, true );
        io  &= context.writeFrame( outtext.getString() );
    }

    // Go to a absolute tick count
    else if ( tokens.getParameter( 1 )[0] == '@' )
    {
        unsigned long long absoluteMsec = 0;
        if ( !Cpl::Text::a2ull( absoluteMsec, tokens.getParameter( 1 ) + 1 ) )
        {
            outtext.format( "Invalid '@mm' argument (%s)", tokens.getParameter( 1 ) );
            context.writeFrame( outtext );
            return Cpl::TShell::Command::eERROR_INVALID_ARGS;
        }

        Cpl::System::ElapsedTime::Precision_T simTime  = Cpl::System::ElapsedTime::precision();
        unsigned long long  simMsec = ( unsigned long long ) simTime.m_seconds * 1000 + simTime.m_thousandths;
        if ( simMsec < absoluteMsec )
        {
            Cpl::System::SimTick::advance( ( size_t) ( absoluteMsec - simMsec ) );
        }

        simTime  = Cpl::System::ElapsedTime::precision();
        Cpl::Text::formatPrecisionTimeStamp( outtext, simTime, true );
        io  &= context.writeFrame( outtext.getString() );
    }

    // Advance N ticks
    else if ( tokens.getParameter( 1 )[0] == '+' )
    {
        unsigned long nticks = 0;
        if ( !Cpl::Text::a2ul( nticks, tokens.getParameter( 1 ) + 1 ) )
        {
            outtext.format( "Invalid '+nn' argument (%s)", tokens.getParameter( 1 ) );
            context.writeFrame( outtext );
            return Cpl::TShell::Command::eERROR_INVALID_ARGS;

        }

        Cpl::System::SimTick::advance( nticks );
        Cpl::System::ElapsedTime::Precision_T simTime  = Cpl::System::ElapsedTime::precision();
        Cpl::Text::formatPrecisionTimeStamp( outtext, simTime, true );
        io  &= context.writeFrame( outtext.getString() );
    }

    // Bad syntax
    else
    {
        return Cpl::TShell::Command::eERROR_BAD_SYNTAX;
    }

    // If I get here -->the command syntax was correct
    return io ? Command::eSUCCESS : Command::eERROR_IO;
}


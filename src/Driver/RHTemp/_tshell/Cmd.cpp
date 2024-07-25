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

#include "Cmd.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/System/Api.h"
#include <string.h>         

///
using namespace Driver::RHTemp;


///////////////////////////
TShellCmd::TShellCmd( Driver::RHTemp::Api&                       driver,
                      Cpl::Container::Map<Cpl::TShell::Command>& commandList,
                      Cpl::TShell::Security::Permission_T        minPermLevel ) noexcept
    : Command( commandList, verb, minPermLevel )
    , m_driver( driver )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T TShellCmd::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    bool                            io       = true;

    // Start
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "start" ) == 0 )
    {
        bool result = m_driver.start();
        if ( result )
        {
            io = context.writeFrame( "Driver started" );
            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }
        else
        {
            context.writeFrame( "Driver FAILED to started" );
            return Command::eERROR_FAILED;
        }
    }

    // Stop
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "stop" ) == 0 )
    {
        m_driver.stop();
        io = context.writeFrame( "Driver stopped" );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Blocking Sample
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "sample" ) == 0 )
    {
        float rh;
        float tempC;
        bool result = m_driver.sample( rh, tempC );
        if ( result )
        {
            outtext.format( "RH=%g%c, Temp=%g'C.", rh, '%', tempC );
            io = context.writeFrame( outtext.getString() );
            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }
        else
        {
            context.writeFrame( "Driver FAILED to sample RH/Temp" );
            return Command::eERROR_FAILED;
        }
    }

    // Start async-Sample
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "begin" ) == 0 )
    {
        Driver::RHTemp::Api::SamplingState_T result = m_driver.startSample();
        if ( result == Driver::RHTemp::Api::eSAMPLING )
        {
            io = context.writeFrame( "Async Sampling started..." );
            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }
        else
        {
            outtext.format( "Failed to start async sampling (%d)", result );
            context.writeFrame( outtext.getString() );
            return Command::eERROR_FAILED;
        }
    }

    // Read async result/state
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "read" ) == 0 )
    {
        float rh;
        float temp;
        Driver::RHTemp::Api::SamplingState_T result = m_driver.getSample( rh, temp );
        if ( result == Driver::RHTemp::Api::eSAMPLE_READY )
        {
            outtext.format( "Sample Result: RH=%g%c, Temp=%g'C.", rh, '%', temp );
            io = context.writeFrame( outtext.getString() );
            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }
        else
        {
            outtext.format( "Sample state=%s",
                            result == Driver::RHTemp::Api::eERROR ? "eERROR" :
                            result == Driver::RHTemp::Api::eSAMPLING ? "eSAMPLING" :
                            "eNOT_STARTED" );
            io &= context.writeFrame( outtext.getString() );
            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }
    }

    // Heater on/off
    if ( tokens.numParameters() == 3 && *(tokens.getParameter( 1 )) == 'h' )
    {
        bool enabled = strcmp( tokens.getParameter( 2 ), "on" ) == 0 ? true : false;
        if ( m_driver.setHeaterState( enabled ) )
        {
            outtext.format( "Heater %s", enabled ? "ENABLED" : "disabled" );
            io &= context.writeFrame( outtext.getString() );
            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }

        context.writeFrame( "Failed to set/change the Heater state" );
        return Command::eERROR_FAILED;
    }

    // If I get here -->the argument(s) where bad
    return Cpl::TShell::Command::eERROR_INVALID_ARGS;
}


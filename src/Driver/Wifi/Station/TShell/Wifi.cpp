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

#include "Wifi.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/Text/atob.h"
#include "Cpl/System/Trace.h"
#include <string.h>

///
using namespace Driver::Wifi::Station;
using namespace Driver::Wifi::Station::TShell;

#define SECT_   "Driver::Wifi::Station::TShell"

///////////////////////////
Wifi::Wifi( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
    : Cpl::TShell::Cmd::Command( commandList, DRIVERWIFISTATIONTSHELL_CMD_WIFI_ )
{
}

static void wifiStatusCHange( Connection::State_T currentState )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("WIFI Network status change: %s", Connection::toString( currentState )) );

}

///////////////////////////
Cpl::TShell::Command::Result_T Wifi::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    unsigned                        numParms = tokens.numParameters();
    bool                            io       = true;

    // Display Status
    if ( numParms == 1 )
    {
        Connection::State_T linkState = Connection::getState();
        outtext.format( "WIFI Network status: %s", Connection::toString( linkState ) );
        io &= context.writeFrame( outtext );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Connect to the network
    if ( numParms == 4  )
    {
        // Convert the auth-method to binary
        int authMethod;
        if ( !Cpl::Text::a2i( authMethod, tokens.getParameter( 1 ) ) || authMethod < 0 || authMethod  > Connection::eWPA2_MIXED_PSK )
        {
            outtext.format( "Invalid auth-method (%d). Type 'help %' for list of auth-methods", 
                            DRIVERWIFISTATIONTSHELL_CMD_WIFI_, 
                            tokens.getParameter( 1 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        Connection::start( tokens.getParameter( 2 ), 
                           tokens.getParameter( 3 ), 
                           (Connection::Authentication_T) authMethod,
                           wifiStatusCHange );
        outtext.format( "Connecting to WIFI...  SSID=%s, passwd=%s, auth-method=%d", 
                        tokens.getParameter( 2 ), 
                        tokens.getParameter( 3 ), 
                        authMethod );
        io &= context.writeFrame( outtext );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Disconnect from network
    else  if ( numParms == 2 && *tokens.getParameter( 1 ) == 'd' )
    {
        Connection::stop();
        outtext.format( "Disconnecting from the WIFI network..." );
        io &= context.writeFrame( outtext );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // If I get here the command failed!
    return Command::eERROR_BAD_SYNTAX;
}
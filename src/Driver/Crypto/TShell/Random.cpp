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

#include "Random.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include <string.h>         
#include "Driver/Crypto/Random.h"

///
using namespace Driver::Crypto::TShell;

#define MAX_LENGTH      256

///////////////////////////
Random::Random( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
                Cpl::TShell::Security::Permission_T        minPermLevel ) noexcept
    :Command( commandList, verb, minPermLevel )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Random::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    bool                            io       = true;

    // Error checking
    if ( tokens.numParameters() != 2 )
    {
        return Cpl::TShell::Command::eERROR_INVALID_ARGS;
    }

    // Parse <numbytes>
    unsigned numBytes =0;
    if ( !Cpl::Text::a2ui( numBytes, tokens.getParameter( 1 ) ) && numBytes > MAX_LENGTH )
    {
        outtext.format( "Invalid (or too large) <numbytes> argument (%s)", tokens.getParameter( 1 ) );
        context.writeFrame( outtext );
        return Cpl::TShell::Command::eERROR_INVALID_ARGS;
    }
    
    // Generate the random number.
    uint8_t ranNum[MAX_LENGTH];
    if ( Driver::Crypto::generateRandom( ranNum, numBytes ) != DRIVER_CRYPTO_SUCCESS )
    {
        outtext.format( "Failed (numBytes=%s)", tokens.getParameter( 1 ) );
        context.writeFrame( outtext );
        return Cpl::TShell::Command::eERROR_FAILED;
    }

    // Convert the random number to HEX-ASCII
    if ( !Cpl::Text::bufferToAsciiHex( ranNum, numBytes, outtext ) )
    {
        outtext.format( "Failed (numBytes=%s) to result to HEXASCII", tokens.getParameter( 1 ) );
        context.writeFrame( outtext );
        return Cpl::TShell::Command::eERROR_FAILED;
    }

    // If I get here -->everything worked
    io &= context.writeFrame( outtext );
    return io ? Command::eSUCCESS : Command::eERROR_IO;
}


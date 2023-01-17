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

#include "Wait.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/System/Api.h"
#include <string.h>         


///
using namespace Cpl::TShell::Cmd;

///////////////////////////
Wait::Wait( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
            Security::Permission_T                     minPermLevel ) noexcept
    :Command( commandList, verb, minPermLevel )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Wait::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();

    // Error checking
    if ( tokens.numParameters() != 2 )
    {
        return Cpl::TShell::Command::eERROR_INVALID_ARGS;
    }

    // Get the wait time		
    unsigned long  waitTimeMs;
    if ( Cpl::Text::a2ul( waitTimeMs, tokens.getParameter( 1 ) ) )
    {
        outtext.format( "Waiting %lu msec ...", waitTimeMs );
        bool io = context.writeFrame( outtext.getString() );
        Cpl::System::Api::sleep( waitTimeMs );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // If I get here -->the argument(s) where bad
    return Cpl::TShell::Command::eERROR_INVALID_ARGS;
}


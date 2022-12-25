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

#include "User.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"


///
using namespace Cpl::TShell::Cmd;


///////////////////////////
User::User( Cpl::Container::Map<Cpl::TShell::Command>& commandList, Cpl::TShell::Security& validator ) noexcept
    : Command( commandList, CPLTSHELLCMD_CMD_USER_, Cpl::TShell::Security::ePUBLIC ) // ALWAYS a public command
    , m_validator( validator )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T User::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );

    // Logout
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "logout" ) == 0 )
    {
        context.setUserPermissionLevel( Security::ePUBLIC );
    }

    // Login
    else if ( tokens.numParameters() == 4 && strcmp( tokens.getParameter( 1 ), "login" ) == 0 )
    {
        context.setUserPermissionLevel( m_validator.validateCredentials( tokens.getParameter( 2 ), tokens.getParameter( 3 ) ) );
    }

    // ALWAYS return success AND intentionally provide NO FEEDBACK to the user (though the user will get a 'prompt' when logged)
    return Command::eSUCCESS;
}


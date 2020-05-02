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

#include "Help.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"


///
using namespace Cpl::TShell::Cmd;


static void outputCmdHelp_( Cpl::TShell::Context_& context, Cpl::TShell::Command& cmd, bool& io, bool includeDetails );
static void outputLongText_( Cpl::TShell::Context_& context, bool& io, const char* text );


///////////////////////////
Help::Help( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
	:Command( commandList, "help" )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Help::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
	Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );

	// Error checking
	if ( tokens.numParameters() > 2 )
	{
		return Command::eERROR_EXTRA_ARGS;
	}

	// Housekeeping
	Cpl::Container::Map<Cpl::TShell::Command>&  cmdList = context.getCommands();
	bool                                        io      = true;
	Cpl::TShell::Command*                       cmdPtr;
	Cpl::Container::KeyLiteralString            verb( tokens.getParameter( 1 ) );

	// Command specific help
	if ( ( cmdPtr=cmdList.find( verb ) ) )
	{
		outputCmdHelp_( context, *cmdPtr, io, true );
	}

	// List the commands
	else
	{
		cmdPtr = cmdList.first();

		while ( cmdPtr && io == true )
		{
			outputCmdHelp_( context, *cmdPtr, io, tokens.numParameters() == 2 );
			cmdPtr = cmdList.next( *cmdPtr );
		}
	}

	return io ? Command::eSUCCESS : Command::eERROR_IO;
}



void outputCmdHelp_( Cpl::TShell::Context_& context, Cpl::TShell::Command& cmd, bool& io, bool includeDetails )
{
	outputLongText_( context, io, cmd.getUsage() );
	if ( includeDetails )
	{
		const char* details = cmd.getHelp();
		if ( details && *details != '\0' )
		{
			outputLongText_( context, io, details );
			io &= context.writeFrame( " " );
		}
	}
}


void outputLongText_( Cpl::TShell::Context_& context, bool& io, const char* text )
{
	const char* ptr = text;
	while ( *ptr )
	{
		if ( *ptr == '\n' )
		{
			io &= context.writeFrame( text, ptr - text );
			text = ++ptr;
		}
		else
		{
			ptr++;
		}
	}

	size_t numBytes = ptr - text;
	if ( numBytes )
	{
		io &= context.writeFrame( text, numBytes );
	}
}

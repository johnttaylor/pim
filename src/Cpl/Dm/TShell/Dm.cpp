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

#include "Dm.h"
#include "Cpl/Dm/ModelPoint.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include <string.h>

///
using namespace Cpl::Dm::TShell;


///////////////////////////
Dm::Dm( Cpl::Container::Map<Cpl::TShell::Command>& commandList, Cpl::Dm::ModelDatabaseApi& modelDatabase, const char* cmdNameAndDatabaseNumber ) noexcept
	: Cpl::TShell::Cmd::Command( commandList, cmdNameAndDatabaseNumber )
	, m_database( modelDatabase )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Dm::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
	const char* subCmd = Cpl::Text::stripSpace( Cpl::Text::stripNotSpace( cmdString ) );

	// Initial check for not-enough-tokens
	if ( subCmd == 0 || *subCmd == '\0' )
	{
		return Command::eERROR_MISSING_ARGS;
	}

	// LIST sub-command
	if ( strncmp( subCmd, "ls", 2 ) == 0 )
	{
		Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );

		// Too many args?
		if ( tokens.numParameters() > 4 )
		{
			return Command::eERROR_EXTRA_ARGS;
		}

		// Get the optional filter arg
		const char* filter = tokens.numParameters() == 3 ? tokens.getParameter( 2 ) : 0;

		// Walk the Model database
		Cpl::Dm::ModelPoint* point = m_database.getFirstByName();
		while ( point )
		{
			if ( filter == 0 || strstr( point->getName(), filter ) != 0 )
			{
				if ( !context.writeFrame( point->getName() ) )
				{
					return Command::eERROR_IO;
				}

			}
			point = m_database.getNextByName( *point );
		}

		// If I get here -->the command succeeded
		return Command::eSUCCESS;

	}

	// READ Sub-command
	else if ( strncmp( subCmd, "read", 4 ) == 0 )
	{
		Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );

		// Wrong number of args?
		if ( tokens.numParameters() < 2 )
		{
			return Command::eERROR_MISSING_ARGS;
		}
		else if ( tokens.numParameters() > 3 )
		{
			return Command::eERROR_EXTRA_ARGS;
		}

		// Look-up the model point
		Cpl::Dm::ModelPoint* point = m_database.lookupModelPoint( tokens.getParameter( 2 ) );
		if ( point == 0 )
		{
			return Command::eERROR_INVALID_ARGS;
		}

		// Generate the JSON object/string for the Model point
		bool				truncated;
		int					outlen;
		Cpl::Text::String&	outtext    = context.getOutputBuffer();
		char*				outptr     = outtext.getBuffer( outlen );
		if ( point->toJSON( outptr, outlen, truncated ) == false )
		{
			return Command::eERROR_FAILED;
		}

		// Output the JSON object
		return context.writeFrame( outtext ) ? Command::eSUCCESS : Command::eERROR_IO;
	}

	// WRITE sub-command
	else if ( strncmp( subCmd, "write", 5 ) == 0 )
	{
		// Find the start of the JSON object
		const char* json   = Cpl::Text::stripSpace( Cpl::Text::stripNotSpace( subCmd ) );
		if ( *json != '{' )
		{
			return Command::eERROR_INVALID_ARGS;
		}

		// Attempt to update the Model Point
		Cpl::Text::FString<128> errorMsg;
		if ( !m_database.fromJSON( json, &errorMsg ) )
		{
			context.writeFrame( errorMsg );
			return Command::eERROR_INVALID_ARGS;
		}
		return Command::eSUCCESS;
	}

	// TOUCH Sub-command
	else if (strncmp(subCmd, "touch", 5) == 0)
	{
		Cpl::Text::Tokenizer::TextBlock tokens(cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar());

		// Wrong number of args?
		if (tokens.numParameters() < 2)
		{
			return Command::eERROR_MISSING_ARGS;
		}
		else if (tokens.numParameters() > 3)
		{
			return Command::eERROR_EXTRA_ARGS;
		}

		// Look-up the model point
		Cpl::Dm::ModelPoint* point = m_database.lookupModelPoint(tokens.getParameter(2));
		if (point == 0)
		{
			return Command::eERROR_INVALID_ARGS;
		}

		// Call touch on the MP (to trigger change notification(s))
		point->touch();
		return Command::eSUCCESS;
	}

	// If I get here the command failed!
	return Command::eERROR_FAILED;
}
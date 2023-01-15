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

#include "Trace.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include <string.h>



///
using namespace Cpl::TShell::Cmd;
using namespace Cpl::TShell;


static const char* info2text_( Cpl::System::Trace::InfoLevel_T level );
static Cpl::System::Trace::InfoLevel_T text2info_( const char* level, bool& errorOccurred );
static void dummy_( const char* f1, const char* f2, const char* f3, const char* f4 );



///////////////////////////
Trace::Trace( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
			  Security::Permission_T                     minPermLevel ) noexcept
	:Command( commandList, verb, minPermLevel )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T Trace::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
	Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
	Cpl::Text::String&              token    = context.getTokenBuffer();
	Cpl::Text::String&              outtext  = context.getOutputBuffer();
	unsigned                        numParms = tokens.numParameters();
	bool                            io       = true;

	// Do nothing if trace was not compiled in
	if ( CPL_SYSTEM_TRACE_IS_COMPILED() == false )
	{
		return context.writeFrame( "CPL Tracing was NOT ACTIVATED at Compiled time" ) ? Command::eSUCCESS : Command::eERROR_IO;
	}


	// Output trace status
	if ( numParms == 1 )
	{
		// Output Enable sections
		io  &= context.writeFrame( " " );
		io  &= context.writeFrame( "TRACE: Currently Enabled Sections:" );
		io  &= context.writeFrame( "----------------------------------" );
		unsigned    count = CPL_SYSTEM_TRACE_GET_SECTIONS( token );
		const char* ptr   = Cpl::Text::stripSpace( token );
		while ( count-- )
		{
			const char* next = Cpl::Text::stripNotSpace( ptr );
			io &= context.writeFrame( ptr, next - ptr );
			ptr = Cpl::Text::stripSpace( next );
		}

		// Output Enable Thread Filters
		io  &= context.writeFrame( " " );
		io  &= context.writeFrame( "TRACE: Currently Enabled Thread Filters:" );
		io  &= context.writeFrame( "----------------------------------------" );
		count = CPL_SYSTEM_TRACE_GET_THREAD_FILTERS( token );
		ptr   = Cpl::Text::stripSpace( token );
		while ( count-- )
		{
			const char* next = Cpl::Text::stripNotSpace( ptr );
			io &= context.writeFrame( ptr, next - ptr );
			ptr = Cpl::Text::stripSpace( next );
		}

		// Runtime state
		outtext.format( "TRACE: Runtime state:= %s, Info Level:= %s", CPL_SYSTEM_TRACE_IS_ENABLED() ? "ENABLED" : "DISABLED", info2text_( CPL_SYSTEM_TRACE_GET_INFO_LEVEL() ) );
		io &= context.writeFrame( " " );
		io &= context.writeFrame( outtext );
		return io ? Command::eSUCCESS : Command::eERROR_IO;
	}


	// Global enable/disable
	if ( numParms == 2 && strcmp( tokens.getParameter( 1 ), "on" ) == 0 )
	{
		CPL_SYSTEM_TRACE_ENABLE();
		return Command::eSUCCESS;
	}
	if ( numParms == 2 && strcmp( tokens.getParameter( 1 ), "off" ) == 0 )
	{
		CPL_SYSTEM_TRACE_DISABLE();
		return Command::eSUCCESS;
	}


	// Relocate Trace output
	if ( numParms == 2 && strcmp( tokens.getParameter( 1 ), "here" ) == 0 )
	{
		CPL_SYSTEM_TRACE_REDIRECT( outfd );
		return Command::eSUCCESS;
	}
	if ( numParms == 2 && strcmp( tokens.getParameter( 1 ), "revert" ) == 0 )
	{
		CPL_SYSTEM_TRACE_REVERT();
		return Command::eSUCCESS;
	}


	// Change info level
	if ( numParms == 3 && strcmp( tokens.getParameter( 1 ), "level" ) == 0 )
	{
		bool errorOccurred = false;
		CPL_SYSTEM_TRACE_SET_INFO_LEVEL( text2info_( tokens.getParameter( 2 ), errorOccurred ) );
		return errorOccurred ? Command::eERROR_INVALID_ARGS : Command::eSUCCESS;
	}

	// Enable/Disable sections
	if ( numParms > 3 && strcmp( tokens.getParameter( 1 ), "section" ) == 0 )
	{
		numParms    -= 3;
		unsigned idx = 3;
		if ( strcmp( tokens.getParameter( 2 ), "on" ) == 0 )
		{
			while ( numParms-- )
			{
				CPL_SYSTEM_TRACE_ENABLE_SECTION( tokens.getParameter( idx ) );
				idx++;
			}
		}
		else if ( strcmp( tokens.getParameter( 2 ), "off" ) == 0 )
		{
			while ( numParms-- )
			{
				CPL_SYSTEM_TRACE_DISABLE_SECTION( tokens.getParameter( idx ) );
				idx++;
			}
		}
		else
		{
			// If I get here -->the argument(s) where bad
			return Command::eERROR_INVALID_ARGS;
		}

		// Enable/disable trace sections WORKED!
		return Command::eSUCCESS;
	}

	// Set/remove thread filters
	if ( numParms < 7 && strcmp( tokens.getParameter( 1 ), "threadfilters" ) == 0 )
	{
		const char* f1 = 0;
		const char* f2 = 0;
		const char* f3 = 0;
		const char* f4 = 0;

		// Get thread filters (max of 4)
		if ( numParms >= 3 )
		{
			f1 = tokens.getParameter( 2 );
		}
		if ( numParms >= 4 )
		{
			f2 = tokens.getParameter( 3 );
		}
		if ( numParms >= 5 )
		{
			f3 = tokens.getParameter( 4 );
		}
		if ( numParms >= 6 )
		{
			f4 = tokens.getParameter( 5 );
		}

		// Set thread filters
		if ( numParms != 2 )
		{
			CPL_SYSTEM_TRACE_SET_THREAD_4FILTERS( f1, f2, f3, f4 );
		}

		// Clear thread filters
		else
		{
			CPL_SYSTEM_TRACE_CLEAR_THREAD_FILTER();
		}

		// Klug:: Attempt to avoid unused-variable compiler warnings when TRACE is disable at compile time. 
		dummy_( f1, f2, f3, f4 );
		return Command::eSUCCESS;
	}


	// If I get here -->the argument(s) where bad
	return Command::eERROR_INVALID_ARGS;
}


////////////////////////////////////////////////////////////////////////////
const char* info2text_( Cpl::System::Trace::InfoLevel_T level )
{
	switch ( level )
	{
	case Cpl::System::Trace::eNONE:    return "NONE";
	case Cpl::System::Trace::eBRIEF:   return "BRIEF";
	case Cpl::System::Trace::eINFO:    return "INFO";
	case Cpl::System::Trace::eVERBOSE: return "VERBOSE";
	case Cpl::System::Trace::eMAX:     return "MAX";
	}

	return "??UNKNOWN??";
}

Cpl::System::Trace::InfoLevel_T text2info_( const char* level, bool& errorOccurred )
{
    // Trap NULL string
    if ( level == 0 )
    {
        errorOccurred = true;
        return CPL_SYSTEM_TRACE_GET_INFO_LEVEL();
    }

    errorOccurred = false;

	if ( strcmp( level, "none" ) == 0 )
	{
		return Cpl::System::Trace::eNONE;
	}
	else if ( strcmp( level, "brief" ) == 0 )
	{
		return Cpl::System::Trace::eBRIEF;
	}
	else if ( strcmp( level, "info" ) == 0 )
	{
		return Cpl::System::Trace::eINFO;
	}
	else if ( strcmp( level, "verbose" ) == 0 )
	{
		return Cpl::System::Trace::eVERBOSE;
	}
	else if ( strcmp( level, "max" ) == 0 )
	{
		return Cpl::System::Trace::eMAX;
	}

	// If I get here -->there was bad token -->so no change in level
	errorOccurred = true;
	return CPL_SYSTEM_TRACE_GET_INFO_LEVEL();
}


void dummy_( const char* f1, const char* f2, const char* f3, const char* f4 )
{
	bool dummy;
	text2info_( 0, dummy );
}

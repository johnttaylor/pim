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

#include "TPrint.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"

///
using namespace Cpl::TShell::Cmd;
using namespace Cpl::TShell;



///////////////////////////
TPrint::TPrint( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
	:Command( commandList, CPLTSHELLCMD_CMD_TPRINT_ )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T TPrint::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
	Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
	Cpl::Text::String&              outtext = context.getOutputBuffer();

	// Error checking
	if ( tokens.numParameters() > 2 )
	{
		return Command::eERROR_EXTRA_ARGS;
	}

	// Generated expanded text
	Cpl::Text::formatPrecisionTimeStamp( outtext, Cpl::System::ElapsedTime::precision() );
	outtext += "  ";
	outtext += tokens.getParameter( 1 );

	// Output the final text
	return context.writeFrame( outtext ) ? Command::eSUCCESS : Command::eERROR_IO;
}

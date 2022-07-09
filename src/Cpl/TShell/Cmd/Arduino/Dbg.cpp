/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Dbg.h"
#include "debug.h"
#include "Cpl/Text/String.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"

///
using namespace Cpl::TShell::Cmd::Arduino;



///////////////////////////
Dbg::Dbg( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
:Cpl::TShell::Cmd::Command(commandList, "dbg")
    {
    }


/////////////////////////////////////////////////////////
Cpl::TShell::Command::Result_T Dbg::execute( Context_& context, char* rawCmdString, Cpl::Io::Output& outfd ) noexcept
    {
    Cpl::Text::Tokenizer::TextBlock tokens( rawCmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    //Cpl::Text::String&            token    = context.getTokenBuffer();
    //Cpl::Text::String&            outtext  = context.getOutputBuffer();
    unsigned                        numParms = tokens.numParameters();

    // Error checking
    if ( numParms > 2 )
        {
        return Command::eERROR_EXTRA_ARGS;
        }

    // Global enable/disable
    if ( numParms == 2 && strcmp(tokens.getParameter(1), "mem") == 0 )
        {
        dbgMemInfo();   // Outputs over the 'native' arduino serial port which in theory is the same serial port being used for the DAC shell.
        return Command::eSUCCESS;
        }

    return eERROR_INVALID_ARGS;
    }

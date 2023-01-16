#ifndef BobCommand_h
#define BobCommand_h
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

#include "Cpl/TShell/Cmd/Command.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/Text/atob.h"
#include "ModelPoints.h"


/*---------------------------------------------------------------------------*/
class BobCommand : public Cpl::TShell::Cmd::Command
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "bob";

    /// The command usage string
    static constexpr const char* usage = "bob on|off\n" 
                                         "bob on|off delay";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                                                          1         2         3         4         5         6         7         8
                                                 12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp = "  Sets the Bob's trace output to on/off and the delay time between incrementing\n"
                                                "  Bob's internal counter.";

public:
    /// Constructor. Default to: no security
    BobCommand( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
                Cpl::TShell::Security::Permission_T        minPerm = Cpl::TShell::Security::ePUBLIC ) noexcept
        :Command( commandList, verb, minPerm )
    {
    }

public:
    /// The terse help for the Bob command
    const char* getUsage() const noexcept { return usage; }

    /// The verbose help for the Bob Command
    const char* getHelp() const noexcept { return detailedHelp; }


public:
    /// See Cpl::TShell::Command
    Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
    {
        // Tokenize the command input string. This is a destructive parse in that it modifies 'cmdString'
        // NOTE: The 'TextBlock' tokenizer supports quoted tokens.
        Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );

        // Get work/temporary buffers.  Note: These buffers are shared across all commands
        // NOTE: Because stack space can be very limited on embedded targets, it
        //       is recommended to use the shared buffer instead of allocating
        //       (what could be relative large) buffer(s) on the stack.
        Cpl::Text::String& token  = context.getTokenBuffer();
        Cpl::Text::String& outBuf = context.getOutputBuffer();

        // Error checking
        if ( tokens.numParameters() > 3 || tokens.numParameters() < 2 )
        {
            return Command::eERROR_BAD_SYNTAX;
        }

        // Set Trace output state
        token = tokens.getParameter( 1 );
        bool traceEnabled = token == "on" ? true : false;
        mp::enableBobsTrace.write( traceEnabled );
        outBuf.format( "Bob's output is: %s", traceEnabled ? "ENABLED" : "disabled" );
        context.writeFrame( outBuf.getString() );

        // Optionally Set the delay
        if ( tokens.numParameters() > 2 )
        {
            // Parse delay time
            unsigned long newdelay;
            if ( !Cpl::Text::a2ul( newdelay, tokens.getParameter( 2 ) ) )
            {
                outBuf.format( "Invalid 'delay' argument (%s)", tokens.getParameter( 2 ) );
                context.writeFrame( outBuf.getString() );
                return Command::eERROR_INVALID_ARGS;
            }

            // Have a valid delay time - update the model point
            mp::bobsDelayMs.write( newdelay );
            outBuf.format( "Bob's delay set to: %lu msecs", newdelay );
            context.writeFrame( outBuf.getString() );
        }

        return Command::eSUCCESS;
    }
};


#endif	// end header latch







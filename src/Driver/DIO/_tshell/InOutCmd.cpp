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

#include "InOutCmd.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/System/Api.h"
#include <string.h>         

///
using namespace Driver::DIO;


///////////////////////////
InOutCmd::InOutCmd( Cpl::Container::Map<Cpl::TShell::Command>& commandList,
                    Cpl::TShell::Security::Permission_T        minPermLevel ) noexcept
    : Command( commandList, verb, minPermLevel )
{
}

static Driver::DIO::InOut::Config_T inCfg_[OPTION_DRIVER_DIO_IN_OUT_CMD_MAX_IN_PINS];
static Driver::DIO::InOut::Config_T outCfg_[OPTION_DRIVER_DIO_IN_OUT_CMD_MAX_OUT_PINS];
static uint8_t                      numIns_;
static uint8_t                      numOuts_;

static bool parseCfg( unsigned                         numPins, 
                      Cpl::Text::Tokenizer::TextBlock& tokens, 
                      unsigned                         startParmIndex, 
                      Driver::DIO::InOut::Config_T     cfgArray[], 
                      Cpl::Text::String&               errMsg );


///////////////////////////
Cpl::TShell::Command::Result_T InOutCmd::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    bool                            io       = true;

    // Start
    if ( tokens.numParameters() >= 4 && strcmp( tokens.getParameter( 1 ), "start" ) == 0 )
    {
        // Number of Inputs
        unsigned numInputs;
        if ( !Cpl::Text::a2ui( numInputs, tokens.getParameter( 2 ) ) || numInputs > OPTION_DRIVER_DIO_IN_OUT_CMD_MAX_IN_PINS )
        {
            outtext.format( "ERROR: Unable to parse 'numInputs' or the value is too large (%s)", tokens.getParameter( 2 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        // Number of outputs
        unsigned startIndex = numInputs * 2 + 3;
        if ( tokens.numParameters() < startIndex + 1)
        {
            context.writeFrame( "ERROR: Cannot parse 'numOutputs' - insufficient parameters" );
            return Command::eERROR_INVALID_ARGS;
        }
        unsigned numOutputs;
        if ( !Cpl::Text::a2ui( numOutputs, tokens.getParameter( startIndex ) ) || numOutputs > OPTION_DRIVER_DIO_IN_OUT_CMD_MAX_OUT_PINS )
        {
            outtext.format( "ERROR: Unable to parse 'numOutputs' or the value is too large (%s)", tokens.getParameter( startIndex ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        // Parse Input config
        outtext = "ERROR: Input Cfg: ";
        if ( !parseCfg( numInputs, tokens, 3, inCfg_, outtext ) )
        {
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        // Parse Output config
        outtext = "ERROR: Output Cfg: ";
        if ( !parseCfg( numOutputs, tokens, startIndex+1, outCfg_, outtext ) )
        {
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        // Start the driver
        if ( !InOut::start( numInputs, inCfg_, numOutputs, outCfg_ ) )
        {
            context.writeFrame( "ERROR: Failed to start the driver" );
            return Command::eERROR_FAILED;
        }
        numIns_  = numInputs;
        numOuts_ = numOutputs;
        outtext.format( "Driver Started. NumIn=%u, NumOut=%u", numInputs, numOutputs );
        io &= context.writeFrame( outtext );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Stop
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "stop" ) == 0 )
    {
        InOut::stop();
        io = context.writeFrame( "Driver stopped" );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Write by bit
    if ( tokens.numParameters() == 4 && *(tokens.getParameter( 1 )) == 'w' )
    {
        unsigned pinIdx;
        if ( !Cpl::Text::a2ui( pinIdx, tokens.getParameter( 2 ) ) )
        {
            outtext.format( "ERROR: Unable to parse '<pin-dx>'  (%s)", tokens.getParameter( 2 ) );
            context.writeFrame( outtext );
            return Command::eERROR_INVALID_ARGS;
        }

        bool bitValue = false;
        if ( *(tokens.getParameter( 3 )) == '1' )
        {
            bitValue = true;
        }
        if ( !Driver::DIO::InOut::setOutput( pinIdx, bitValue ) )
        {
            outtext.format( "Write failed. Pin index %u, val %d", pinIdx, bitValue ? 1 : 0 );
            context.writeFrame( outtext );
            return Command::eERROR_FAILED;
        }
        outtext.format( "Output Pin index %u set to %d", pinIdx, bitValue ? 1 : 0 );
        io &= context.writeFrame( outtext );
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // Current state
    if ( tokens.numParameters() == 1 )
    {
        for ( unsigned idx=0; idx < numIns_ && io; idx++ )
        {
            bool inVal;
            if ( !Driver::DIO::InOut::getInput( idx, inVal ) )
            {
                outtext.format( "Failed reading Input Pin Idx %u (PinID %02d)", idx, inCfg_[idx].pin );
                context.writeFrame( outtext );
                return Command::eERROR_FAILED;
            }
            outtext.format( "Input Pin Index  %u (%02d):  %d", idx, inCfg_[idx].pin, inVal );
            io &= context.writeFrame( outtext );
        }

        io &= context.writeFrame( "" );
        for ( unsigned idx=0; idx < numOuts_ && io; idx++ )
        {
            bool cmdVal;
            if ( !Driver::DIO::InOut::getOutput( idx, cmdVal ) )
            {
                outtext.format( "Failed reading Output Pin Idx %u (PinID %02d)", idx, outCfg_[idx].pin );
                context.writeFrame( outtext );
                return Command::eERROR_FAILED;
            }
            outtext.format( "Output Pin Index %u (%02d):  %d", idx, outCfg_[idx].pin, cmdVal );
            io &= context.writeFrame( outtext );
        }
        return io ? Command::eSUCCESS : Command::eERROR_IO;
    }

    // If I get here -->the argument(s) where bad
    return Cpl::TShell::Command::eERROR_INVALID_ARGS;
}

bool parseCfg( unsigned numPins, Cpl::Text::Tokenizer::TextBlock& tokens, unsigned parmIndex, Driver::DIO::InOut::Config_T cfgArray[], Cpl::Text::String& errMsg )
{
    for ( unsigned idx = 0; idx < numPins; idx++, parmIndex +=2 )
    {
        // Configure parameters come in pairs
        if ( tokens.numParameters() < parmIndex + 1 )
        {
            errMsg += "Missing PinCfg parameter";
            return false;
        }

        // Get PIN ID (support hex notation)
        unsigned long pin;
        if ( !Cpl::Text::a2ul( pin, tokens.getParameter( parmIndex ), 0 ) )
        {
            errMsg.formatAppend( "Unable to parse Pin ID (%s)", tokens.getParameter( parmIndex ) );
            return false;
        }

        // Get Blob (support hex notation)
        unsigned long blob;
        if ( !Cpl::Text::a2ul( blob, tokens.getParameter( parmIndex + 1 ), 0 ) )
        {
            errMsg.formatAppend( "Unable to parse Blob (%s)", tokens.getParameter( parmIndex + 1 ) );
            return false;
        }

        cfgArray[idx].pin  = pin;
        cfgArray[idx].blob = blob;
    }

    return true;
}

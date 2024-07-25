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
/** @file

    This file implements the DIO::InOut driver with a simulated driver. 
    
    The IO signals are simulated using Model Points.

    When an Input PIN is configure with a Pull-Up resistor - then Model Point
    value is inverted when reading the input.

*/

#include "Driver/DIO/InOut.h"
#include "InOutModelPoints.h"
#include "InOut.h"

using namespace Driver::DIO;


static uint8_t                 numInputs_;
static uint8_t                 numOutputs_;
static const InOut::Config_T*  inputCfg_;
static bool                    started_;

static bool afterPulls( uint8_t inputIndex, bool rawSignal )
{
    if ( (inputCfg_[inputIndex].blob & Driver::DIO::Simulated::INOUT_CFG_PULL_UPDOWN_MASK) == Driver::DIO::Simulated::INOUT_CFG_PULL_UP )
    {
        return !rawSignal;
    }
    return rawSignal;
}


///////////////////////////////////////////////////////////
bool Driver::DIO::InOut::start( uint8_t          numInputs,
                                const Config_T   inputCfg[],
                                uint8_t          numOutputs,
                                const Config_T   outputCfg[] )
{
    // Ignore if already started
    if ( !started_ )
    {
        // Enforce limits
        if ( numInputs > DRIVER_DIO_SIMULATED_INOUT_MAX_INPUTS || numOutputs > DRIVER_DIO_SIMULATED_INOUT_MAX_OUTPUTS )
        {
            return false;
        }
        
        // Cache the config (but only 'bits' we care about)
        inputCfg_   = inputCfg;
        numInputs_  = numInputs;
        numOutputs_ = numOutputs;

        // Initialize the Inputs (to - in theory - the default state)
        for ( uint8_t idx=0; idx < numInputs_; idx++ )
        {
            mp::simDioInOut_inputs[idx]->write( afterPulls( idx, false) );
        }

        // Initialize the Outputs state (per the interface semantics)
        for ( uint8_t idx=0; idx < numOutputs_; idx++ )
        {
            mp::simDioInOut_outputs[idx]->write( false );
        }

        started_ = true;
        return true;
    }

    return false;
}

void Driver::DIO::InOut::stop()
{
    started_ = false;
}

bool Driver::DIO::InOut::getOutput( uint8_t outputIndex, bool& assertedOut )
{
    if ( started_ && outputIndex < numOutputs_ )
    {
        return mp::simDioInOut_outputs[outputIndex]->read( assertedOut );
    }

    return false;
}

bool Driver::DIO::InOut::setOutput( uint8_t outputIndex, bool asserted )
{
    if ( started_ && outputIndex < numOutputs_ )
    {
        mp::simDioInOut_outputs[outputIndex]->write( asserted );
        return true;
    }

    return false;
}

bool Driver::DIO::InOut::getInput( uint8_t inputIndex, bool& assertedOut )
{
    if ( started_ && inputIndex < numInputs_ )
    {
        return mp::simDioInOut_inputs[inputIndex]->read( assertedOut );
    }

    return false;
}



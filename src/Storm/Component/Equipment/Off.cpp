/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Off.h"

using namespace Storm::Component::Equipment;

//////////////////////////////////////////////////////////////
Off::Off()
{
    // Nothing needed/required
}

//////////////////////////////////////////////////////////////
bool Off::executeActive( Args_T& args ) noexcept
{
    // Turn OFF all of the equipment (including the both fans - but NOT the SOV)
    Storm::Dm::MpVirtualOutputs::setSafeAllOff( args.vOutputs );

    // Trap the start of the next 'off cycle'
    if ( args.systemOn )
    {
        args.cycleInfo.beginOffTime = args.currentInterval;
    }

    // Set my state of OFF
    args.cycleInfo.mode = Storm::Type::CycleStatus::eOFF;
    args.systemOn       = false;
    return true;
}

bool Off::executeOff( Args_T& args ) noexcept
{
    // Nothing needed/required
    return true;
}

bool Off::start( Cpl::System::ElapsedTime::Precision_T& intervalTime ) noexcept
{
    // Nothing needed/required
    return true;
}

void Off::reset() noexcept
{
    // Nothing needed/required
}
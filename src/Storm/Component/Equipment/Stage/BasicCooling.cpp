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


#include "BasicCooling.h"
#include "Cpl/System/Assert.h"


/// Namespaces
using namespace Storm::Component::Equipment::Stage;



///////////////////////////////
BasicCooling::BasicCooling( unsigned systemStageIndex, unsigned outputStageIndex )
    : Basic( systemStageIndex, outputStageIndex )
{
    // Initialize my FSM
    initialize();
}

///////////////////////////////
void BasicCooling::initializeActive() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );

    Basic::initializeActive();
    m_args->vOutputs.sovInHeating = false;
}

void BasicCooling::stageOff() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );
    m_args->vOutputs.outdoorStages[m_outIndex] = STORM_DM_MP_VIRTUAL_OUTPUTS_OFF;
    
    // Only turn the indoor fan off when the 1st stage is turned off
    if ( m_systemIndex == 0 )
    {
        m_args->vOutputs.indoorFan = STORM_DM_MP_VIRTUAL_OUTPUTS_OFF;
    }
}

void BasicCooling::stageOn() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );
    m_args->vOutputs.outdoorStages[m_outIndex] = STORM_DM_MP_VIRTUAL_OUTPUTS_ON;
    
    // Only turn the indoor fan off on the 1st stage is turned on
    if ( m_systemIndex == 0 )
    {
        m_args->vOutputs.indoorFan = STORM_DM_MP_VIRTUAL_OUTPUTS_ON;
    }
}


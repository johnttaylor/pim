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

#include "Cooling.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"


#define SECT_ "Storm::Component::Equipment"

using namespace Storm::Component::Equipment;

//////////////////////////////////////////////////////////////
Cooling::Cooling( StageApi& firstStageCooling )
    : m_1StageCooling( firstStageCooling )
{
}

//////////////////////////////////////////////////////////////
bool Cooling::executeActive( Args_T& args ) noexcept
{
    switch ( args.systemConfig.numCompressorStages )
    {
    case 1:
        // Single Stage logic
        singleStage( args );

        // 'Run' the stage(s)
        m_1StageCooling.execute( args );
        break;


    default:
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Cooling::executeActive: Unsupported num-of-compressor-stages (%d)", args.systemConfig.numCompressorStages ) );
        return false;
    }

    return true;
}

bool Cooling::executeOff( Args_T& args ) noexcept
{
    m_1StageCooling.requestModeToOff();
    return true;
}

bool Cooling::start( Cpl::System::ElapsedTime::Precision_T& intervalTime ) noexcept
{
    reset();
    return true;
}

void Cooling::reset() noexcept
{
    // initialize Stage object(s)
    m_1StageCooling.reconfigure( 0, 0 );
    m_1StageCooling.requestModeToOff();
}

//////////////////////////////////////////////////////////////
bool Cooling::singleStage( Args_T& args ) noexcept
{
    // If the system was previously off AND there is sufficient load to turn on
    if ( args.systemOn == false && args.pvOut > OPTION_STORM_COMPONENT_EQUIPMENT_COOLING_TURN_ON_THRESHOLD )
    {
        // Has the system met the minimum compressor off time?
        Cpl::System::ElapsedTime::Precision_T minOffTime = { OPTION_STORM_MIN_COMPRESSOR_OFF_TIME_SEC, 0 };
        if ( Cpl::System::ElapsedTime::expiredPrecision( args.equipmentBeginTimes.outdoorUnitBeginOffTime, minOffTime, args.currentInterval ) || args.whiteBox.defeatEquipMinOffTime == true )
        {
            m_1StageCooling.requestOn( args );
        }

        return true;
    }

    // stage 1 is the active stage AND there is no longer sufficient load requiring conditioning 
    else if ( m_1StageCooling.isActive() && args.pvOut <= OPTION_STORM_COMPONENT_EQUIPMENT_COOLING_TURN_OFF_THRESHOLD )
    {
        // Am I in an off-cycle OR has the system met the minimum 1st stage on cycle time?
        Cpl::System::ElapsedTime::Precision_T minOnTime = { args.systemConfig.stages[0].minOnTime, 0 };
        if ( m_1StageCooling.isOffCycle() || Cpl::System::ElapsedTime::expiredPrecision( args.cycleInfo.beginOnTime, minOnTime, args.currentInterval ) )
        {
            m_1StageCooling.requestOff( args );
        }

        return true;
    }

    // Return false because no 'logic was tripped'
    return false;
}


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

#include "Catch/catch.hpp"
#include "Storm/Component/Control.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "statics.h"


using namespace Storm::Component;

#define SECT_       "_0test"

class MyTestEquipment : public Control::Equipment
{
public:
    bool executeActive( Args_T& args ) noexcept
    {
        m_executeCount++;
        args.vOutputs.indoorFan               = m_executeCount * 2;
        args.cycleInfo.beginOffTime.m_seconds = m_startCount * 10;
        args.cycleInfo.beginOnTime.m_seconds  = m_operatingMode;
        args.systemOn                         = m_executeCount & 0x1;    // Set to true when count is odd
        return true;
    }

    bool executeOff( Args_T& args ) noexcept
    {
        m_executeOffCount += m_operatingMode;
        return true;
    }

    bool start( Cpl::System::ElapsedTime::Precision_T intervalTime ) noexcept
    {
        m_startCount++;
        return true;
    }

    void reset() noexcept
    {
    }

    Storm::Type::OperatingMode getOperatingMode() const noexcept
    {
        return Storm::Type::OperatingMode::_from_integral_unchecked( m_operatingMode );
    }

public:
    MyTestEquipment( Storm::Type::OperatingMode operatingMode ) : m_operatingMode( operatingMode ), m_executeCount( 0 ), m_executeOffCount( 0 ), m_startCount( 0 ) {}

public:
    int m_operatingMode;
    int m_executeCount;
    int m_executeOffCount;
    int m_startCount;
};


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Control" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Control::Input_T  ins  = { &mp_systemConfig, &mp_pvOut, &mp_vOutputs, &mp_equipmentBeginTimes, &mp_systemOn, &mp_cycleInfo, &mp_operatingModeChanged, &mp_whiteBox };
    Control::Output_T outs = { &mp_vOutputs, &mp_cycleInfo, &mp_systemOn };
    mp_systemOn.write( false );
    mp_operatingModeChanged.write( false );

    MyTestEquipment coolingEquipment( Storm::Type::OperatingMode::eCOOLING );
    MyTestEquipment heatingEquipment( Storm::Type::OperatingMode::eHEATING );
    Control componentCooling( coolingEquipment, ins, outs );
    Control componentHeating( heatingEquipment, ins, outs );
    Storm::Type::SystemConfig_T sysCfg;
    Storm::Dm::MpSystemConfig::setToOff( sysCfg );

    // Start the components
    Cpl::System::ElapsedTime::Precision_T time = { 0, 1 };
    componentCooling.start( time );
    componentHeating.start( time );
    REQUIRE( coolingEquipment.m_startCount == 1 );
    REQUIRE( heatingEquipment.m_startCount == 1 );


    // Execute the Components in COOLING
    sysCfg.currentOpMode = Storm::Type::OperatingMode::eCOOLING;
    mp_systemConfig.write( sysCfg );
    mp_pvOut.write( 0 );
    time.m_thousandths += 2;
    componentCooling.doWork( true, time );
    componentHeating.doWork( true, time );
    REQUIRE( coolingEquipment.m_startCount == 1 );
    REQUIRE( heatingEquipment.m_startCount == 1 );
    REQUIRE( coolingEquipment.m_executeCount == 1 );
    REQUIRE( heatingEquipment.m_executeCount == 0 );
    REQUIRE( coolingEquipment.m_executeOffCount == 0 );
    REQUIRE( heatingEquipment.m_executeOffCount == Storm::Type::OperatingMode::eHEATING );
    Storm::Type::VirtualOutputs_T         outputValues;
    Storm::Type::CycleInfo_T              cycleValues;
    bool     systemOnValue;
    mp_vOutputs.read( outputValues );
    mp_cycleInfo.read( cycleValues );
    mp_systemOn.read( systemOnValue );
    REQUIRE( outputValues.indoorFan == 1 * 2 );
    REQUIRE( cycleValues.beginOffTime.m_seconds == 1 * 10 );
    REQUIRE( cycleValues.beginOnTime.m_seconds == Storm::Type::OperatingMode::eCOOLING );
    REQUIRE( systemOnValue == true );


    // Execute the Components in HEATING
    sysCfg.currentOpMode = Storm::Type::OperatingMode::eHEATING;
    mp_systemConfig.write( sysCfg );
    time.m_thousandths += 1;
    componentCooling.doWork( true, time );
    componentHeating.doWork( true, time );
    REQUIRE( coolingEquipment.m_startCount == 1 );
    REQUIRE( heatingEquipment.m_startCount == 1 );
    REQUIRE( coolingEquipment.m_executeCount == 1 );
    REQUIRE( heatingEquipment.m_executeCount == 1 );
    REQUIRE( coolingEquipment.m_executeOffCount == Storm::Type::OperatingMode::eCOOLING );
    REQUIRE( heatingEquipment.m_executeOffCount == Storm::Type::OperatingMode::eHEATING );
    mp_vOutputs.read( outputValues );
    mp_cycleInfo.read( cycleValues );
    mp_systemOn.read( systemOnValue );
    REQUIRE( outputValues.indoorFan == 1 * 2 );
    REQUIRE( cycleValues.beginOffTime.m_seconds == 1 * 10 );
    REQUIRE( cycleValues.beginOnTime.m_seconds == Storm::Type::OperatingMode::eHEATING );
    REQUIRE( systemOnValue == true );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


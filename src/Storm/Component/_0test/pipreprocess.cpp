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
#include "Storm/Component/PiPreProcess.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "statics.h"


using namespace Storm::Component;

#define SECT_       "_0test"


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "PI PreProcess" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    PiPreProcess::Input_T  ins  = { &mp_activeIdt, &mp_systemConfig, &mp_operatingModeChanged, &mp_setpoints };
    PiPreProcess::Output_T outs = { &mp_activeSetpoint, &mp_deltaIdtError, &mp_deltaSetpoint, &mp_setpointChanged };

    PiPreProcess component( ins, outs );
    Storm::Type::SystemConfig_T sysCfg;
    Storm::Dm::MpSystemConfig::setToOff( sysCfg );

    // Start the component (and 'prime' it for the first real interval)
    Cpl::System::ElapsedTime::Precision_T time = { 0, 1 };
    component.start( time );
    component.doWork( true, time );
    time.m_thousandths += 1;

    
    SECTION( "setpoint" )
    {
        // Base model point setup
        mp_operatingModeChanged.write( true );
        Storm::Dm::MpEquipmentConfig::Data cfg = { Storm::Type::IduType::eAIR_HANDLER, Storm::Type::OduType::eAC, 1, 1, true };
        mp_equipmentConfig.write( cfg );
        
        // Cooling mode
        mp_activeIdt.write( 78.0F );
        mp_setpoints.write( 80.0F, 60.0F );
        sysCfg.currentOpMode = Storm::Type::OperatingMode::eCOOLING;
        mp_systemConfig.write( sysCfg );
        component.doWork( true, time );
        float  value;
        int8_t valid = mp_activeSetpoint.read( value );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 80.0F ) == true );
        valid = mp_deltaIdtError.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( value, -2.0F ) == true );
        valid = mp_deltaSetpoint.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 0.0F ) == true );
        bool changed;
        valid = mp_setpointChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );


        // Heating mode
        mp_activeIdt.write( 59.5F );
        mp_setpoints.write( 80.0F, 60.0F );
        sysCfg.currentOpMode = Storm::Type::OperatingMode::eHEATING;
        mp_systemConfig.write( sysCfg );
        mp_operatingModeChanged.write( true );
        time.m_thousandths += 1;
        component.doWork( true, time );
        valid = mp_activeSetpoint.read( value );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 60.0F ) == true );
        valid = mp_deltaIdtError.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 0.5F ) == true );
        valid = mp_deltaSetpoint.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 0.0F ) == true );
        valid = mp_setpointChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );

        // Change in heating setpoint
        mp_activeIdt.write( 59.5F );
        mp_setpoints.write( 80.0F, 61.0 );
        mp_operatingModeChanged.write( false );
        time.m_thousandths += 1;
        component.doWork( true, time );
        valid = mp_activeSetpoint.read( value );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 61.0F ) == true );
        valid = mp_deltaIdtError.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 1.5F ) == true );
        valid = mp_deltaSetpoint.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 1.0F ) == true );
        valid = mp_setpointChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == true );

        // No Change in heating setpoint
        time.m_thousandths += 1;
        component.doWork( true, time );
        valid = mp_deltaSetpoint.read( value );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 0.0F ) == true );
        valid = mp_setpointChanged.read( changed );
        REQUIRE( Cpl::Dm::ModelPoint::IS_VALID( valid ) == true );
        REQUIRE( changed == false );
    }


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


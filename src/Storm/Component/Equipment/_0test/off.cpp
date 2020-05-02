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
#include "Storm/Component/Equipment/Off.h"
#include "Storm/Dm/MpSystemConfig.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"


using namespace Storm::Component::Equipment;

#define SECT_       "_0test"


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Off" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    Off uut;
    Storm::Component::Control::Equipment::Args_T args = { 0, };
    Storm::Dm::MpSystemConfig::setToOff( args.systemConfig );

    args.vOutputs.indoorFan               = 1;
    args.vOutputs.outdoorFan              = 2;
    args.vOutputs.indoorStages[0]         = 3;
    args.vOutputs.outdoorStages[0]        = 4;
    args.vOutputs.sovInHeating            = true;
    args.currentInterval                  = { 1, 0 };
    args.currentTick                      = { 1, 1 };
    args.cycleInfo.beginOffTime           = { 3, 3 };
    args.systemOn                         = true;
    args.systemConfig.numCompressorStages = 1;
    args.systemConfig.numIndoorStages     = 1;
    args.systemConfig.indoorUnitType      = Storm::Type::IduType::eFURNACE;
    args.systemConfig.outdoorUnitType     = Storm::Type::OduType::eAC;

    // Exercise the Equipment
    uut.start( { 0, 100 } );
    uut.executeOff( args );
    REQUIRE( args.vOutputs.indoorFan == 1 );
    REQUIRE( args.vOutputs.outdoorFan == 2 );
    REQUIRE( args.vOutputs.indoorStages[0] == 3 );
    REQUIRE( args.vOutputs.outdoorStages[0] == 4 );
    REQUIRE( args.vOutputs.sovInHeating == true );
    Cpl::System::ElapsedTime::Precision_T expected = { 3,3 };
    REQUIRE( args.systemOn == true );
    REQUIRE( args.cycleInfo.beginOffTime == expected );

    uut.reset();
    uut.executeActive( args );
    REQUIRE( args.vOutputs.indoorFan == 0 );
    REQUIRE( args.vOutputs.outdoorFan == 0 );
    REQUIRE( args.vOutputs.indoorStages[0] == 0 );
    REQUIRE( args.vOutputs.outdoorStages[0] == 0 );
    REQUIRE( args.vOutputs.sovInHeating == true );
    REQUIRE( args.systemOn == false );
    expected = { 1,0 };
    REQUIRE( args.cycleInfo.beginOffTime == expected );

    args.currentInterval = { 4, 0 };
    args.currentTick = { 4, 1 };
    uut.executeActive( args );
    REQUIRE( args.vOutputs.indoorFan == 0 );
    REQUIRE( args.vOutputs.outdoorFan == 0 );
    REQUIRE( args.vOutputs.indoorStages[0] == 0 );
    REQUIRE( args.vOutputs.outdoorStages[0] == 0 );
    REQUIRE( args.vOutputs.sovInHeating == true );
    REQUIRE( args.systemOn == false );
    REQUIRE( args.cycleInfo.beginOffTime == expected );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


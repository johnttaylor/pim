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
#include "Storm/Component/IdtSelection.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "statics.h"


using namespace Storm::Component;

#define SECT_       "_0test"

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "IDT Selection" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    IdtSelection::Input_T  ins  = { &mp_primaryRawIdt , &mp_secondaryRawIdt, &mp_enabledSecondaryIdt };
    IdtSelection::Output_T outs = { &mp_activeIdt, &mp_systemForcedOffRefCnt , &mp_idtAlarms };
    IdtSelection component( ins, outs );

    // Start the component (and 'prime' it for the first real interval)
    Cpl::System::ElapsedTime::Precision_T time = { 0, 1 };
    component.start( time );
    component.doWork( true, time );
    time.m_thousandths += 1;

    SECTION( "temperature" )
    {
        // Use secondary
        mp_secondaryRawIdt.write( 20.0F );
        mp_primaryRawIdt.write( 30.0F );
        mp_enabledSecondaryIdt.write( true );
        component.doWork( true, time );
        float  value;
        bool valid = mp_activeIdt.read( value );
        REQUIRE( valid == true );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 20.0F ) == true );

        // Default to primary when the secondary is bad
        mp_enabledSecondaryIdt.setInvalid();
        time.m_thousandths += 1;
        component.doWork( true, time );
        valid = mp_activeIdt.read( value );
        REQUIRE( valid == true );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 30.0F ) == true );

        // Use secondary when primary is bad
        mp_enabledSecondaryIdt.write( true );
        mp_secondaryRawIdt.write( 21.0F );
        time.m_thousandths += 1;
        component.doWork( true, time );
        valid = mp_activeIdt.read( value );
        REQUIRE( valid == true );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 21.0F ) == true );

        // Use primary (no secondary available)
        mp_enabledSecondaryIdt.write( false );
        mp_primaryRawIdt.write( 31.0F );
        time.m_thousandths += 1;
        component.doWork( true, time );
        valid = mp_activeIdt.read( value );
        REQUIRE( valid == true );
        REQUIRE( Cpl::Math::areFloatsEqual( value, 31.0F ) == true );
    }

    SECTION( "alarms" )
    {
        CPL_SYSTEM_TRACE_SCOPE( SECT_, "ALARMS test" );

        // Primary only --> failure
        mp_enabledSecondaryIdt.setInvalid();
        mp_primaryRawIdt.setInvalid();
        component.doWork( true, time );
        Storm::Dm::MpIdtAlarm::Data value;
        bool valid = mp_idtAlarms.read( value );
        REQUIRE( valid == true );
        REQUIRE( value.critical == true );
        REQUIRE( value.primaryAlarm == true );
        REQUIRE( value.secondaryAlarm == false );
        REQUIRE( value.primaryAck == false );
        REQUIRE( value.secondaryAck == false );
        uint32_t refCounter;
        valid = mp_systemForcedOffRefCnt.read( refCounter );
        REQUIRE( valid == true );
        REQUIRE( refCounter == 1 );

        // Recovery
        mp_primaryRawIdt.write( 25.0F );
        time.m_thousandths += 1;
        component.doWork( true, time );
        valid = mp_idtAlarms.read( value );
        REQUIRE( valid == true );
        REQUIRE( value.critical == false );
        REQUIRE( value.primaryAlarm == false );
        REQUIRE( value.secondaryAlarm == false );
        valid = mp_systemForcedOffRefCnt.read( refCounter );
        REQUIRE( valid == true );
        REQUIRE( refCounter == 0 );

        // Secondary Failure, but good primary
        mp_enabledSecondaryIdt.write( true );
        mp_secondaryRawIdt.setInvalid();
        time.m_thousandths += 1;
        component.doWork( true, time );
        valid = mp_idtAlarms.read( value );
        REQUIRE( valid == true );
        REQUIRE( value.critical == false );
        REQUIRE( value.primaryAlarm == false );
        REQUIRE( value.secondaryAlarm == true );
        valid = mp_systemForcedOffRefCnt.read( refCounter );
        REQUIRE( valid == true );
        REQUIRE( refCounter == 0 );

        // Secondary and Primary failure
        mp_primaryRawIdt.setInvalid();
        time.m_thousandths += 1;
        component.doWork( true, time );
        valid = mp_idtAlarms.read( value );
        REQUIRE( valid == true );
        REQUIRE( value.critical == true );
        REQUIRE( value.primaryAlarm == true );
        REQUIRE( value.secondaryAlarm == true );
        valid = mp_systemForcedOffRefCnt.read( refCounter );
        REQUIRE( valid == true );
        REQUIRE( refCounter == 1 );

        // Primary Failure, Secondary good
        mp_secondaryRawIdt.write( 24.0F );
        time.m_thousandths += 1;
        component.doWork( true, time );
        valid = mp_idtAlarms.read( value );
        REQUIRE( valid == true );
        REQUIRE( value.critical == false );
        REQUIRE( value.primaryAlarm == true );
        REQUIRE( value.secondaryAlarm == false );
        valid = mp_systemForcedOffRefCnt.read( refCounter );
        REQUIRE( valid == true );
        REQUIRE( refCounter == 0 );
    }


    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


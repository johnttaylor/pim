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
#include "Storm/Component/Pi.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "statics.h"


using namespace Storm::Component;

#define SECT_       "_0test"


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "PI" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();
    Pi::Input_T  ins  = { &mp_resetPiPulse, &mp_deltaIdtError, &mp_systemConfig, &mp_freezePiRefCnt, &mp_inhibitfRefCnt };
    Pi::Output_T outs = { &mp_pvOut, &mp_sumError, &mp_pvInhibited };


    Pi component( ins, outs );
    mp_freezePiRefCnt.reset();
    mp_inhibitfRefCnt.reset();
    Storm::Type::SystemConfig_T sysCfg;
    Storm::Dm::MpSystemConfig::setToOff( sysCfg );

    // Start the component (and 'prime' it for the first real interval)
    Cpl::System::ElapsedTime::Precision_T time = { 0, 1 };
    component.start( time );
    component.doWork( true, time );
    time.m_thousandths += 1;

    SECTION( "nominal path" )
    {
#define GAIN        10.0F
#define RESET_TIME  100.0F      // ResetTime = 100ms
        
        sysCfg.gain     = GAIN;
        sysCfg.reset    = RESET_TIME;
        sysCfg.maxPvOut = 200.F;
        mp_systemConfig.write( sysCfg );
        mp_resetPiPulse.write( true );
        mp_deltaIdtError.write( 1.0F );
        time.m_thousandths += 1;
        component.doWork( true, time );

        float pvOut, sumError;
        bool  pvInhibited;
        bool validPvOut = mp_pvOut.read( pvOut );
        bool validSumErr = mp_sumError.read( sumError );
        bool validPvInhibit = mp_pvInhibited.read( pvInhibited );
        REQUIRE( validPvOut == true );
        REQUIRE( validSumErr == true );
        REQUIRE( validPvInhibit == true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 10.1F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 1.0F ) == true );
        REQUIRE( pvInhibited == false );

        mp_resetPiPulse.write( false );
        mp_deltaIdtError.write( 1.0F );
        for ( int i=1; i < 100; i++ )
        {
            time.m_thousandths += 1;
            component.doWork( true, time );
        }

        mp_pvOut.read( pvOut );
        mp_sumError.read( sumError );
        mp_pvInhibited.read( pvInhibited );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 20.0F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 100.0F ) == true );
        REQUIRE( pvInhibited == false );
    }

    SECTION( "freeze..." )
    {
        mp_resetPiPulse.write( true );
        mp_deltaIdtError.write( 1.0F );
        time.m_thousandths += 1;
        component.doWork( true, time );

        float pvOut, sumError;
        bool  pvInhibited;
        bool validPvOut = mp_pvOut.read( pvOut );
        bool validSumErr = mp_sumError.read( sumError );
        bool validPvInhibit = mp_pvInhibited.read( pvInhibited );
        REQUIRE( validPvOut == true );
        REQUIRE( validSumErr == true );
        REQUIRE( validPvInhibit == true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 10.1F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 1.0F ) == true );
        REQUIRE( pvInhibited == false );

        mp_resetPiPulse.write( false );
        mp_deltaIdtError.write( 1.0F );
        for ( int i=1; i < 50; i++ )
        {
            time.m_thousandths += 1;
            component.doWork( true, time );
        }
        mp_pvOut.read( pvOut );
        mp_sumError.read( sumError );
        mp_pvInhibited.read( pvInhibited );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 15.0F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 50.0F ) == true );
        REQUIRE( pvInhibited == false );

        mp_freezePiRefCnt.increment();
        mp_deltaIdtError.write( 2.0F );
        for ( int i=50; i < 100; i++ )
        {
            time.m_thousandths += 1;
            component.doWork( true, time );
        }
        mp_pvOut.read( pvOut );
        mp_sumError.read( sumError );
        mp_pvInhibited.read( pvInhibited );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 15.0F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 50.0F ) == true );
        REQUIRE( pvInhibited == true );

        mp_deltaIdtError.write( 1.0F );
        mp_freezePiRefCnt.decrement();
        for ( int i=50; i < 100; i++ )
        {
            time.m_thousandths += 1;
            component.doWork( true, time );
        }
        mp_pvOut.read( pvOut );
        mp_sumError.read( sumError );
        mp_pvInhibited.read( pvInhibited );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 20.0F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 100.0F ) == true );
        REQUIRE( pvInhibited == false );
    }

    SECTION( "inhibit..." )
    {
        mp_resetPiPulse.write( true );
        mp_deltaIdtError.write( 1.0F );
        time.m_thousandths += 1;
        component.doWork( true, time );

        float pvOut, sumError;
        bool  pvInhibited;
        bool validPvOut = mp_pvOut.read( pvOut );
        bool validSumErr = mp_sumError.read( sumError );
        bool validPvInhibit = mp_pvInhibited.read( pvInhibited );
        REQUIRE( validPvOut == true );
        REQUIRE( validSumErr == true );
        REQUIRE( validPvInhibit == true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 10.1F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 1.0F ) == true );
        REQUIRE( pvInhibited == false );

        mp_resetPiPulse.write( false );
        mp_deltaIdtError.write( 1.0F );
        for ( int i=1; i < 50; i++ )
        {
            time.m_thousandths += 1;
            component.doWork( true, time );
        }
        mp_pvOut.read( pvOut );
        mp_sumError.read( sumError );
        mp_pvInhibited.read( pvInhibited );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 15.0F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 50.0F ) == true );
        REQUIRE( pvInhibited == false );

        mp_inhibitfRefCnt.increment();
        mp_deltaIdtError.write( 2.0F );
        for ( int i=50; i < 100; i++ )
        {
            time.m_thousandths += 1;
            component.doWork( true, time );
        }
        mp_pvOut.read( pvOut );
        mp_sumError.read( sumError );
        mp_pvInhibited.read( pvInhibited );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 25.2F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 50.0F ) == true );
        REQUIRE( pvInhibited == true );

        mp_deltaIdtError.write( 1.0F );
        mp_inhibitfRefCnt.decrement();
        for ( int i=50; i < 100; i++ )
        {
            time.m_thousandths += 1;
            component.doWork( true, time );
        }
        mp_pvOut.read( pvOut );
        mp_sumError.read( sumError );
        mp_pvInhibited.read( pvInhibited );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 20.0F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 100.0F ) == true );
        REQUIRE( pvInhibited == false );
    }

    SECTION( "max pv out..." )
    {
        mp_resetPiPulse.write( true );
        mp_deltaIdtError.write( 100.0F );
        time.m_thousandths += 1;
        component.doWork( true, time );

        float pvOut, sumError;
        bool  pvInhibited;
        bool validPvOut = mp_pvOut.read( pvOut );
        bool validSumErr = mp_sumError.read( sumError );
        bool validPvInhibit = mp_pvInhibited.read( pvInhibited );
        REQUIRE( validPvOut == true );
        REQUIRE( validSumErr == true );
        REQUIRE( validPvInhibit == true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 200.0F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 0.0F ) == true );
        REQUIRE( pvInhibited == true );
    }

    SECTION( "negative out..." )
    {
        mp_resetPiPulse.write( true );
        mp_deltaIdtError.write( -1.0F );
        time.m_thousandths += 1;
        component.doWork( true, time );

        float pvOut, sumError;
        bool  pvInhibited;
        bool validPvOut = mp_pvOut.read( pvOut );
        bool validSumErr = mp_sumError.read( sumError );
        bool validPvInhibit = mp_pvInhibited.read( pvInhibited );
        REQUIRE( validPvOut == true );
        REQUIRE( validSumErr == true );
        REQUIRE( validPvInhibit == true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "pvOut=%g, sumError=%g", pvOut, sumError ) );
        REQUIRE( Cpl::Math::areFloatsEqual( pvOut, 0.0F ) == true );
        REQUIRE( Cpl::Math::areFloatsEqual( sumError, 0.0F ) == true );
        REQUIRE( pvInhibited == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}


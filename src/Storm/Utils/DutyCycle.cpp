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


#include "DutyCycle.h"
#include "Cpl/Math/real.h"
#include "Cpl/System/FatalError.h"

using namespace Storm::Utils;

static float k_[Storm::Type::Cph::eNUM_OPTIONS] =
{
    /* 2CPH @ 50% = 15min  */ 15.0F * 60 * 0.5F,
    /* 3CPH @ 50% = 10min  */ 10.0F * 60 * 0.5F,
    /* 4CPH @ 50% = 7.5min */ 7.5F * 60 * 0.5F,
    /* 5CPH @ 50% = 6min   */ 6.0F * 60 * 0.5F,
    /* 6CPH @ 50% = 5min   */ 5.0F * 60 * 0.5F
};

static uint32_t maxMinimumTimeLimits_[Storm::Type::Cph::eNUM_OPTIONS] =
{
    /* 2CPH - 15min  */  15 * 60 ,
    /* 3CPH - 10min  */  10 * 60 ,
    /* 4CPH - 7.5min */ ( uint32_t) ( 7.5 * 60 ),
    /* 5CPH - 6min   */ ( 6 * 60 ),
    /* 6CPH - 5min   */ ( 5 * 60 )
};


///////////////////////////////

static float normalizePv( float pvVar, float pvLowerBound, float pvUpperBound )
{
    // Normalize the Process Variable (to a percentage) for the specified range
    float range = pvUpperBound - pvLowerBound;
    if ( pvVar < pvLowerBound )
    {
        pvVar = 0.0F;
    }
    else
    {
        pvVar = ( pvVar - pvLowerBound ) / range;
        if ( pvVar > 1.0F )
        {
            pvVar = 1.0F;
        }
    }

    return pvVar;
}

uint32_t DutyCycle::calculateOffTime( float pvVar, uint32_t minOffTime, Storm::Type::Cph cyclesPerHour, float pvLowerBound, float pvUpperBound )
{
    // Trap invalid CPH option
    if ( cyclesPerHour == +Storm::Type::Cph::eNUM_OPTIONS )
    {
        Cpl::System::FatalError::logf( "DutyCycle::calculateOffTime: Invalid CPH selection (pvVar=%g, minOffTime=%d, lower=%g, upper=%g)", pvVar, minOffTime, pvLowerBound, pvUpperBound );
    }

    // Normalize the Process Variable (to a percentage) for the specified range
    pvVar = normalizePv( pvVar, pvLowerBound, pvUpperBound );

    // Calc off time
    uint32_t cycleTime = 0xFFFFFFFF;    // Maximum possible OFF time if at no load
    if ( Cpl::Math::areFloatsEqual( pvVar, 0.0F ) == false )
    {
        cycleTime = (uint32_t) ( k_[cyclesPerHour] / pvVar );
    }

    // Enforce min off time
    uint32_t timeLimit = getMaximumMinOffTime( cyclesPerHour );
    if ( minOffTime > timeLimit )
    {
        minOffTime = timeLimit;
    }
    if ( cycleTime < minOffTime )
    {
        cycleTime = minOffTime;
    }

    return cycleTime;
}

uint32_t DutyCycle::calculateOnTime( float pvVar, uint32_t minOnTime, Storm::Type::Cph cyclesPerHour, float pvLowerBound, float pvUpperBound )
{
    // Trap invalid CPH option
    if ( cyclesPerHour == +Storm::Type::Cph::eNUM_OPTIONS )
    {
        Cpl::System::FatalError::logf( "DutyCycle::calculateOnTime: Invalid CPH selection (pvVar=%g, minOffTime=%d, lower=%g, upper=%g)", pvVar, minOnTime, pvLowerBound, pvUpperBound );
    }

    // Normalize the Process Variable (to a percentage) for the specified range
    pvVar = normalizePv( pvVar, pvLowerBound, pvUpperBound );

    // Calc on time
    uint32_t cycleTime = 0xFFFFFFFF;    // Maximum possible ON time if at max load
    float divisor      = 1.0F - pvVar;
    if ( Cpl::Math::areFloatsEqual( divisor, 0.0F ) == false )
    {
        cycleTime = (uint32_t) ( k_[cyclesPerHour] / ( 1.0F - pvVar ) );
    }

    // Enforce min on time
    uint32_t timeLimit = getMaximumMinOnTime( cyclesPerHour );
    if ( minOnTime > timeLimit )
    {
        minOnTime = timeLimit;
    }
    if ( cycleTime < minOnTime )
    {
        cycleTime = minOnTime;
    }

    return cycleTime;
}


uint32_t DutyCycle::getMaximumMinOnTime( Storm::Type::Cph cyclesPerHour )
{
    // Trap invalid CPH option
    if ( cyclesPerHour == +Storm::Type::Cph::eNUM_OPTIONS )
    {
        Cpl::System::FatalError::logf( "getMaximumMinOnTime::calculateOffTime: Invalid CPH selection" );
    }

    // Return the max allowed value
    return maxMinimumTimeLimits_[cyclesPerHour];
}

uint32_t DutyCycle::getMaximumMinOffTime( Storm::Type::Cph cyclesPerHour )
{
    // Trap invalid CPH option
    if ( cyclesPerHour == +Storm::Type::Cph::eNUM_OPTIONS )
    {
        Cpl::System::FatalError::logf( "getMaximumMinOffTime::calculateOffTime: Invalid CPH selection" );
    }

    // Return the max allowed value
    return maxMinimumTimeLimits_[cyclesPerHour];
}

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

#include "Cpl/System/ElapsedTime.h"
#include "FreeRTOS.h"
#include "task.h"


/// 
using namespace Cpl::System;

extern "C" void vApplicationTickHook( void );


///////////////////////////////////////////////////////////////
static uint16_t      secondsDivider_;
static unsigned long elaspedSeconds_;

///////////////////////////////////////////////////////////////
void vApplicationTickHook( void )
{
    if ( ++secondsDivider_ >= 1000 / portTICK_PERIOD_MS )
    {
        secondsDivider_ = 0;
        elaspedSeconds_++;
    }
}


///////////////////////////////////////////////////////////////
unsigned long ElapsedTime::milliseconds( void ) noexcept
{
    return millisecondsInRealTime();
}


unsigned long ElapsedTime::seconds( void ) noexcept
{
    return secondsInRealTime();
}


ElapsedTime::Precision_T ElapsedTime::precision( void ) noexcept
{
    return precisionInRealTime();
}


///////////////////////////////////////////////////////////////
unsigned long ElapsedTime::millisecondsInRealTime( void ) noexcept
{
    unsigned long s;
    uint16_t      ms;

    // Note: I use my own counter because it was the only way to guaranty that milliseconds and seconds are in sync
    taskENTER_CRITICAL();
    s  = elaspedSeconds_;
    ms = secondsDivider_;
    taskEXIT_CRITICAL();

    return (ms * portTICK_PERIOD_MS) + s * 1000L;
}

unsigned long ElapsedTime::secondsInRealTime( void ) noexcept
{
    unsigned long s;

    taskENTER_CRITICAL();
    s  = elaspedSeconds_;
    taskEXIT_CRITICAL();

    return s;
}


ElapsedTime::Precision_T ElapsedTime::precisionInRealTime( void ) noexcept
{
    ElapsedTime::Precision_T now;

    taskENTER_CRITICAL();
    now.m_seconds     = elaspedSeconds_;
    now.m_thousandths = secondsDivider_;
    taskEXIT_CRITICAL();

    now.m_thousandths *= portTICK_PERIOD_MS;
    return now;
}

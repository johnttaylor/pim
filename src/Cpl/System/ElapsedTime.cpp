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


#include "ElapsedTime.h"

///
using namespace Cpl::System;


#define MAX_POSITVE_MSEC_   0x7FFF

///////////////////////////////////////////////////////
ElapsedTime::Precision_T & ElapsedTime::Precision_T::operator +=( const ElapsedTime::Precision_T & x )
{
    this->m_thousandths += x.m_thousandths;
    this->m_seconds     += x.m_seconds + this->m_thousandths / 1000;
    this->m_thousandths %= 1000;
    return *this;
}

ElapsedTime::Precision_T & ElapsedTime::Precision_T::setFromMilliseconds( uint32_t milliseconds )
{
    this->m_seconds     = milliseconds / 1000;
    this->m_thousandths = milliseconds % 1000;
    return *this;
}

ElapsedTime::Precision_T & ElapsedTime::Precision_T::operator =( uint32_t milliseconds )
{
    return setFromMilliseconds( milliseconds );
}

bool ElapsedTime::Precision_T::operator > ( Precision_T other ) const
{
    if ( m_seconds > other.m_seconds ||
        ( m_seconds == other.m_seconds && m_thousandths > other.m_thousandths ) )
    {
        return true;
    }

    return false;
}

bool ElapsedTime::Precision_T::operator >= ( Precision_T other ) const
{
    if ( *this == other || *this > other )
    {
        return true;
    }

    return false;
}
bool ElapsedTime::Precision_T::operator < ( Precision_T other ) const
{
    if ( m_seconds < other.m_seconds ||
        ( m_seconds == other.m_seconds && m_thousandths < other.m_thousandths ) )
    {
        return true;
    }

    return false;
}
bool ElapsedTime::Precision_T::operator <= ( Precision_T other ) const
{
    if ( *this == other || *this < other )
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////
ElapsedTime::Precision_T ElapsedTime::deltaPrecision( Precision_T startTime, Precision_T endTime ) noexcept
{
    Precision_T delta;

    // Calc delta seconds
    delta.m_seconds = endTime.m_seconds - startTime.m_seconds;

    // Calc delta milliseconds (trap case of 'borrow')
    delta.m_thousandths = endTime.m_thousandths - startTime.m_thousandths;
    if ( delta.m_thousandths > MAX_POSITVE_MSEC_ )
    {
        delta.m_seconds--;
        delta.m_thousandths += 1000;
    }

    return delta;
}


bool ElapsedTime::expiredPrecision( Precision_T timeMarker, Precision_T duration, Precision_T currentTime ) noexcept
{
    Precision_T delta = ElapsedTime::deltaPrecision( timeMarker, currentTime );
    return (delta.m_seconds > duration.m_seconds) || ( delta.m_seconds == duration.m_seconds && delta.m_thousandths >= duration.m_thousandths);
}

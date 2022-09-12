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
/** @file */

#include "ComfortConfig.h"
#include "Cph.h"
#include "Storm/Utils/DutyCycle.h"

///
using namespace Storm::Type;

bool Storm::Type::ComfortStageParameters_T::validate( ComfortStageParameters_T& src ) noexcept
{
    bool modified = false;

    if ( src.cph >= Storm::Type::Cph::eNUM_OPTIONS || src.cph < Storm::Type::Cph::e2CPH )
    {
        modified = true;
        src.cph  = Storm::Type::Cph::e3CPH;
    }

    uint32_t limit = Storm::Utils::DutyCycle::getMaximumMinOffTime( Storm::Type::Cph::_from_integral_unchecked( src.cph ) );
    if ( src.minOffTime > limit )
    {
        src.minOffTime = limit;
        modified       = true;
    }

    limit = Storm::Utils::DutyCycle::getMaximumMinOnTime( Storm::Type::Cph::_from_integral_unchecked( src.cph ) );
    if ( src.minOnTime > limit )
    {
        src.minOnTime = limit;
        modified      = true;
    }

    return modified;
}

bool Storm::Type::ComfortConfig_T::validate( Storm::Type::ComfortConfig_T& src ) noexcept
{
    bool modified = false;

    modified |= ComfortStageParameters_T::validate( src.compressorCooling );
    modified |= ComfortStageParameters_T::validate( src.compressorHeating );
    modified |= ComfortStageParameters_T::validate( src.indoorHeating );

    return modified;
}
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


#include "MpSetpoints.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Math/real.h"
#include <string.h>

///
using namespace Storm::Dm;


///////////////////////////////////////////////////////////////////////////////
MpSetpoints::MpSetpoints( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, float coolSetpt, float heatSetpt )
    : ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
    , m_data( { coolSetpt, heatSetpt } )
{
    validateSetpoints( m_data.coolSetpt, m_data.heatSetpt, m_data.coolSetpt, m_data.heatSetpt );
}

void MpSetpoints::validateSetpoints( float newCooling, float newHeating, float& finalCooling, float& finalHeating )
{
    // Enforce cooling range
    if ( newCooling > OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING )
    {
        newCooling = OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING;
    }
    else if ( newCooling < OPTION_STORM_DM_MP_SETPOINTS_MIN_COOLING )
    {
        newCooling = OPTION_STORM_DM_MP_SETPOINTS_MIN_COOLING;
    }

    // Enforce heating range
    if ( newHeating > OPTION_STORM_DM_MP_SETPOINTS_MAX_HEATING )
    {
        newHeating = OPTION_STORM_DM_MP_SETPOINTS_MAX_HEATING;
    }
    else if ( newHeating < OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING )
    {
        newHeating = OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING;
    }

    // Enforce Cooling is ALWAYS higher than the heating setpoint
    if ( newCooling <= newHeating )
    {
        newHeating = newCooling - 1.0F;
    }

    // Update results
    finalCooling = newCooling;
    finalHeating = newHeating;
}

///////////////////////////////////////////////////////////////////////////////
void MpSetpoints::hookSetInvalid() noexcept
{
    // Reset the cooling/heating set-point to their 'safe' values when being
    // invalidated. This ensure proper behavior when just write ONE set-point.
    m_data.coolSetpt = OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING;
    m_data.heatSetpt = OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING;
}

void MpSetpoints::attach( Observer & observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpSetpoints::detach( Observer & observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

const char* MpSetpoints::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpSetpoints";
}

///////////////////////////////////////////////////////////////////////////////
void MpSetpoints::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj = doc.createNestedObject( "val" );
    valObj["cool"] = m_data.coolSetpt;
    valObj["heat"] = m_data.heatSetpt;
}

bool MpSetpoints::fromJSON_( JsonVariant & src, LockRequest_T lockRequest, uint16_t & retSequenceNumber, Cpl::Text::String * errorMsg ) noexcept
{
    if ( src.is<JsonObject>() )
    {
        float cool = m_data.coolSetpt;
        float heat = m_data.heatSetpt;

        // Parse Fields
        if ( src["cool"].is<float>() )
        {
            cool = src["cool"];
        }
        if ( src["heat"].is<float>() )
        {
            heat = src["heat"];
        }

        retSequenceNumber = write( cool, heat, lockRequest );
        return true;
    }

    if ( errorMsg )
    {
        errorMsg->format( "Invalid key/value for 'val'" );
    }
    return false;
}


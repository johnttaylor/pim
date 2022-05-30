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


#include "MpComfortConfig.h"
#include "Storm/Utils/DutyCycle.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "Storm/Constants.h"
#include <string.h>

#define SECT_   "Storm::Dm"

///
using namespace Storm::Dm;

static void setDefaults( Storm::Type::ComfortStageParameters_T& parms )
{
    parms.cph        = OPTION_STORM_DEFAULT_CPH;
    parms.minOffTime = OPTION_STORM_DEFAULT_MIN_OFF_CYCLE_TIME;
    parms.minOnTime  = OPTION_STORM_DEFAULT_MIN_ON_CYCLE_TIME;
}

///////////////////////////////////////////////////////////////////////////////

MpComfortConfig::MpComfortConfig( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    : ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof(m_data), true )
{
    hookSetInvalid();
}

///////////////////////////////////////////////////////////////////////////////
void MpComfortConfig::hookSetInvalid() noexcept
{
    // Reset the default values when being invalidated. This ensure proper 
    // behavior when just updating a single field.
    setDefaults( m_data.compressorCooling );
    setDefaults( m_data.compressorHeating );
    setDefaults( m_data.indoorHeating );
}

uint16_t MpComfortConfig::writeCompressorCooling( const Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Storm::Type::ComfortConfig_T          src    = m_data;
    Storm::Type::ComfortStageParameters_T newVal = newParameters;
    newVal.validate( newVal );
    src.compressorCooling = newVal;
    uint16_t result       = write( src, lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

uint16_t MpComfortConfig::writeCompressorHeating( const Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Storm::Type::ComfortConfig_T          src    = m_data;
    Storm::Type::ComfortStageParameters_T newVal = newParameters;
    newVal.validate( newVal );
    src.compressorHeating = newVal;
    uint16_t result       = write( src, lockRequest );
    
    m_modelDatabase.unlock_();
    return result;
}

uint16_t MpComfortConfig::writeIndoorHeating( const Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Storm::Type::ComfortConfig_T          src    = m_data;
    Storm::Type::ComfortStageParameters_T newVal = newParameters;
    newVal.validate( newVal );
    src.indoorHeating = newVal;
    uint16_t result   = write( src, lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

void MpComfortConfig::attach( Observer & observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpComfortConfig::detach( Observer & observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* MpComfortConfig::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpComfortConfig";
}

static void buildEntry( JsonObject& obj, Storm::Type::ComfortStageParameters_T& parms )
{
    Storm::Type::Cph cph = Storm::Type::Cph::_from_integral_unchecked( parms.cph );
    obj["cph"]           = cph._to_string();
    obj["minOn"]         = parms.minOnTime;
    obj["minOff"]        = parms.minOffTime;
}

void MpComfortConfig::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj    = doc.createNestedObject( "val" );
    
    JsonObject coolObj   = valObj.createNestedObject( "cmpCool" );
    buildEntry( coolObj, m_data.compressorCooling );
    
    JsonObject heatObj   = valObj.createNestedObject( "cmpHeat" );
    buildEntry( heatObj, m_data.compressorHeating );
    
    JsonObject indoorObj = valObj.createNestedObject( "indoorHeat" );
    buildEntry( indoorObj, m_data.indoorHeating );
}


static bool parseEntry( JsonObject& obj, Storm::Type::ComfortStageParameters_T& parms, Cpl::Text::String * errorMsg, const char* field )
{
    const char* enumVal = obj["cph"];
    if ( enumVal != 0 )
    {
        auto maybeValue = Storm::Type::Cph::_from_string_nothrow( enumVal );
        if ( !maybeValue )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Invalid %s CPH enum value (%s)", field, enumVal );
            }
            return false;
        }

        parms.cph = *maybeValue;
    }
    parms.minOnTime  = obj["minOn"] | parms.minOnTime;
    parms.minOffTime = obj["minOff"] | parms.minOffTime;
    return true;
}

bool MpComfortConfig::fromJSON_( JsonVariant & src, LockRequest_T lockRequest, uint16_t & retSequenceNumber, Cpl::Text::String * errorMsg ) noexcept
{
    Storm::Type::ComfortConfig_T newVal = m_data;

    // COOLING operation
    JsonObject cmpCooling = src["cmpCool"];
    if ( !parseEntry( cmpCooling, newVal.compressorCooling, errorMsg, "CmpCooling" ) )
    {
        return false;
    }

    // Compressor HEATING operation
    JsonObject cmpHeating = src["cmpHeat"];
    if ( !parseEntry( cmpHeating, newVal.compressorHeating, errorMsg, "CmpHeating" ) )
    {
        return false;
    }

    // Indoor HEATING operation
    JsonObject iduHeating = src["indoorHeat"];
    if ( !parseEntry( iduHeating, newVal.indoorHeating, errorMsg, "IduHeating" ) )
    {
        return false;
    }

    retSequenceNumber = write( newVal, lockRequest );
    return true;
}



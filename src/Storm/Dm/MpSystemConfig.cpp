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


#include "MpSystemConfig.h"
#include "Storm/Utils/DutyCycle.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include <string.h>

#define SECT_   "Storm::Dm"

///
using namespace Storm::Dm;



///////////////////////////////////////////////////////////////////////////////

MpSystemConfig::MpSystemConfig( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    : ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
{
    // Set all potential 'pad bytes' to zero so memcmp() will work correctly
    hookSetInvalid();

    // Default the configuration
    setToOff( m_data );
}

void MpSystemConfig::setToOff( Storm::Type::SystemConfig_T& cfgToReset ) noexcept
{
    // Set all potential 'pad bytes' to zero
    memset( &cfgToReset, 0, sizeof(Storm::Type::SystemConfig_T) );

    cfgToReset.numCompressorStages = 0;
    cfgToReset.numIndoorStages     = 0;
    cfgToReset.totalStages         = 0;
    cfgToReset.gain                = 1.0F;
    cfgToReset.reset               = 1.0F;
    cfgToReset.indoorUnitType      = Storm::Type::IduType::eFURNACE;
    cfgToReset.outdoorUnitType     = Storm::Type::OduType::eAC;
    cfgToReset.currentOpMode       = Storm::Type::OperatingMode::eOFF;
    for ( uint16_t i=0; i < OPTION_STORM_MAX_TOTAL_STAGES; i++ )
    {
        cfgToReset.stages[i].cph          = OPTION_STORM_DEFAULT_CPH;
        cfgToReset.stages[i].minOffTime   = OPTION_STORM_DEFAULT_MIN_OFF_CYCLE_TIME;
        cfgToReset.stages[i].minOnTime    = OPTION_STORM_DEFAULT_MIN_ON_CYCLE_TIME;
        cfgToReset.stages[i].lowerBound   = 0.0F;
        cfgToReset.stages[i].upperBound   = 1.0F;
        cfgToReset.maxPvOut               = cfgToReset.stages[i].upperBound;
        cfgToReset.stages[i].minIndoorFan = 0;
        cfgToReset.stages[i].maxIndoorFan = cfgToReset.stages[i].minIndoorFan;
    }
}

///////////////////////////////////////////////////////////////////////////////
void MpSystemConfig::attach( Observer & observer, uint16_t initialSeqNumber ) noexcept
{
    attachSubscriber( observer, initialSeqNumber );
}

void MpSystemConfig::detach( Observer & observer ) noexcept
{
    detachSubscriber( observer );
}

const char* MpSystemConfig::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpSystemConfig";
}

bool MpSystemConfig::isDataEqual_( const void* otherData ) const noexcept
{
    Storm::Type::SystemConfig_T* otherPtr = (Storm::Type::SystemConfig_T*) otherData;

    // Compare bounds array (must be done brute force since the array contains floats)
    for ( int i=0; i < m_data.totalStages; i++ )
    {
        if ( !Cpl::Math::areFloatsEqual( m_data.stages[i].lowerBound, otherPtr->stages[i].lowerBound ) )
        {
            return false;
        }
        if ( !Cpl::Math::areFloatsEqual( m_data.stages[i].upperBound, otherPtr->stages[i].upperBound ) )
        {
            return false;
        }
        if ( m_data.stages[i].maxIndoorFan != otherPtr->stages[i].maxIndoorFan )
        {
            return false;
        }
        if ( m_data.stages[i].minIndoorFan != otherPtr->stages[i].minIndoorFan )
        {
            return false;
        }
        if ( m_data.stages[i].cph != otherPtr->stages[i].cph )
        {
            return false;
        }
        if ( m_data.stages[i].minOffTime != otherPtr->stages[i].minOffTime )
        {
            return false;
        }
        if ( m_data.stages[i].minOnTime != otherPtr->stages[i].minOnTime )
        {
            return false;
        }
    }

    // Compare non-stage fields
    return m_data.currentOpMode == otherPtr->currentOpMode &&
        m_data.indoorUnitType == otherPtr->indoorUnitType &&
        m_data.outdoorUnitType == otherPtr->outdoorUnitType &&
        m_data.numCompressorStages == otherPtr->numCompressorStages &&
        m_data.numIndoorStages == otherPtr->numIndoorStages &&
        m_data.fanContinuousSpeed == otherPtr->fanContinuousSpeed &&
        Cpl::Math::areFloatsEqual( m_data.gain, otherPtr->gain ) &&
        Cpl::Math::areFloatsEqual( m_data.reset, otherPtr->reset ) &&
        Cpl::Math::areFloatsEqual( m_data.maxPvOut, otherPtr->maxPvOut );
}



///////////////////////////////////////////////////////////////////////////////
void MpSystemConfig::setJSONVal( JsonDocument& doc ) noexcept
{
    // Parameters
    JsonObject valObj       = doc.createNestedObject( "val" );
    valObj["opMode"]        = Storm::Type::OperatingMode::_from_integral_unchecked( m_data.currentOpMode )._to_string();
    valObj["oduType"]       = Storm::Type::OduType::_from_integral_unchecked( m_data.outdoorUnitType )._to_string();
    valObj["iduType"]       = Storm::Type::IduType::_from_integral_unchecked( m_data.indoorUnitType )._to_string();
    valObj["numCompStages"] = m_data.numCompressorStages;
    valObj["numIdStages"]   = m_data.numIndoorStages;
    valObj["totalStages"]   = m_data.totalStages;
    valObj["fanCont"]       = m_data.fanContinuousSpeed;
    valObj["gain"]          = (double) m_data.gain;
    valObj["reset"]         = (double) m_data.reset;
    valObj["maxPv"]         = (double) m_data.maxPvOut;

    // Bounds array
    JsonArray  bounds  = valObj.createNestedArray( "stages" );
    for ( int i=0; i < m_data.totalStages; i++ )
    {
        JsonObject elemObj   = bounds.createNestedObject();
        elemObj["stage"]     = i + 1;
        elemObj["lower"]     = (double) m_data.stages[i].lowerBound;
        elemObj["upper"]     = (double) m_data.stages[i].upperBound;
        elemObj["minBlower"] = m_data.stages[i].minIndoorFan;
        elemObj["maxBlower"] = m_data.stages[i].maxIndoorFan;
        elemObj["cph"]       = Storm::Type::Cph::_from_integral_unchecked( m_data.stages[i].cph )._to_string();
        elemObj["minOn"]     = m_data.stages[i].minOnTime;
        elemObj["minOff"]    = m_data.stages[i].minOffTime;
    }
}

bool MpSystemConfig::fromJSON_( JsonVariant & src, LockRequest_T lockRequest, uint16_t & retSequenceNumber, Cpl::Text::String * errorMsg ) noexcept
{
    Storm::Type::SystemConfig_T newVal = m_data;

    if ( src.is<JsonObject>() )
    {
        // Numeric values
        if ( src["numCompStages"].is<unsigned>() )
        {
            newVal.numCompressorStages = src["numCompStages"].as<unsigned>();
        }
        if ( src["numIdStages"].is<unsigned>() )
        {
            newVal.numIndoorStages = src["numIdStages"].as<unsigned>();
        }
        if ( src["totalStages"].is<unsigned>() )
        {
            newVal.totalStages = src["totalStages"].as<unsigned>();
        }
        if ( src["fanCont"].is<unsigned>() )
        {
            newVal.fanContinuousSpeed = src["fanCont"].as<unsigned>();
        }
        if ( src["gain"].is<float>() )
        {
            newVal.gain = src["gain"].as<float>();
        }
        if ( src["reset"].is<float>() )
        {
            newVal.reset = src["reset"].as<float>();
        }
        if ( src["maxPv"].is<float>() )
        {
            newVal.maxPvOut = src["maxPv"].as<float>();
        }

        // Enum values
        const char* enumVal = src["opMode"];
        if ( enumVal )
        {
            // Convert the text to an enum value
            auto maybeValue = Storm::Type::OperatingMode::_from_string_nothrow( enumVal );
            if ( maybeValue )
            {
                newVal.currentOpMode = *maybeValue;
            }
        }
        enumVal = src["iduType"];
        if ( enumVal )
        {
            // Convert the text to an enum value
            auto maybeValue = Storm::Type::IduType::_from_string_nothrow( enumVal );
            if ( maybeValue )
            {
                newVal.indoorUnitType = *maybeValue;
            }
        }
        enumVal = src["oduType"];
        if ( enumVal )
        {
            // Convert the text to an enum value
            auto maybeValue = Storm::Type::OduType::_from_string_nothrow( enumVal );
            if ( maybeValue )
            {
                newVal.outdoorUnitType = *maybeValue;
            }
        }


        // PV Bounds
        if ( src["stages"].is<JsonArray>() )
        {
            JsonArray stageArray = src["stages"];
            for ( unsigned i=0; i < stageArray.size(); i++ )
            {
                if ( stageArray[i]["stage"].is<unsigned>() )
                {
                    unsigned stageNum = stageArray[i]["stage"];
                    if ( stageNum > 0 && stageNum <= newVal.totalStages )
                    {
                        newVal.stages[stageNum - 1].lowerBound   = stageArray[i]["lower"] | newVal.stages[stageNum - 1].lowerBound;
                        newVal.stages[stageNum - 1].upperBound   = stageArray[i]["upper"] | newVal.stages[stageNum - 1].upperBound;
                        newVal.stages[stageNum - 1].minIndoorFan = stageArray[i]["minBlower"] | newVal.stages[stageNum - 1].minIndoorFan;
                        newVal.stages[stageNum - 1].maxIndoorFan = stageArray[i]["maxBlower"] | newVal.stages[stageNum - 1].maxIndoorFan;
                        newVal.stages[stageNum - 1].minOnTime    = stageArray[i]["minOn"] | newVal.stages[stageNum - 1].minOnTime;
                        newVal.stages[stageNum - 1].minOffTime   = stageArray[i]["minOff"] | newVal.stages[stageNum - 1].minOffTime;

                        // Enum values
                        const char* enumVal = stageArray[i]["cph"];
                        if ( enumVal )
                        {
                            // Convert the text to an enum value
                            auto maybeValue = Storm::Type::Cph::_from_string_nothrow( enumVal );
                            if ( maybeValue )
                            {
                                newVal.stages[stageNum - 1].cph = *maybeValue;
                            }
                        }
                    }
                }
            }
        }

        retSequenceNumber = write( newVal, lockRequest );
        return true;
    }

    if ( errorMsg )
    {
        errorMsg->format( "Invalid key/value pair" );
    }
    return false;
}



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


#include "MpVirtualOutputs.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define MAX_OUTPUT      1000

#define SECT_   "Storm::Dm"



///
using namespace Storm::Dm;

///////////////////////////////////////////////////////////////////////////////
MpVirtualOutputs::MpVirtualOutputs( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    : ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
{
    hookSetInvalid();   // Default everything to OFF (and the SOV in cooling)
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpVirtualOutputs::setIndoorFanOutput( uint16_t fanSpeed, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::VirtualOutputs_T newData;
    m_modelDatabase.lock_();

    newData           = m_data;
    newData.indoorFan = fanSpeed;

    uint16_t result = write( newData, lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpVirtualOutputs::setIndoorFanContinousOutput( uint16_t fanContSpeed, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::VirtualOutputs_T newData;
    m_modelDatabase.lock_();

    newData               = m_data;
    newData.indoorFanCont = fanContSpeed;

    uint16_t result = write( newData, lockRequest );
    m_modelDatabase.unlock_();

    return result;
}


uint16_t MpVirtualOutputs::setIndoorStageOutput( uint8_t stageIndex, uint16_t stageOutput, LockRequest_T lockRequest ) noexcept
{
    // Trap out-of-range stage index value
    if ( stageIndex >= STORM_MAX_INDOOR_STAGES )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("MpVirtualOutputs::MpVirtualOutputs() Invalid INDOOR stage index=%d (num stages=%d)", stageIndex, STORM_MAX_OUTDOOR_STAGES) );
        return getSequenceNumber();
    }

    Storm::Type::VirtualOutputs_T newData;
    m_modelDatabase.lock_();

    newData                          = m_data;
    newData.indoorStages[stageIndex] = stageOutput;

    uint16_t result = write( newData );
    m_modelDatabase.unlock_();

    return result;
}


uint16_t MpVirtualOutputs::setOutdoorFanOutput( uint16_t fanSpeed, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::VirtualOutputs_T newData;
    m_modelDatabase.lock_();

    newData            = m_data;
    newData.outdoorFan = fanSpeed;

    uint16_t result = write( newData, lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpVirtualOutputs::setOutdoorStageOutput( uint8_t stageIndex, uint16_t stageOutput, LockRequest_T lockRequest ) noexcept
{
    // Trap out-of-range stage index value
    if ( stageIndex >= STORM_MAX_OUTDOOR_STAGES )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("MpVirtualOutputs::MpVirtualOutputs() Invalid OUTDOOR stage index=%d (num stages=%d)", stageIndex, STORM_MAX_OUTDOOR_STAGES) );
        return getSequenceNumber();
    }

    Storm::Type::VirtualOutputs_T newData;
    m_modelDatabase.lock_();

    newData                           = m_data;
    newData.outdoorStages[stageIndex] = stageOutput;

    uint16_t result = write( newData );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpVirtualOutputs::setSovToCooling( LockRequest_T lockRequest ) noexcept
{
    Storm::Type::VirtualOutputs_T newData;
    m_modelDatabase.lock_();

    newData              = m_data;
    newData.sovInHeating = false;

    uint16_t result = write( newData, lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpVirtualOutputs::setSovToHeating( LockRequest_T lockRequest ) noexcept
{
    Storm::Type::VirtualOutputs_T newData;
    m_modelDatabase.lock_();

    newData              = m_data;
    newData.sovInHeating = true;

    uint16_t result = write( newData, lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpVirtualOutputs::setOutdoorOff( LockRequest_T lockRequest ) noexcept
{
    Storm::Type::VirtualOutputs_T newData;
    m_modelDatabase.lock_();

    newData              = m_data;
    for ( int i=0; i < STORM_MAX_OUTDOOR_STAGES; i++ )
    {
        newData.outdoorStages[i] = STORM_DM_MP_VIRTUAL_OUTPUTS_OFF;
    }
    newData.outdoorFan = STORM_DM_MP_VIRTUAL_OUTPUTS_OFF;

    uint16_t result = write( newData, lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpVirtualOutputs::setIndoorOff( LockRequest_T lockRequest ) noexcept
{
    Storm::Type::VirtualOutputs_T newData;
    m_modelDatabase.lock_();

    newData              = m_data;
    for ( int i=0; i < STORM_MAX_INDOOR_STAGES; i++ )
    {
        newData.indoorStages[i] = STORM_DM_MP_VIRTUAL_OUTPUTS_OFF;
    }
    newData.indoorFan     = STORM_DM_MP_VIRTUAL_OUTPUTS_OFF;
    newData.indoorFanCont = STORM_DM_MP_VIRTUAL_OUTPUTS_OFF;

    uint16_t result = write( newData, lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpVirtualOutputs::setSafeAllOff( LockRequest_T lockRequest ) noexcept
{
    Storm::Type::VirtualOutputs_T newData;
    m_modelDatabase.lock_();

    setSafeAllOff( newData );
    newData.sovInHeating = m_data.sovInHeating;

    uint16_t result = write( newData, lockRequest );
    m_modelDatabase.unlock_();

    return result;
}


void MpVirtualOutputs::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpVirtualOutputs::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* MpVirtualOutputs::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpVirtualOutputs";
}


void MpVirtualOutputs::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj   = doc.createNestedObject( "val" );
    valObj["idFan"]     = m_data.indoorFan;
    valObj["idFanCont"] = m_data.indoorFanCont;
    valObj["odFan"]     = m_data.outdoorFan;
    valObj["sovHeat"]   = m_data.sovInHeating;

    JsonArray  idStages = valObj.createNestedArray( "idStages" );
    for ( int i=0; i < STORM_MAX_INDOOR_STAGES; i++ )
    {
        JsonObject elemObj  = idStages.createNestedObject();
        elemObj["stage"]    = i + 1;
        elemObj["capacity"] = m_data.indoorStages[i];
    }

    JsonArray  odStages = valObj.createNestedArray( "odStages" );
    for ( int i=0; i < STORM_MAX_OUTDOOR_STAGES; i++ )
    {
        JsonObject elemObj  = odStages.createNestedObject();
        elemObj["stage"]    = i + 1;
        elemObj["capacity"] = m_data.outdoorStages[i];
    }
}


bool MpVirtualOutputs::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Storm::Type::VirtualOutputs_T updatedData = m_data;

    if ( src.is<JsonObject>() )
    {
        // Parameters
        updatedData.indoorFan     = src["idFan"] | updatedData.indoorFan;
        updatedData.indoorFanCont = src["idFanCont"] | updatedData.indoorFanCont;
        updatedData.outdoorFan    = src["odFan"] | updatedData.outdoorFan;
        updatedData.sovInHeating  = src["sovHeat"] | updatedData.sovInHeating;

        // Indoor Output stages
        if ( src["idStages"].is<JsonArray>() )
        {
            JsonArray idStages = src["idStages"];
            for ( unsigned i=0; i < idStages.size(); i++ )
            {
                if ( idStages[i]["stage"].is<unsigned>() )
                {
                    unsigned stageNum = idStages[i]["stage"];
                    if ( stageNum > 0 && stageNum <= STORM_MAX_INDOOR_STAGES )
                    {
                        updatedData.indoorStages[stageNum - 1] = idStages[i]["capacity"] | updatedData.indoorStages[stageNum - 1];
                    }
                }
            }
        }

        // Outdoor Output stages
        if ( src["odStages"].is<JsonArray>() )
        {
            JsonArray odStages = src["odStages"];
            for ( unsigned i=0; i < odStages.size(); i++ )
            {
                if ( odStages[i]["stage"].is<unsigned>() )
                {
                    int stageNum = odStages[i]["stage"];
                    if ( stageNum > 0 && stageNum <= STORM_MAX_OUTDOOR_STAGES )
                    {
                        updatedData.outdoorStages[stageNum - 1] = odStages[i]["capacity"] | updatedData.outdoorStages[stageNum - 1];
                    }
                }
            }
        }

        retSequenceNumber = write( updatedData, lockRequest );
        return true;
    }

    if ( errorMsg )
    {
        errorMsg->format( "Invalid key/value pair" );
    }
    return false;
}

void MpVirtualOutputs::validate( Storm::Type::VirtualOutputs_T & newValues ) const noexcept
{
    if ( newValues.indoorFan > MAX_OUTPUT )
    {
        newValues.indoorFan = MAX_OUTPUT;
    }
    if ( newValues.indoorFanCont > MAX_OUTPUT )
    {
        newValues.indoorFanCont = MAX_OUTPUT;
    }
    if ( newValues.outdoorFan > MAX_OUTPUT )
    {
        newValues.outdoorFan = MAX_OUTPUT;
    }

    for ( int i=0; i < STORM_MAX_INDOOR_STAGES; i++ )
    {
        if ( newValues.indoorStages[i] > MAX_OUTPUT )
        {
            newValues.indoorStages[i] = MAX_OUTPUT;
        }
    }
    for ( int i=0; i < STORM_MAX_OUTDOOR_STAGES; i++ )
    {
        if ( newValues.outdoorStages[i] > MAX_OUTPUT )
        {
            newValues.outdoorStages[i] = MAX_OUTPUT;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
bool MpVirtualOutputs::areStagesOn( const Storm::Type::VirtualOutputs_T & outputs )
{
    for ( int i=0; i < STORM_MAX_INDOOR_STAGES; i++ )
    {
        if ( outputs.indoorStages[i] > 0 )
        {
            return true;
        }
    }
    for ( int i=0; i < STORM_MAX_OUTDOOR_STAGES; i++ )
    {
        if ( outputs.outdoorStages[i] > 0 )
        {
            return true;
        }
    }

    // If I get here -->no equipment/stages where on
    return false;
}

void MpVirtualOutputs::setSafeAllOff( Storm::Type::VirtualOutputs_T & outputs )
{
    // Set everything to ZERO, but preserve the SOV setting
    bool sov = outputs.sovInHeating;
    memset( &outputs, 0, sizeof( Storm::Type::VirtualOutputs_T ) );
    outputs.sovInHeating = sov;
}


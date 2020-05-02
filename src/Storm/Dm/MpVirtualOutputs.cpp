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
MpVirtualOutputs::MpVirtualOutputs( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, MODEL_POINT_STATE_VALID )
{
    memset( &m_data, 0, sizeof( m_data ) );
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpVirtualOutputs::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
    // Set all outputs to off when invalidating the Model Point
    m_modelDatabase.lock_();
    memset( &m_data, 0, sizeof( m_data ) );
    uint16_t result = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );
    m_modelDatabase.unlock_();
    return result;
}

int8_t MpVirtualOutputs::read( Storm::Type::VirtualOutputs_T& dstData, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &dstData, sizeof( Storm::Type::VirtualOutputs_T ), seqNumPtr );
}

uint16_t MpVirtualOutputs::write( const Storm::Type::VirtualOutputs_T& srcData, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::VirtualOutputs_T newData = srcData;
    validate( newData );
    return ModelPointCommon_::write( &newData, sizeof( Storm::Type::VirtualOutputs_T ), lockRequest );
}

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
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "MpVirtualOutputs::MpVirtualOutputs() Invalid INDOOR stage index=%d (num stages=%d)", stageIndex, STORM_MAX_OUTDOOR_STAGES ) );
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
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "MpVirtualOutputs::MpVirtualOutputs() Invalid OUTDOOR stage index=%d (num stages=%d)", stageIndex, STORM_MAX_OUTDOOR_STAGES ) );
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


uint16_t MpVirtualOutputs::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void MpVirtualOutputs::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpVirtualOutputs::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

bool MpVirtualOutputs::isDataEqual_( const void* otherData ) const noexcept
{
    return  memcmp( &m_data, otherData, sizeof( m_data ) ) == 0;
}

void MpVirtualOutputs::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Storm::Type::VirtualOutputs_T ) );
    Storm::Type::VirtualOutputs_T* dstDataPtr = ( Storm::Type::VirtualOutputs_T* ) dstData;
    *dstDataPtr = m_data;
}

void MpVirtualOutputs::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Storm::Type::VirtualOutputs_T ) );
    Storm::Type::VirtualOutputs_T* dataSrcPtr = ( Storm::Type::VirtualOutputs_T* ) srcData;
    m_data = *dataSrcPtr;
}


///////////////////////////////////////////////////////////////////////////////
const char* MpVirtualOutputs::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpVirtualOutputs";
}

size_t MpVirtualOutputs::getSize() const noexcept
{
    return sizeof( Storm::Type::VirtualOutputs_T );
}

size_t MpVirtualOutputs::getInternalDataSize_() const noexcept
{
    return sizeof( Storm::Type::VirtualOutputs_T );
}


const void* MpVirtualOutputs::getImportExportDataPointer_() const noexcept
{
    return &m_data;
}

bool MpVirtualOutputs::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
{
    // Get my state
    m_modelDatabase.lock_();
    uint16_t seqnum = m_seqNum;
    int8_t   valid  = m_validState;
    bool     locked = m_locked;

    // Start the conversion
    JsonDocument& doc = beginJSON( valid, locked, seqnum, verbose );

    // Construct the 'val' key/value pair 
    if ( IS_VALID( valid ) )
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

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose );

    m_modelDatabase.unlock_();
    return true;
}


bool MpVirtualOutputs::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Storm::Type::VirtualOutputs_T updatedData = m_data;

    // Parameters
    updatedData.indoorFan     = src["idFan"] | updatedData.indoorFan;
    updatedData.indoorFanCont = src["idFanCont"] | updatedData.indoorFanCont;
    updatedData.outdoorFan    = src["odFan"] | updatedData.outdoorFan;
    updatedData.sovInHeating  = src["sovHeat"] | updatedData.sovInHeating;

    // Indoor Output stages
    JsonArray idStages = src["idStages"];
    for ( unsigned i=0; i < idStages.size(); i++ )
    {
        int stageNum = idStages[i]["stage"];
        if ( stageNum < 1 || stageNum > STORM_MAX_INDOOR_STAGES )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Invalid indoor stage number (%d)", stageNum );
            }
            return false;
        }

        updatedData.indoorStages[stageNum - 1] = idStages[i]["capacity"] | updatedData.indoorStages[stageNum - 1];
    }

    // Outdoor Output stages
    JsonArray odStages = src["odStages"];
    for ( unsigned i=0; i < odStages.size(); i++ )
    {
        int stageNum = odStages[i]["stage"];
        if ( stageNum < 1 || stageNum > STORM_MAX_OUTDOOR_STAGES )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Invalid outdoor stage number (%d)", stageNum );
            }
            return false;
        }

        updatedData.outdoorStages[stageNum - 1] = odStages[i]["capacity"] | updatedData.outdoorStages[stageNum - 1];
    }

    retSequenceNumber = write( updatedData, lockRequest );
    return true;
}

void MpVirtualOutputs::validate( Storm::Type::VirtualOutputs_T& newValues ) const noexcept
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
bool MpVirtualOutputs::areStagesOn( const Storm::Type::VirtualOutputs_T& outputs )
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

void MpVirtualOutputs::setSafeAllOff( Storm::Type::VirtualOutputs_T& outputs )
{
    memset( outputs.indoorStages, 0, sizeof( outputs.indoorStages ) );
    memset( outputs.outdoorStages, 0, sizeof( outputs.outdoorStages ) );
    outputs.indoorFan      = STORM_DM_MP_VIRTUAL_OUTPUTS_OFF;
    outputs.indoorFanCont  = STORM_DM_MP_VIRTUAL_OUTPUTS_OFF;
    outputs.outdoorFan     = STORM_DM_MP_VIRTUAL_OUTPUTS_OFF;
}


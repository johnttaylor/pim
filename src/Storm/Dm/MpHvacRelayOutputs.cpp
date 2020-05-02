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


#include "MpHvacRelayOutputs.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define INVALID_VALUE   9999
#define MAX_OUTPUT      100

#define SECT_   "Storm::Dm"


///
using namespace Storm::Dm;

///////////////////////////////////////////////////////////////////////////////
MpHvacRelayOutputs::MpHvacRelayOutputs( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, MODEL_POINT_STATE_VALID )
{
    memset( &m_data, 0, sizeof( m_data ) );
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpHvacRelayOutputs::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
    // Set all outputs to off when invalidating the Model Point
    m_modelDatabase.lock_();
    memset( &m_data, 0, sizeof( m_data ) );
    uint16_t result = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );
    m_modelDatabase.unlock_();
    return result;
}

int8_t MpHvacRelayOutputs::read( Storm::Type::HvacRelayOutputs_T& dstData, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &dstData, sizeof( Storm::Type::HvacRelayOutputs_T ), seqNumPtr );
}

uint16_t MpHvacRelayOutputs::write( const Storm::Type::HvacRelayOutputs_T& srcData, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::HvacRelayOutputs_T newData = srcData;
    validate( newData );
    return ModelPointCommon_::write( &newData, sizeof( Storm::Type::HvacRelayOutputs_T ), lockRequest );
}


uint16_t MpHvacRelayOutputs::setSafeAllOff( LockRequest_T lockRequest ) noexcept
{
    Storm::Type::HvacRelayOutputs_T newData;
    setSafeAllOff( newData );

    m_modelDatabase.lock_();
    newData.o = m_data.o;
    uint16_t result = write( newData, lockRequest );
    m_modelDatabase.unlock_();

    return result;
}


uint16_t MpHvacRelayOutputs::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void MpHvacRelayOutputs::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpHvacRelayOutputs::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

bool MpHvacRelayOutputs::isDataEqual_( const void* otherData ) const noexcept
{
    return  memcmp( &m_data, otherData, sizeof( m_data ) ) == 0;
}

void MpHvacRelayOutputs::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Storm::Type::HvacRelayOutputs_T ) );
    Storm::Type::HvacRelayOutputs_T* dstDataPtr = ( Storm::Type::HvacRelayOutputs_T* ) dstData;
    *dstDataPtr = m_data;
}

void MpHvacRelayOutputs::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Storm::Type::HvacRelayOutputs_T ) );
    Storm::Type::HvacRelayOutputs_T* dataSrcPtr = ( Storm::Type::HvacRelayOutputs_T* ) srcData;
    m_data = *dataSrcPtr;
}


///////////////////////////////////////////////////////////////////////////////
const char* MpHvacRelayOutputs::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpHvacRelayOutputs";
}

size_t MpHvacRelayOutputs::getSize() const noexcept
{
    return sizeof( Storm::Type::HvacRelayOutputs_T );
}

size_t MpHvacRelayOutputs::getInternalDataSize_() const noexcept
{
    return sizeof( Storm::Type::HvacRelayOutputs_T );
}


const void* MpHvacRelayOutputs::getImportExportDataPointer_() const noexcept
{
    return &m_data;
}

bool MpHvacRelayOutputs::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
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
        JsonObject valObj  = doc.createNestedObject( "val" );
        valObj["g"]   = m_data.g;
        valObj["bk"]  = m_data.bk;
        valObj["w1"]  = m_data.w1;
        valObj["w2"]  = m_data.w2;
        valObj["w3"]  = m_data.w3;
        valObj["y1"]  = m_data.y1;
        valObj["y2"]  = m_data.y2;
        valObj["o"]   = m_data.o ? "COOL" : "HEAT";
    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose );

    m_modelDatabase.unlock_();
    return true;
}


bool MpHvacRelayOutputs::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Storm::Type::HvacRelayOutputs_T updatedData = m_data;

    if ( src.is<JsonObject>() )
    {
        // Blower speed
        unsigned bk = src["bk"] | INVALID_VALUE;
        if ( bk != INVALID_VALUE )
        {
            updatedData.bk = bk;
        }

        // Relay outputs
        updatedData.g  = src["g"] | updatedData.g;
        updatedData.w1 = src["w1"] | updatedData.w1;
        updatedData.w2 = src["w2"] | updatedData.w2;
        updatedData.w3 = src["w3"] | updatedData.w3;
        updatedData.y1 = src["y1"] | updatedData.y1;
        updatedData.y2 = src["y2"] | updatedData.y2;

        // SOV state
        const char* sov = src["o"];
        if ( sov )
        {
            if ( strcmp( sov, "COOL" ) == 0 )
            {
                updatedData.o = true;
            }
            else if ( strcmp( sov, "HEAT" ) == 0 )
            {
                updatedData.o = false;
            }

        }
    }

    retSequenceNumber = write( updatedData, lockRequest );
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void MpHvacRelayOutputs::validate( Storm::Type::HvacRelayOutputs_T& newValues ) const noexcept
{
    if ( newValues.bk > MAX_OUTPUT )
    {
        newValues.bk = MAX_OUTPUT;
    }

}

void MpHvacRelayOutputs::setSafeAllOff( Storm::Type::HvacRelayOutputs_T& outputs )
{
    outputs.bk = 0;
    outputs.g  = false;
    outputs.w1 = false;
    outputs.w2 = false;
    outputs.w3 = false;
    outputs.y1 = false;
    outputs.y2 = false;
}


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


#include "MpEquipmentBeginTimes.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Math/real.h"
#include <string.h>

///
using namespace Storm::Dm;


///////////////////////////////////////////////////////////////////////////////
MpEquipmentBeginTimes::MpEquipmentBeginTimes( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, MODEL_POINT_STATE_VALID )
{
    memset( &m_data, 0, sizeof( m_data ) );
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpEquipmentBeginTimes::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
    // Zero out the MP when invalidating the Model Point
    m_modelDatabase.lock_();
    memset( &m_data, 0, sizeof( m_data ) );
    uint16_t result = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );
    m_modelDatabase.unlock_();
    return result;
}

int8_t MpEquipmentBeginTimes::read( Storm::Type::EquipmentTimes_T& dstData, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &dstData, sizeof( Storm::Type::EquipmentTimes_T ), seqNumPtr );
}

uint16_t MpEquipmentBeginTimes::write( const Storm::Type::EquipmentTimes_T& srcData, LockRequest_T lockRequest ) noexcept
{
    return ModelPointCommon_::write( &srcData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
}

uint16_t MpEquipmentBeginTimes::setIndoorUnitBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                       = m_data;
    newData.indoorUnitBeginOnTime = newBeginOnCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}
uint16_t MpEquipmentBeginTimes::setIndoorUnitBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                        = m_data;
    newData.indoorUnitBeginOffTime = newBeginOffCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpEquipmentBeginTimes::setOutdoorUnitBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                        = m_data;
    newData.outdoorUnitBeginOnTime = newBeginOnCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}
uint16_t MpEquipmentBeginTimes::setOutdoorUnitBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                         = m_data;
    newData.outdoorUnitBeginOffTime = newBeginOffCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpEquipmentBeginTimes::setSystemBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                   = m_data;
    newData.systemBeginOnTime = newBeginOnCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}
uint16_t MpEquipmentBeginTimes::setSystemBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                    = m_data;
    newData.systemBeginOffTime = newBeginOffCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpEquipmentBeginTimes::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void MpEquipmentBeginTimes::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpEquipmentBeginTimes::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

bool MpEquipmentBeginTimes::isDataEqual_( const void* otherData ) const noexcept
{
    return memcmp( &m_data, otherData, sizeof( m_data ) ) == 0;
}

void MpEquipmentBeginTimes::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Storm::Type::EquipmentTimes_T ) );
    Storm::Type::EquipmentTimes_T* dstDataPtr = ( Storm::Type::EquipmentTimes_T* ) dstData;
    *dstDataPtr                               = m_data;
}

void MpEquipmentBeginTimes::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Storm::Type::EquipmentTimes_T ) );
    Storm::Type::EquipmentTimes_T* dataSrcPtr = ( Storm::Type::EquipmentTimes_T* ) srcData;
    m_data                                    = *dataSrcPtr;
}


///////////////////////////////////////////////////////////////////////////////
const char* MpEquipmentBeginTimes::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpEquipmentBeginTimes";
}

size_t MpEquipmentBeginTimes::getSize() const noexcept
{
    return sizeof( Storm::Type::EquipmentTimes_T );
}

size_t MpEquipmentBeginTimes::getInternalDataSize_() const noexcept
{
    return sizeof( Storm::Type::EquipmentTimes_T );
}


const void* MpEquipmentBeginTimes::getImportExportDataPointer_() const noexcept
{
    return &m_data;
}

bool MpEquipmentBeginTimes::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
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
        JsonObject valObj                = doc.createNestedObject( "val" );
        valObj["beginIndoorOnTimeSec"]   = ( double) ( m_data.indoorUnitBeginOnTime.m_seconds ) + ( m_data.indoorUnitBeginOnTime.m_thousandths / 1000.0 );
        valObj["beginIndoorOffTimeSec"]  = ( double) ( m_data.indoorUnitBeginOffTime.m_seconds ) + ( m_data.indoorUnitBeginOffTime.m_thousandths / 1000.0 );
        valObj["beginOutdoorOnTimeSec"]  = ( double) ( m_data.outdoorUnitBeginOnTime.m_seconds ) + ( m_data.outdoorUnitBeginOnTime.m_thousandths / 1000.0 );
        valObj["beginOutdoorOffTimeSec"] = ( double) ( m_data.outdoorUnitBeginOffTime.m_seconds ) + ( m_data.outdoorUnitBeginOffTime.m_thousandths / 1000.0 );
    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose );

    m_modelDatabase.unlock_();
    return true;
}

#define INVALID_BEGIN_TIME  (-1.0)

bool MpEquipmentBeginTimes::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Storm::Type::EquipmentTimes_T updatedData = m_data;

    // Parse Fields
    double beginTime = src["beginIndoorOnTimeSec"] | INVALID_BEGIN_TIME;
    if ( !Cpl::Math::areDoublesEqual( beginTime, INVALID_BEGIN_TIME ) )
    {
        updatedData.indoorUnitBeginOnTime.m_seconds     = ( unsigned long) beginTime;
        updatedData.indoorUnitBeginOnTime.m_thousandths = ( uint16_t) ( ( beginTime - updatedData.indoorUnitBeginOnTime.m_seconds ) * 1000.0 + 0.5 );
    }
    beginTime = src["beginIndoorOffTimeSec"] | INVALID_BEGIN_TIME;
    if ( !Cpl::Math::areDoublesEqual( beginTime, INVALID_BEGIN_TIME ) )
    {
        updatedData.indoorUnitBeginOffTime.m_seconds     = ( unsigned long) beginTime;
        updatedData.indoorUnitBeginOffTime.m_thousandths = ( uint16_t) ( ( beginTime - updatedData.indoorUnitBeginOffTime.m_seconds ) * 1000.0 + 0.5 );
    }

    beginTime = src["beginOutdoorOnTimeSec"] | INVALID_BEGIN_TIME;
    if ( !Cpl::Math::areDoublesEqual( beginTime, INVALID_BEGIN_TIME ) )
    {
        updatedData.outdoorUnitBeginOnTime.m_seconds     = ( unsigned long) beginTime;
        updatedData.outdoorUnitBeginOnTime.m_thousandths = ( uint16_t) ( ( beginTime - updatedData.outdoorUnitBeginOnTime.m_seconds ) * 1000.0 + 0.5 );
    }
    beginTime = src["beginOutdoorOffTimeSec"] | INVALID_BEGIN_TIME;
    if ( !Cpl::Math::areDoublesEqual( beginTime, INVALID_BEGIN_TIME ) )
    {
        updatedData.outdoorUnitBeginOffTime.m_seconds     = ( unsigned long) beginTime;
        updatedData.outdoorUnitBeginOffTime.m_thousandths = ( uint16_t) ( ( beginTime - updatedData.outdoorUnitBeginOffTime.m_seconds ) * 1000.0 + 0.5 );
    }

    retSequenceNumber = write( updatedData, lockRequest );
    return true;
}


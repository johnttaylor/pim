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


#include "MpCycleInfo.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Math/real.h"
#include <string.h>

///
using namespace Storm::Dm;


///////////////////////////////////////////////////////////////////////////////
MpCycleInfo::MpCycleInfo( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, MODEL_POINT_STATE_VALID )
{
    memset( &m_data, 0, sizeof( m_data ) );
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpCycleInfo::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
    // Zero out the MP when invalidating the Model Point
    m_modelDatabase.lock_();
    memset( &m_data, 0, sizeof( m_data ) );
    uint16_t result = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );
    m_modelDatabase.unlock_();
    return result;
}

int8_t MpCycleInfo::read( Storm::Type::CycleInfo_T& dstData, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &dstData, sizeof( Storm::Type::CycleInfo_T ), seqNumPtr );
}

uint16_t MpCycleInfo::write( const Storm::Type::CycleInfo_T& srcData, LockRequest_T lockRequest ) noexcept
{
    return ModelPointCommon_::write( &srcData, sizeof( Storm::Type::CycleInfo_T ), lockRequest );
}

uint16_t MpCycleInfo::setOnTime( uint32_t newOnCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::CycleInfo_T newData;
    m_modelDatabase.lock_();

    newData          = m_data;
    newData.onTime   = newOnCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::CycleInfo_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpCycleInfo::setOffTime( uint32_t newOffCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::CycleInfo_T newData;
    m_modelDatabase.lock_();

    newData          = m_data;
    newData.offTime  = newOffCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::CycleInfo_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}
uint16_t MpCycleInfo::setBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::CycleInfo_T newData;
    m_modelDatabase.lock_();

    newData             = m_data;
    newData.beginOnTime = newBeginOnCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::CycleInfo_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}
uint16_t MpCycleInfo::setBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::CycleInfo_T newData;
    m_modelDatabase.lock_();

    newData              = m_data;
    newData.beginOffTime = newBeginOffCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::CycleInfo_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpCycleInfo::setMode( Storm::Type::CycleStatus newMode, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::CycleInfo_T newData;
    m_modelDatabase.lock_();

    newData      = m_data;
    newData.mode = newMode;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::CycleInfo_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpCycleInfo::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void MpCycleInfo::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpCycleInfo::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

bool MpCycleInfo::isDataEqual_( const void* otherData ) const noexcept
{
    return memcmp( &m_data, otherData, sizeof( m_data ) ) == 0;
}

void MpCycleInfo::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Storm::Type::CycleInfo_T ) );
    Storm::Type::CycleInfo_T* dstDataPtr = ( Storm::Type::CycleInfo_T* ) dstData;
    *dstDataPtr                          = m_data;
}

void MpCycleInfo::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Storm::Type::CycleInfo_T ) );
    Storm::Type::CycleInfo_T* dataSrcPtr = ( Storm::Type::CycleInfo_T* ) srcData;
    m_data                               = *dataSrcPtr;
}


///////////////////////////////////////////////////////////////////////////////
const char* MpCycleInfo::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpCycleInfo";
}

size_t MpCycleInfo::getSize() const noexcept
{
    return sizeof( Storm::Type::CycleInfo_T );
}

size_t MpCycleInfo::getInternalDataSize_() const noexcept
{
    return sizeof( Storm::Type::CycleInfo_T );
}


const void* MpCycleInfo::getImportExportDataPointer_() const noexcept
{
    return &m_data;
}

bool MpCycleInfo::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
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
        JsonObject valObj         = doc.createNestedObject( "val" );
        valObj["onTimeMsec"]      = m_data.onTime;
        valObj["offTimeMsec"]     = m_data.offTime;
        valObj["beginOnTimeSec"]  = ( double) ( m_data.beginOnTime.m_seconds ) +  ( m_data.beginOnTime.m_thousandths / 1000.0 );
        valObj["beginOffTimeSec"] = ( double) ( m_data.beginOffTime.m_seconds ) + ( m_data.beginOffTime.m_thousandths / 1000.0 );
        valObj["mode"]            = Storm::Type::CycleStatus::_from_integral_unchecked( m_data.mode )._to_string();
    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose );

    m_modelDatabase.unlock_();
    return true;
}

#define INVALID_CYCLE_TIME  ((uint32_t)-1)
#define INVALID_BEGIN_TIME  (-1.0)

bool MpCycleInfo::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Storm::Type::CycleInfo_T updatedData = m_data;

    // Parse Fields
    uint32_t cycleTime = src["onTimeMsec"] | INVALID_CYCLE_TIME;
    if ( cycleTime != INVALID_CYCLE_TIME )
    {
        updatedData.onTime = cycleTime;
    }
    cycleTime = src["offTimeMsec"] | INVALID_CYCLE_TIME;
    if ( cycleTime != INVALID_CYCLE_TIME )
    {
        updatedData.offTime = cycleTime;
    }

    double beginTime = src["beginOnTimeSec"] | INVALID_BEGIN_TIME;
    if ( !Cpl::Math::areDoublesEqual( beginTime, INVALID_BEGIN_TIME ) )
    {
        updatedData.beginOnTime.m_seconds     = ( unsigned long) beginTime;
        updatedData.beginOnTime.m_thousandths = ( uint16_t) ( ( beginTime - updatedData.beginOnTime.m_seconds ) * 1000.0 + 0.5 );
    }
    beginTime = src["beginOffTimeSec"] | INVALID_BEGIN_TIME;
    if ( !Cpl::Math::areDoublesEqual( beginTime, INVALID_BEGIN_TIME ) )
    {
        updatedData.beginOffTime.m_seconds     = ( unsigned long) beginTime;
        updatedData.beginOffTime.m_thousandths = ( uint16_t) ( ( beginTime - updatedData.beginOffTime.m_seconds ) * 1000.0 + 0.5 );
    }

    const char* enumVal = src["mode"];
    if ( enumVal )
    {
        // Convert the text to an enum value
        auto maybeValue = Storm::Type::CycleStatus::_from_string_nothrow( enumVal );
        if ( !maybeValue )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Invalid enum value (%s)", enumVal );
            }
            return false;
        }

        updatedData.mode = *maybeValue;
    }

    retSequenceNumber = write( updatedData, lockRequest );
    return true;
}


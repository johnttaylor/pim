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


#include "MpIdtAlarm.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include <string.h>

///
using namespace Storm::Dm;

static bool getBooleanValue( JsonObject& src, const char* key, bool& newValue );

///////////////////////////////////////////////////////////////////////////////
MpIdtAlarm::MpIdtAlarm( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, MODEL_POINT_STATE_VALID )
    , m_data( { false, false, false, false, false } )
{
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpIdtAlarm::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
    // Clear all of the flags when invalidating the Model Point
    m_modelDatabase.lock_();
    m_data = { false, false, false, false, false };
    uint16_t result = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );
    m_modelDatabase.unlock_();
    return result;
}

int8_t MpIdtAlarm::read( Data& dstData, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &dstData, sizeof( Data ), seqNumPtr );
}

uint16_t MpIdtAlarm::write( const Data& srcData, LockRequest_T lockRequest ) noexcept
{
    return ModelPointCommon_::write( &srcData, sizeof( Data ), lockRequest );
}

uint16_t MpIdtAlarm::setAlarm( bool primaryAlarmState, bool secondaryAlarmState, bool isCritical, LockRequest_T lockRequest ) noexcept
{
    Data newData;
    m_modelDatabase.lock_();

    newData                = m_data;
    newData.primaryAlarm   = primaryAlarmState;
    newData.secondaryAlarm = secondaryAlarmState;
    newData.critical       = isCritical;

    // Clear ACK flags on transition to Active Alarm
    if ( primaryAlarmState && m_data.primaryAlarm == false)
    {
        newData.primaryAck = false;
    }
    if ( secondaryAlarmState && m_data.secondaryAlarm == false )
    {
        newData.secondaryAck = false;
    }
    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Data ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpIdtAlarm::acknowledgePrimaryAlarm( LockRequest_T lockRequest ) noexcept
{
    Data newData;
    m_modelDatabase.lock_();

    newData            = m_data;
    newData.primaryAck = true;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Data ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpIdtAlarm::acknowledgeSecondaryAlarm( LockRequest_T lockRequest ) noexcept
{
    Data newData;
    m_modelDatabase.lock_();

    newData              = m_data;
    newData.secondaryAck = true;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Data ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpIdtAlarm::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void MpIdtAlarm::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpIdtAlarm::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

bool MpIdtAlarm::isDataEqual_( const void* otherData ) const noexcept
{
    Data* otherDataPtr = ( Data*) otherData;

    // Note: By comparing every field -->I don't have worry about pack-bytes in the data structure
    return  otherDataPtr->primaryAlarm == m_data.primaryAlarm &&
        otherDataPtr->primaryAck == m_data.primaryAck &&
        otherDataPtr->secondaryAlarm == m_data.secondaryAlarm &&
        otherDataPtr->secondaryAck == m_data.secondaryAck &&
        otherDataPtr->critical == m_data.critical;
}

void MpIdtAlarm::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Data ) );
    Data* dstDataPtr   = ( Data*) dstData;
    *dstDataPtr        = m_data;
}

void MpIdtAlarm::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Data ) );
    Data* dataSrcPtr   = ( Data*) srcData;
    m_data             = *dataSrcPtr;
}


///////////////////////////////////////////////////////////////////////////////
const char* MpIdtAlarm::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpIdtAlarm";
}

size_t MpIdtAlarm::getSize() const noexcept
{
    return sizeof( Data );
}

size_t MpIdtAlarm::getInternalDataSize_() const noexcept
{
    return sizeof( Data );
}


const void* MpIdtAlarm::getImportExportDataPointer_() const noexcept
{
    return &m_data;
}

bool MpIdtAlarm::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
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
        JsonObject valObj = doc.createNestedObject( "val" );
        valObj["priAlarm"] = m_data.primaryAlarm;
        valObj["priAck"] = m_data.primaryAck;
        valObj["secAlarm"] = m_data.secondaryAlarm;
        valObj["secAck"] = m_data.secondaryAck;
        valObj["critical"] = m_data.critical;
    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose );

    m_modelDatabase.unlock_();
    return true;
}

bool MpIdtAlarm::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Data updatedData = m_data;
    int missingCount = 0;

    // Parse Fields
    JsonObject valObj = src;
    if ( !getBooleanValue( valObj, "priAlarm", updatedData.primaryAlarm ) )
    {
        missingCount++;
    }
    if ( !getBooleanValue( valObj, "priAck", updatedData.primaryAck ) )
    {
        missingCount++;
    }
    if ( !getBooleanValue( valObj, "secAlarm", updatedData.secondaryAlarm ) )
    {
        missingCount++;
    }
    if ( !getBooleanValue( valObj, "secAck", updatedData.secondaryAck ) )
    {
        missingCount++;
    }
    if ( !getBooleanValue( valObj, "critical", updatedData.critical ) )
    {
        missingCount++;
    }

    // Throw an error if NO valid key/value pairs where specified
    if ( missingCount == 5 )
    {
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }

    retSequenceNumber = write( updatedData, lockRequest );
    return true;
}

bool getBooleanValue( JsonObject& src, const char* key, bool& newValue )
{
    // Attempt to parse the value key/value pair
    bool checkForError  =  src[key] | false;
    bool checkForError2 = src[key] | true;
    if ( checkForError2 == true && checkForError == false )
    {
        return false;
    }
    newValue = checkForError;
    return true;
}
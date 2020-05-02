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


#include "MpSimpleAlarm.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include <string.h>

///
using namespace Storm::Dm;

static bool getBooleanValue( JsonObject& src, const char* key, bool& newValue );

///////////////////////////////////////////////////////////////////////////////
MpSimpleAlarm::MpSimpleAlarm( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, MODEL_POINT_STATE_VALID )
    , m_data( { false, false, false } )
{
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpSimpleAlarm::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
    // Clear all of the flags when invalidating the Model Point
    m_modelDatabase.lock_();
    m_data = { false, false, false };
    uint16_t result = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );
    m_modelDatabase.unlock_();
    return result;
}

int8_t MpSimpleAlarm::read( Data& dstData, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &dstData, sizeof( Data ), seqNumPtr );
}

uint16_t MpSimpleAlarm::write( const Data& srcData, LockRequest_T lockRequest ) noexcept
{
    return ModelPointCommon_::write( &srcData, sizeof( Data ), lockRequest );
}

uint16_t MpSimpleAlarm::setAlarm( bool active, bool isCritical, LockRequest_T lockRequest ) noexcept
{
    Data newData;
    m_modelDatabase.lock_();

    newData          = m_data;
    newData.active   = active;
    newData.critical = isCritical;
    
    // ALWAYS clear ACK flag when the active status changes
    if ( active != m_data.active )
    {
        newData.acked = false;
    }
    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Data ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpSimpleAlarm::acknowledgeAlarm( LockRequest_T lockRequest ) noexcept
{
    Data newData;
    m_modelDatabase.lock_();

    newData       = m_data;
    newData.acked = true;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Data ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpSimpleAlarm::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void MpSimpleAlarm::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpSimpleAlarm::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

bool MpSimpleAlarm::isDataEqual_( const void* otherData ) const noexcept
{
    Data* otherDataPtr = ( Data*) otherData;

    // Note: By comparing every field -->I don't have worry about pack-bytes in the data structure
    return  otherDataPtr->active == m_data.active &&
        otherDataPtr->acked == m_data.acked &&
        otherDataPtr->critical == m_data.critical;
}

void MpSimpleAlarm::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Data ) );
    Data* dstDataPtr   = ( Data*) dstData;
    *dstDataPtr        = m_data;
}

void MpSimpleAlarm::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Data ) );
    Data* dataSrcPtr   = ( Data*) srcData;
    m_data             = *dataSrcPtr;
}


///////////////////////////////////////////////////////////////////////////////
const char* MpSimpleAlarm::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpSimpleAlarm";
}

size_t MpSimpleAlarm::getSize() const noexcept
{
    return sizeof( Data );
}

size_t MpSimpleAlarm::getInternalDataSize_() const noexcept
{
    return sizeof( Data );
}


const void* MpSimpleAlarm::getImportExportDataPointer_() const noexcept
{
    return &m_data;
}

bool MpSimpleAlarm::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
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
        valObj["active"]   = m_data.active;
        valObj["ack"]      = m_data.acked;
        valObj["critical"] = m_data.critical;
    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose );

    m_modelDatabase.unlock_();
    return true;
}

bool MpSimpleAlarm::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Data updatedData = m_data;
    int missingCount = 0;

    // Parse Fields
    JsonObject valObj = src;
    if ( !getBooleanValue( valObj, "active", updatedData.active ) )
    {
        missingCount++;
    }
    if ( !getBooleanValue( valObj, "ack", updatedData.acked ) )
    {
        missingCount++;
    }
    if ( !getBooleanValue( valObj, "critical", updatedData.critical ) )
    {
        missingCount++;
    }

    // Throw an error if NO valid key/value pairs where specified
    if ( missingCount == 3 )
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
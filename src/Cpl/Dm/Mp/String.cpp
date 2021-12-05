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


#include "String.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include <string.h>
#include <new>

#define ESTIMATED_JSON_OVERHEAD		128

#define META_DATA_OVERHEAD          (1 + sizeof( String::Data ))    // Include the Metadata and space for the null terminator

///
using namespace Cpl::Dm::Mp;

static char emptyString_[1] = { '\0' };

///////////////////////////////////////////////////////////////////////////////
Cpl::Dm::Mp::String::String( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, size_t maxLength )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, OPTION_CPL_DM_MODEL_POINT_STATE_INVALID )
    , m_data( { new( std::nothrow ) char[maxLength + 1], 0, maxLength } )
{
    // Throw a fatal error if global parse buffer is too small
    if ( ( OPTION_CPL_DM_MODEL_DATABASE_MAX_CAPACITY_JSON_DOC - ESTIMATED_JSON_OVERHEAD ) < maxLength )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::String().  Creating a string of size %lu which is greater than the fromString() parser buffer", maxLength );
    }

    // Trapped failed to allocate memory -->silent fail and set string size to zero
    if ( m_data.stringPtr == 0 )
    {
        m_data.stringPtr = emptyString_;
        m_data.maxLength = 0;
    }
    else
    {
        // For a deterministic value - initialize data to 'empty string'Null pointer for initial value -->set initial value to an empty string
        m_data.stringPtr[0] = '\0';
        m_data.stringLen    = 0;
    }
}

Cpl::Dm::Mp::String::String( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, size_t maxLength, const char* initialValue )
    :ModelPointCommon_( myModelBase, &m_data, staticInfo, Cpl::Dm::ModelPoint::MODEL_POINT_STATE_VALID )
    , m_data( { new( std::nothrow ) char[maxLength + 1], 0, maxLength } )
{
    // Trapped failed to allocate memory -->silent fail and set string size to zero
    if ( m_data.stringPtr == 0 )
    {
        m_data.stringPtr = emptyString_;
        m_data.maxLength = 0;
    }

    // Set the initial value
    if ( initialValue )
    {
        strncpy( m_data.stringPtr, initialValue, maxLength );
        m_data.stringPtr[m_data.maxLength] = '\0';
    }

    // Null pointer for initial value -->set initial value to an empty string
    else
    {
        m_data.stringPtr[0] = '\0';
    }

    // Set my string length
    m_data.stringLen = strlen( m_data.stringPtr );
}

Cpl::Dm::Mp::String::~String()
{
    // Free up the memory IF it was allocated from the heap
    if ( m_data.maxLength > 0 )
    {
        delete m_data.stringPtr;
    }
}

///////////////////////////////////////////////////////////////////////////////
int8_t Cpl::Dm::Mp::String::read( Cpl::Text::String& dstData, uint16_t* seqNumPtr ) const noexcept
{
    Data dst;
    int  bufferMaxLength;
    dst.stringPtr = dstData.getBuffer( bufferMaxLength );
    dst.maxLength = bufferMaxLength;
    return ModelPointCommon_::read( &dst, sizeof( Data ), seqNumPtr );
}

int8_t Cpl::Dm::Mp::String::read( Data& dstData, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &dstData, sizeof( Data ), seqNumPtr );
}


uint16_t Cpl::Dm::Mp::String::write( const Data& srcData, LockRequest_T lockRequest ) noexcept
{
    // Trap the null pointer case -->Do NOTHING
    if ( srcData.stringPtr == 0 )
    {
        m_modelDatabase.lock_();
        uint16_t seqNum = m_seqNum;
        m_modelDatabase.unlock_();
        return seqNum;
    }

    return ModelPointCommon_::write( &srcData, sizeof( Data ), lockRequest );
}

uint16_t Cpl::Dm::Mp::String::write( const char* newValue, LockRequest_T lockRequest ) noexcept
{
    // Trap the null pointer case -->Do NOTHING
    if ( newValue == 0 )
    {
        m_modelDatabase.lock_();
        uint16_t seqNum = m_seqNum;
        m_modelDatabase.unlock_();
        return seqNum;
    }

    return write( newValue, strlen( newValue ), lockRequest );
}

uint16_t Cpl::Dm::Mp::String::write( const char* srcData, size_t srcLen, LockRequest_T lockRequest ) noexcept
{
    Data src = { (char*) srcData, srcLen, srcLen };
    return ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );
}

uint16_t Cpl::Dm::Mp::String::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void Cpl::Dm::Mp::String::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Cpl::Dm::Mp::String::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

bool Cpl::Dm::Mp::String::isDataEqual_( const void* otherData ) const noexcept
{
    Data*  otherDataPtr = (Data*) otherData;

    return otherDataPtr->stringLen == m_data.stringLen && strncmp( m_data.stringPtr, otherDataPtr->stringPtr, m_data.stringLen ) == 0;
}

void Cpl::Dm::Mp::String::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Data ) );
    Data* dataDstPtr = (Data*) dstData;

    dstSize = dataDstPtr->maxLength >= m_data.stringLen ? m_data.stringLen : dataDstPtr->maxLength;
    memcpy( dataDstPtr->stringPtr, m_data.stringPtr, dstSize );
    dataDstPtr->stringPtr[dstSize] = '\0';
    dataDstPtr->stringLen          = dstSize;
}

void Cpl::Dm::Mp::String::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Data ) );
    Data* dataSrcPtr = (Data*) srcData;

    srcSize = dataSrcPtr->stringLen <= m_data.maxLength ? dataSrcPtr->stringLen : m_data.maxLength;
    memcpy( m_data.stringPtr, dataSrcPtr->stringPtr, srcSize );
    m_data.stringPtr[srcSize] = '\0';
    m_data.stringLen          = srcSize;
}


///////////////////////////////////////////////////////////////////////////////
const char* Cpl::Dm::Mp::String::getTypeAsText() const noexcept
{
    return "Cpl::Dm::Mp::String";
}

size_t Cpl::Dm::Mp::String::getSize() const noexcept
{
    return m_data.maxLength;
}

size_t Cpl::Dm::Mp::String::getInternalDataSize_() const noexcept
{
    return m_data.maxLength + META_DATA_OVERHEAD;
}


bool Cpl::Dm::Mp::String::importMetadata_( const void* srcDataStream, size_t& bytesConsumed ) noexcept
{
    // NOTE: Use memcpy instead of the assignment operator since the alignment of 'srcDataStream' is unknown/not-guaranteed 
    size_t          incomingMaxLen;
    size_t          incomingStringLen;
    String::Data*   incoming = ( String::Data* ) srcDataStream;
    memcpy( &incomingMaxLen, &( incoming->maxLength ), sizeof( incomingMaxLen ) );
    memcpy( &incomingStringLen, &( incoming->stringLen), sizeof( incomingStringLen ) );
    
    // Incoming data MUST fit with the previously allocated memory
    if ( incomingMaxLen> m_data.maxLength || incomingStringLen > m_data.maxLength )
    {
        return false;
    }

    // Capture the incoming string length and ensure that the incoming string data gets null terminated        
    bytesConsumed                       = sizeof( String::Data );
    m_data.stringLen                    = incomingStringLen;
    m_data.stringPtr[incomingStringLen] = '\0';
    return true;
}

bool Cpl::Dm::Mp::String::exportMetadata_( void* dstDataStream, size_t& bytesAdded ) const noexcept
{
    // NOTE: Use memcpy instead of the assignment operator since the alignment of 'dstDataStream' is unknown/not-guaranteed 
    String::Data* dst = ( String::Data* ) dstDataStream;
    size_t stringLen  = m_validState == MODEL_POINT_STATE_VALID ? m_data.stringLen : 0;
    memcpy( &( dst->maxLength ), &( m_data.maxLength ), sizeof( dst->maxLength  ) );
    memcpy( &( dst->stringLen ), &stringLen, sizeof( dst->maxLength ) );
    memset( &( dst->stringPtr ), 0, sizeof( dst->stringPtr ) );
    bytesAdded        = sizeof( String::Data );
    return true;
}

const void* Cpl::Dm::Mp::String::getImportExportDataPointer_() const noexcept
{
    return m_data.stringPtr;
}

bool Cpl::Dm::Mp::String::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
{
    // Get my state
    m_modelDatabase.lock_();
    uint16_t seqnum = m_seqNum;
    int8_t   valid  = m_validState;
    bool     locked = m_locked;

    // Start the conversion
    JsonDocument& doc = beginJSON( valid, locked, seqnum, verbose );

    // Add maxlen key/value
    if ( verbose )
    {
        doc["maxlen"] = m_data.maxLength;
    }

    // Construct the 'val' key/value pair 
    if ( IS_VALID( valid ) )
    {
        doc["val"] = m_data.stringPtr;
    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose );

    m_modelDatabase.unlock_();
    return true;
}

bool Cpl::Dm::Mp::String::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    // Does the value key/value pair exist?
    const char* newValue = src;
    if ( newValue == nullptr )
    {
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }

    // Is the string too long?
    if ( strlen( newValue ) > m_data.maxLength )
    {
        if ( errorMsg )
        {
            errorMsg->format( "String is too long (len=%lu) for the model point (max len=%lu)", (unsigned long) strlen( newValue ), (unsigned long) m_data.maxLength );
        }
        return false;

    }
    retSequenceNumber = write( newValue, lockRequest );
    return true;
}



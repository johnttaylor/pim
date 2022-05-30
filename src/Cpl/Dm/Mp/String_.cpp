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


#include "String_.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include <string.h>

#define ESTIMATED_JSON_OVERHEAD		128

#define META_DATA_OVERHEAD          (1 + sizeof( String::Data ))    // Include the Metadata and space for the null terminator

///
using namespace Cpl::Dm::Mp;

///////////////////////////////////////////////////////////////////////////////
StringBase_::StringBase_( Cpl::Dm::ModelDatabase& myModelBase,
                          const char*             symbolicName,
                          char*                   myDataPtr,
                          size_t                  dataSizeInBytesIncludingNullTerminator )
    : Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, myDataPtr, dataSizeInBytesIncludingNullTerminator, false )
{
    // Clear the entire string INCLUDING the null terminator
    // NOTE: The Null terminator NEVER gets written over
    memset( (void*) myDataPtr, 0, dataSizeInBytesIncludingNullTerminator );
}

/// Constructor. Valid MP.  Requires an initial value
StringBase_::StringBase_( Cpl::Dm::ModelDatabase& myModelBase,
                          const char*             symbolicName,
                          char*                   myDataPtr,
                          size_t                  dataSizeInBytesIncludingNullTerminator,
                          const char*             initialValue )
    : Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, myDataPtr, dataSizeInBytesIncludingNullTerminator, true )
{
    // Set the initial value
    strncpy( (char*) myDataPtr, initialValue, dataSizeInBytesIncludingNullTerminator );
    
    // Make sure that the last byte of the raw storage is the NULL TERMINATOR
    // NOTE: The Null terminator NEVER gets written over
    ((char*)myDataPtr)[dataSizeInBytesIncludingNullTerminator-1] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
bool StringBase_::read( Cpl::Text::String& dstData, uint16_t* seqNumPtr ) const noexcept
{
    int   bufferMaxLength;  
    char* dstStringPtr = dstData.getBuffer( bufferMaxLength );
    return read( dstStringPtr, bufferMaxLength+1, seqNumPtr ); // Note: the 'bufferMaxLength' returned does NOT include space for the null terminator
}

bool StringBase_::read( char* dstData, size_t dataSizeInBytesIncludingNullTerminator, uint16_t* seqNumPtr ) const noexcept
{
    // Max sure the length does not exceed the MP's 'string' storage
    if ( dataSizeInBytesIncludingNullTerminator > m_dataSize )
    {
        dataSizeInBytesIncludingNullTerminator = m_dataSize;
    }

    // Ensure the returned result is always null terminated
    bool valid = ModelPointCommon_::read( dstData, dataSizeInBytesIncludingNullTerminator, seqNumPtr );
    dstData[dataSizeInBytesIncludingNullTerminator - 1] = '\0';
    return valid;
}

uint16_t StringBase_::write( const char* srcData, size_t srcLenInBytesIncludingNullTerminator, LockRequest_T lockRequest ) noexcept
{
    // Trap the null pointer case -->Do NOTHING
    if ( srcData == 0 )
    {
        return getSequenceNumber();
    }

    // Max sure the length does not exceed the MP's 'string' storage
    if ( srcLenInBytesIncludingNullTerminator > m_dataSize )
    {
        srcLenInBytesIncludingNullTerminator = m_dataSize;
    }
    
    m_modelDatabase.lock_();
    uint16_t seqNum = ModelPointCommon_::write( srcData, srcLenInBytesIncludingNullTerminator, lockRequest );
    ((char*) (m_dataPtr))[m_dataSize - 1] = '\0'; // Ensure my new value properly null terminated
    m_modelDatabase.unlock_();
    return seqNum;
}

uint16_t StringBase_::copyFrom( const StringBase_& src, LockRequest_T lockRequest ) noexcept
{
    // Handle the src.invalid case
    if ( src.isNotValid() )
    {
        return setInvalid();
    }

    m_modelDatabase.lock_();
    uint16_t seqNum = StringBase_::write( (const char*) src.m_dataPtr, src.m_dataSize, lockRequest );
    m_modelDatabase.unlock_();
    return seqNum;
}


bool StringBase_::isDataEqual_( const void* otherData ) const noexcept
{
    const char* otherStringPtr = (const char*) otherData;
    size_t otherLen = strlen( otherStringPtr );
    size_t myLen    = strlen( (const char*) m_dataPtr );
    return otherLen == myLen && strncmp( otherStringPtr, (const char*) m_dataPtr, myLen ) == 0;
}

void StringBase_::setJSONVal( JsonDocument& doc ) noexcept
{
    doc["val"] = (char*) m_dataPtr;
}


bool StringBase_::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
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

    // Note: if the incoming string is longer than the MP storage, it will be truncated
    retSequenceNumber = write( newValue, lockRequest );
    return true;
}

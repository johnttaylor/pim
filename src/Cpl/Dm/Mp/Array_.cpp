/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */


#include "Array_.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include <memory.h>

using namespace Cpl::Dm::Mp;

/////////////////////////////////////////////////////
ArrayBase_::ArrayBase_( Cpl::Dm::ModelDatabase& myModelBase,
                        const char*             symbolicName,
                        void*                   myDataPtr,
                        size_t                  numElements,
                        size_t                  elementSize )
    : Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, myDataPtr, numElements * elementSize, false )
    , m_numElements( numElements )
    , m_elementSize( elementSize )
{
    // Initialize the array to all zero - so as to have deterministic 'invalid' value 
    memset( myDataPtr, 0, m_dataSize );
}


ArrayBase_::ArrayBase_( Cpl::Dm::ModelDatabase& myModelBase,
                        const char*             symbolicName,
                        void*                   myDataPtr,
                        size_t                  numElements,
                        size_t                  elementSize,
                        void*                   initialValueSrcPtr )
    : Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, myDataPtr, numElements * elementSize, true )
    , m_numElements( numElements )
    , m_elementSize( elementSize )
{
    // Initialize the array to all zero
    if ( initialValueSrcPtr == nullptr )
    {
        memset( myDataPtr, 0, m_dataSize );
    }

    // Initialize the array to the provided data
    else
    {
        memcpy( myDataPtr, initialValueSrcPtr, m_dataSize );
    }
}


/////////////////////////////////////////////////////
bool ArrayBase_::readArrayElements( void* dstData, size_t dstNumElements, size_t srcIndex, uint16_t* seqNumPtr ) const noexcept
{
    MetaData_T dst ={ (uint8_t*) dstData, dstNumElements, srcIndex };
    return readData( &dst, sizeof( dst ), seqNumPtr );
}

uint16_t ArrayBase_::writeArrayElements( const void* srcData, size_t srcNumElements, size_t dstIndex, LockRequest_T lockRequest ) noexcept
{
    MetaData_T src ={ (uint8_t*) srcData, srcNumElements, dstIndex };
    return writeData( &src, sizeof( src ), lockRequest );
}

uint16_t ArrayBase_::copyArrayFrom( const ArrayBase_& src, LockRequest_T lockRequest ) noexcept
{
    // Handle the src.invalid case
    if ( src.isNotValid() )
    {
        return setInvalid();
    }

    m_modelDatabase.lock_();
    uint16_t seqNum = ArrayBase_::writeArrayElements( src.m_dataPtr, src.m_numElements, 0, lockRequest );
    m_modelDatabase.unlock_();
    return seqNum;
}

/////////////////////////////////////////////////////
void ArrayBase_::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( MetaData_T ) );
    MetaData_T* dstInfo = (MetaData_T*) dstData;

    // Make sure we don't read past the m_data storage
    if ( dstInfo->elemIndex + dstInfo->numElements > m_numElements )
    {
        dstInfo->numElements = m_numElements - dstInfo->elemIndex;
    }

    // Copy the data to 'dst'
    uint8_t* arrayStartPtr = (uint8_t*) m_dataPtr;
    memcpy( dstInfo->elemPtr, &(arrayStartPtr[dstInfo->elemIndex * m_elementSize]), dstInfo->numElements * m_elementSize );
}

void ArrayBase_::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( MetaData_T ) );
    MetaData_T* srcInfo = (MetaData_T*) srcData;

    // Make sure we don't write past the m_data storage
    if ( srcInfo->elemIndex + srcInfo->numElements > m_numElements )
    {
        srcInfo->numElements = m_numElements - srcInfo->elemIndex;
    }

    // Copy the data to 'src'
    uint8_t* arrayStartPtr = (uint8_t*) m_dataPtr;
    memcpy( &(arrayStartPtr[srcInfo->elemIndex * m_elementSize]), srcInfo->elemPtr, srcInfo->numElements * m_elementSize );
}

bool ArrayBase_::isDataEqual_( const void* otherData ) const noexcept
{
    MetaData_T* otherInfo = (MetaData_T*) otherData;

    // Make sure we don't compare past the m_data storage
    if ( otherInfo->elemIndex + otherInfo->numElements > m_numElements )
    {
        otherInfo->numElements = m_numElements - otherInfo->elemIndex;
    }

    uint8_t* arrayStartPtr = (uint8_t*) m_dataPtr;
    return memcmp( &(arrayStartPtr[otherInfo->elemIndex * m_elementSize]), otherInfo->elemPtr, otherInfo->numElements * m_elementSize ) == 0;
}

/////////////////////////////////////////////////////
size_t ArrayBase_::getInternalDataSize_() const noexcept
{
    return getSize() + sizeof( m_numElements ) + sizeof( m_elementSize );
}

bool ArrayBase_::importMetadata_( const void* srcDataStream, size_t& bytesConsumed ) noexcept
{
    // NOTE: Use memcpy instead of the assignment operator since the alignment of 'srcDataStream' is unknown/not-guaranteed 
    uint8_t* incoming = (uint8_t*) srcDataStream;
    size_t   incomingNumElements;
    size_t   incomingElementSize;
    memcpy( &incomingNumElements, incoming, sizeof( incomingNumElements ) );
    memcpy( &incomingElementSize, incoming + sizeof( incomingNumElements ), sizeof( incomingElementSize ) );

    // Array size and element sizes MUST match!
    if ( incomingNumElements != m_numElements || incomingElementSize != m_elementSize )
    {
        return false;
    }

    // No additional actions required
    bytesConsumed = sizeof( incomingNumElements ) + sizeof( incomingElementSize );
    return true;
}

bool ArrayBase_::exportMetadata_( void* dstDataStream, size_t& bytesAdded ) const noexcept
{
    // NOTE: Use memcpy instead of the assignment operator since the alignment of 'dstDataStream' is unknown/not-guaranteed 
    uint8_t* outgoing = (uint8_t*) dstDataStream;
    memcpy( outgoing, &m_numElements, sizeof( m_numElements ) );
    memcpy( outgoing + sizeof( m_numElements ), &m_elementSize, sizeof( m_elementSize ) );
    bytesAdded = sizeof( m_numElements ) + sizeof( m_elementSize );
    return true;
}
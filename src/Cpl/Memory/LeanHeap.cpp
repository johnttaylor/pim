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

#include "LeanHeap.h"
#include "Cpl/System/Assert.h"
#include <string.h>


//
using namespace Cpl::Memory;


#define BYTES_TO_WORDS(n)		(( (n) + ( sizeof( size_t ) - 1 ) ) / sizeof( size_t ))

/////////////////////////////
LeanHeap::LeanHeap( size_t* heapMemory, size_t sizeInBytes )
    : m_ptrBase( heapMemory )
    , m_staringHeapSize( BYTES_TO_WORDS( sizeInBytes ) )
    , m_ptr( heapMemory )
    , m_wordsRemaining( BYTES_TO_WORDS( sizeInBytes ) )
{
    CPL_SYSTEM_ASSERT( m_wordsRemaining * 4 <= sizeInBytes );

    // Zero out all of memory
    memset( m_ptr, 0, sizeInBytes );
}


/////////////////////////////
void* LeanHeap::allocate( size_t numbytes )
{
    // Requested size in size_t words
    size_t numWords = BYTES_TO_WORDS( numbytes );
    if ( numWords > m_wordsRemaining )
    {
        return 0;
    }

    size_t* result    = m_ptr;
    m_wordsRemaining -= numWords;
    m_ptr            += numWords;
    return result;
}

void LeanHeap::reset() noexcept
{
    m_wordsRemaining = m_staringHeapSize;
    m_ptr            = m_ptrBase;
    memset( m_ptr, 0, m_staringHeapSize * sizeof( size_t ) );
}

uint8_t* LeanHeap::getMemoryStart( size_t& dstAllocatedLenInBytes ) noexcept
{
    dstAllocatedLenInBytes = (m_staringHeapSize - m_wordsRemaining) * sizeof( size_t );
    return (uint8_t*) m_ptrBase;
}

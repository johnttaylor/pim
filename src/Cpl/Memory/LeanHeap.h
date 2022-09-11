#ifndef Cpl_Memory_LeanHeap_h_
#define Cpl_Memory_LeanHeap_h_
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

#include "Cpl/Memory/ContiguousAllocator.h"


///
namespace Cpl {
///
namespace Memory {


/** This class manages an allocate-only-heap.  Memory allocated by this class
    cannot be freed. All of the memory is aligned to size_t boundaries.

    Note: The heap has a 'reset' method that can be used to free all of the 
          memory allocated by the instance - HOWEVER it is the responsibility
          of the Application to ensure the proper behavior when this
          method is used (i.e. that is okay to 'free everything')
 */

class LeanHeap: public ContiguousAllocator
{

public:
    /** Constructor.  The memory size of 'heapMemory' MUST be a multiple
        of sizeof(size_t).
     */
    LeanHeap( size_t* heapMemory, size_t sizeInBytes );


public:
    /// See Cpl::Memory::ContiguousAllocator
    void* allocate( size_t numbytes );

    /// See Cpl::Memory::ContiguousAllocator
    void reset() noexcept;

    /// See Cpl::Memory::ContiguousAllocator. 
    uint8_t* getMemoryStart( size_t& dstAllocatedLenInBytes ) noexcept;

private:
    /// Prevent access to the copy constructor -->LeanHeap can not be copied!
    LeanHeap( const LeanHeap& m );

    /// Prevent access to the assignment operator -->LeanHeap can not be copied!
    const LeanHeap& operator=( const LeanHeap& m );

protected:
    /// Pointer to the base of the original heap memory
    size_t*  m_ptrBase;

    /// Size (in words) of the original heap memory
    size_t   m_staringHeapSize;

    /// Pointer to the next available free byte of memory
    size_t*  m_ptr;

    /// The amount of remaining free memory in size_t words
    size_t   m_wordsRemaining;
};


};      // end namespaces
};
#endif  // end header latch

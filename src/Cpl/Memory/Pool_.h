#ifndef Cpl_Memory_Pool_x_h_
#define Cpl_Memory_Pool_x_h_
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

#include "Cpl/Memory/Allocator.h"
#include "Cpl/Container/DList.h"


///
namespace Cpl {
///
namespace Memory {


/** This private concrete class implements a Memory Allocator using a pool of
    fixed size blocks.  The implementation relies on a sub-class to allocate
    the actual memory for the blocks.
 */

class Pool_ : public Allocator
{
public:
    /// Helper class so I can put my blocks into to my standard containers
    class BlockInfo_ : public Cpl::Container::ExtendedItem
    {
    public:
        /// Constructor
        BlockInfo_() :m_blockPtr( 0 ) {}

    public:
        /// Reference to the block being stored in the list
        const char* m_blockPtr;
    };


protected:
    /// My free list of blocks
    Cpl::Container::DList<BlockInfo_>   m_freeList;

    /// My list of allocated blocks
    Cpl::Container::DList<BlockInfo_>   m_allocatedList;

    /// Block size
    size_t                              m_blockSize;

    /// Block size
    size_t                              m_alignedBlockSize;

    /// Flag that controls memory errors behavior
    bool                                m_fatalErrors;


public:
    /// Constructor.
    Pool_( BlockInfo_     infoBlocks[],
           size_t         blockSize,
           size_t         alignedBlockSize,
           size_t         numBlocks,
           void*          arrayOfBlocks,
           bool           fatalErrors
    );


    /// Destructor
    ~Pool_();


public:
    /// See Cpl::Memory::Allocator
    void* allocate( size_t numbytes );

    /// See Cpl::Memory::Allocator
    void release( void *ptr );

    /// See Cpl::Memory::Allocator
    size_t wordSize() const noexcept;

};


};      // end namespaces
};
#endif  // end header latch

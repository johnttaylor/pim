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

#include "Pool_.h"
#include "Cpl/System/FatalError.h"
#include <string.h>


//
using namespace Cpl::Memory;

#define SECT_ "Cpl::Memory"


/////////////////////////////
Pool_::Pool_( BlockInfo_ infoBlocks[], size_t blockSize, size_t alignedBlockSize, size_t numBlocks, void* arrayOfBlocks, bool fatalErrors )
	:m_blockSize( blockSize ),
	m_fatalErrors( fatalErrors )
{
	// Trap possible errors
	if ( !infoBlocks )
	{
		Cpl::System::FatalError::logf( "Cpl::Memory::Pool_::Pool_().  No memory for infoBlocks[]. Allocator=%p", this );
	}
	if ( !arrayOfBlocks )
	{
		Cpl::System::FatalError::logf( "Cpl::Memory::Pool_::Pool_().  No memory for arrayOfBlocks[]. Allocator=%p", this );
	}

	// Generate my free list
	size_t i;
	char*  blockPtr = (char*) arrayOfBlocks;
	for ( i=0; i < numBlocks; i++ )
	{
		infoBlocks[i].m_blockPtr = blockPtr;
		blockPtr                += alignedBlockSize;
		m_freeList.put( infoBlocks[i] );
	}
}


Pool_::~Pool_()
{
}

/////////////////////////////
void* Pool_::allocate( size_t numbytes )
{
	const void* ptr = 0;

	// Trap requesting more memory than the block size
	if ( numbytes > m_blockSize )
	{
		if ( m_fatalErrors )
		{
			Cpl::System::FatalError::logf( "Cpl::Memory::Pool_::allocate().  Failed allocation: Requested size (%p) > block size (%p). Allocator=%p", (void*) numbytes, (void*) m_blockSize, this );
		}
	}

	// Try to allocate memory
	else
	{
		// Get a block from my free list
		BlockInfo_* infoPtr = m_freeList.get();
		if ( infoPtr )
		{
			ptr = infoPtr->m_blockPtr;
			m_allocatedList.put( *infoPtr );
		}

		// Handle the error case
		else
		{
			if ( m_fatalErrors )
			{
				Cpl::System::FatalError::logf( "Cpl::Memory::Pool_::allocate().  Out of memory/blocks (requested size=%p). Allocator=%p", (void*) numbytes, this );
			}
		}
	}

	// Return the allocated block          
	return (void*) ptr;
}


void Pool_::release( void *ptr )
{
	// Handle the case of ptr == 0  \(per semantic of the Allocator interface)
	if ( !ptr )
	{
		return;
	}

	// Check against my allocated list
	BlockInfo_* infoPtr = m_allocatedList.first();
	while ( infoPtr )
	{
		// Found in the allocated list -->move it the free list
		if ( infoPtr->m_blockPtr == ptr )
		{
			m_allocatedList.remove( *infoPtr );
			m_freeList.put( *infoPtr );
			return;
		}

		infoPtr = m_allocatedList.next( *infoPtr );
	}

	// If I get here than a pointer that I did NOT allocated is trying to released (this is bad!)
	Cpl::System::FatalError::logf( "Cpl::Memory::Pool_::release().  Freeing a pointer (%p) that was not previously allocated. Allocator=%p", ptr, this );
}



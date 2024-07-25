#ifndef Cpl_Memory_HPool_h_
#define Cpl_Memory_HPool_h_
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


#include "Cpl/Memory/Pool_.h"
#include "Cpl/Memory/Aligned.h"

///
namespace Cpl {
///
namespace Memory {


/** This template class defines a concrete Allocator that allocates its block
	memory from the HEAP.  However, once the initial set of blocks are
	allocated, no more heap operations are performed.  All of the memory is
	aligned to size_t boundaries.

	NOTES:

		1) If you only need memory for ONE instance - use AlignedClass structure
		   in Aligned.h instead.

		2) The class is not inherently multi-thread safe.

		3) If the requested number of bytes on the allocate() method is greater
		   than the block size (i.e. sizeof(T)), 0 is returned.

		4) The class can be deleted. However, it is the responsibility of the
		   Application to properly clean-up/release ALL outstanding block
		   allocations before deleting the HPool instance.


	Template args: class "T" is the type of class to allocated
 */

template <class T>
class HPool : public Allocator
{
protected:
	/// Allocate memory for BlockInfo_ instances
	Pool_::BlockInfo_*  m_infoBlocks;

	/// Allocate blocks
	AlignedClass<T>*    m_blocks;

	/// My Pool work object
	Pool_*              m_poolPtr;


public:
	/** Constructor.  When the 'fatalErrors' argument is set to true, memory errors
		(e.g. out-of-memory) will generate a Cpl::System::FatalError call.
	 */
	HPool( size_t maxNumBlocks, bool fatalErrors = false )
		:m_infoBlocks( new Pool_::BlockInfo_[maxNumBlocks]() ),
		m_blocks( new AlignedClass<T>[maxNumBlocks] ),
		m_poolPtr( new Pool_( m_infoBlocks, sizeof( T ), sizeof( AlignedClass<T> ), maxNumBlocks, m_blocks, fatalErrors ) )
	{
	}


	/// Destructor.
	~HPool()
	{
		delete m_poolPtr;
		delete[] m_blocks;
		delete[] m_infoBlocks;
	}


public:
	/// See Cpl::Memory::Allocator
	void* allocate( size_t numbytes ) { return m_poolPtr->allocate( numbytes ); }

	/// See Cpl::Memory::Allocator
	void release( void *ptr ) { m_poolPtr->release( ptr ); }

	/// See Cpl::Memory::Allocator
	size_t wordSize() const noexcept { return m_poolPtr->wordSize(); }

private:
	/// Prevent access to the copy constructor -->HPools can not be copied!
	HPool( const HPool& m );

	/// Prevent access to the assignment operator -->HPools can not be copied!
	const HPool& operator=( const HPool& m );

};



};      // end namespaces
};
#endif  // end header latch

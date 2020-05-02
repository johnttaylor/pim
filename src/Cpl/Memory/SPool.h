#ifndef Cpl_Memory_SPool_h_
#define Cpl_Memory_SPool_h_
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


#include "Cpl/Memory/Pool_.h"
#include "Cpl/Memory/Aligned.h"

///
namespace Cpl {
///
namespace Memory {


/** This template class defines a concrete Allocator that STATICALLY allocates
	all of its Memory and can allocate up to N instances of the specified Class.
	All of the memory is aligned to size_t boundaries.

	NOTES:

		1) If you only need memory for ONE instance - use AlignedClass structure
		   in Aligned.h instead.

		2) The class is not inherently multi-thread safe.

		3) If the requested number of bytes on the allocate() method is greater
		   than the block size (i.e. sizeof(T)), 0 is returned.

		4) The class can be deleted. However, it is the responsibility of the
		   Application to properly clean-up/release ALL outstanding block
		   allocations before deleting the SPool instance.


	Template args: class "T" is the type of class to allocated
				   int   "N" is the number of instances that can be allocate
 */

template <class T, int N>
class SPool : public Allocator
{
protected:
	/// Allocate blocks
	AlignedClass<T>     m_blocks[N];

	/// Allocate memory for BlockInfo_ instances
	Pool_::BlockInfo_   m_infoBlocks[N];

	/// My Pool work object
	Pool_               m_pool;

public:
	/** Constructor.  When the 'fatalErrors' argument is set to true, memory errors
		(e.g. out-of-memory) will generate a Cpl::System::FatalError call. .
	 */
	SPool( bool fatalErrors = false )
		:m_infoBlocks(),
		m_pool( m_infoBlocks, sizeof( T ), sizeof( AlignedClass<T> ), N, m_blocks, fatalErrors )
	{
	}

public:
	/// See Cpl::Memory::Allocator
	void* allocate( size_t numbytes ) { return m_pool.allocate( numbytes ); }

	/// See Cpl::Memory::Allocator
	void release( void *ptr ) { m_pool.release( ptr ); }


private:
	/// Prevent access to the copy constructor -->SPools can not be copied!
	SPool( const SPool& m );

	/// Prevent access to the assignment operator -->SPools can not be copied!
	const SPool& operator=( const SPool& m );

};



};      // end namespaces
};
#endif  // end header latch

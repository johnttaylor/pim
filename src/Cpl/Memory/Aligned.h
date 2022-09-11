#ifndef Cpl_Memory_Aligned_h_
#define Cpl_Memory_Aligned_h_
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

#include <stdlib.h>     // for size_t


///
namespace Cpl {
///
namespace Memory {

/** This type is used to create a memory-block that is aligned to a size_t
	boundary.  This class statically allocates at least 'sizeInBytes' bytes of
	memory.
 */
template <size_t sizeInBytes>
union Aligned
{
	/** This member defines the memory in terms of an array of
		size_t words.
	 */
	size_t m_wordMem[( sizeInBytes + ( sizeof( size_t ) - 1 ) ) / sizeof( size_t )];

	/** This member defines the memory in terms of a character array.
	 */
	char   m_byteMem[sizeInBytes];
};


/** This type is used to create a memory block that is large enough to hold
	the memory footprint of ONE instance of 'class T'.  The memory block is
	aligned to a size_t boundary.
 */
template <class T>
union AlignedClass
{
	/** This member defines the memory in terms of an array of
		size_t words.
	 */
	size_t m_wordMem[( sizeof( T ) + ( sizeof( size_t ) - 1 ) ) / sizeof( size_t )];

	/** This member defines the memory in terms of a character array.
	 */
	char   m_byteMem[sizeof( T )];
};


};      // end namespaces
};
#endif  // end header latch

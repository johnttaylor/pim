#ifndef Cpl_Memory_Contigous_Allocator_h_
#define Cpl_Memory_Contigous_Allocator_h_
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

#include "Cpl/Memory/Allocator.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Memory {


/** This abstract class is used 'extend' the Memory Allocator to have contiguous
	semantics.  This means all memory that is allocated can be viewed as
	single contiguous block of memory.  In addition, the 'release' be is hidden
	and replaced with a reset() method that releases/frees ALL memory allocated.


*/
class ContiguousAllocator : public Allocator
{
public:
	/** Resets the allocator, i.e. effectively frees all allocated memory.  It 
		is the caller's RESPONSIBILTY to ensure that it is kosher to free all 
		of the memory.
	 */
	virtual void reset() noexcept = 0;
	
public:
	/** This method returns a pointer to the start of Allocator's contiguous
	    memory AND the total number of bytes allocated so far.

		NOTE: This method has GREAT POWER and it is the responsibility of the 
		      Application to use it correctly (Uncle Ben 2002).
     */ 
	virtual uint8_t* getMemoryStart( size_t& dstAllocatedLenInBytes ) noexcept= 0;

private:
	/** Hide/disable the individual release method (it does not fit the 
	    contiguous semantics).  If it gets called via the parent 'Allocator'
		interface - it does NOTHING
	 */
	void release( void *ptr ){};
};


};      // end namespaces
};
#endif  // end header latch

#ifndef Cpl_Memory_x_testsupport_New_TS_h_
#define Cpl_Memory_x_testsupport_New_TS_h_
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

#include <stddef.h>


/// 
namespace Cpl {
/// 
namespace Memory {


/** This class provides methods to access the unit testing support
	for overriding the global new/delete operators.  The main purpose is to
	be able to trigger memory allocation failures.  It also provides
	limited memory leak detection.

	NOTE: THIS INTERFACE IS **NOT** THREAD SAFE!!

 */
class New_TS
{
public:
	/** This method will cause the next call(s) to 'new' to return a
		null pointer.
	 */
	static void disable( void );

	/** This method 'undos' the disable() call, i.e. the next call(s) to
		new will succeed (assuming there is actual heap memory available).
		This is the default/startup state of the Test heap.
	 */
	static void enable( void );

	/** This method is used to set the 'expected' delta between the number
		of calls to new and delete.  The default is a delta of zero, i.e.
		a delete call for every new call.
	 */
	static void setNewDelete_delta( long delta, bool ignoreDelta=false );


public:
	/// New/Delete metrics
	struct Stats
	{
		///
		unsigned long m_numNewCalls;
		///
		unsigned long m_numNewCallsFailed;  // Failed do to being 'disabled'
		///
		unsigned long m_numDeleteCalls;
		///
		unsigned long m_numDeleteCalls_withNullPtr;
		///
		size_t        m_bytesAllocated;
	};


	/** This method returns and optionally clears the metrics collected
		with respect to Test heap. The caller is responsible for providing
		the memory for the returned metrics.
	 */
	static void getStats( Stats& stats, bool resetStats = true );

	/** This method unconditionally clears the stats
	 */
	static void clearStats();


};


};      // end namespaces
};
#endif  // end header latch


#ifndef Cpl_Checksum_Fletcher16_h_
#define Cpl_Checksum_Fletcher16_h_
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

#include "Cpl/Checksum/Api16.h"



///
namespace Cpl {
///
namespace Checksum {


/** This class provides an implementation for the 16 Bit wide
	Fletcher Checksum
 */
class Fletcher16 : public Api16
{
private:
	/// current sum
	uint8_t m_sum1;

	/// current sum
	uint8_t m_sum2;


public:
	/// Constructor
	Fletcher16() noexcept;


public:
	/// See Cpl::Checksum::Ap16
	void reset( void ) noexcept;

	/// See Cpl::Checksum::Ap16
	void accumulate( void* bytes, unsigned numbytes=1 ) noexcept;

	/// See Cpl::Checksum::Ap16
	uint16_t finalize( void* destBuffer=0 ) noexcept;

	/// See Cpl::Checksum::Ap16
	bool isOkay( void ) noexcept;
};


};      // end namespaces
};
#endif  // end header latch


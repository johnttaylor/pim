#ifndef Cpl_Checksum_Crc32EthernetFast_h_
#define Cpl_Checksum_Crc32EthernetFast_h_
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

#include "Cpl/Checksum/Api32.h"



///
namespace Cpl {
///
namespace Checksum {

/** This class provides an implementation for the 32 Bit "Ethernet"
	standard.  The CRC has following characteristics:
		o The polynomial is: x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1
		o The Data bytes are NOT reflected
		o The remainder is NOT reflected.
		o The final remainder is NOT XOR'd
 */
class Crc32EthernetFast : public Api32
{
private:
	/// Calculated CRC value
	uint32_t    m_crc;

public:
	/// Constructor
	Crc32EthernetFast() noexcept;


public:
	/// See Cpl::Checksum::Api32
	void reset( void ) noexcept;

	/// See Cpl::Checksum::Api32
	void accumulate( void* bytes, unsigned numbytes=1 ) noexcept;

	/// See Cpl::Checksum::Api32
	uint32_t finalize( void* destBuffer=0 ) noexcept;

	/// See Cpl::Checksum::Api32
	bool isOkay( void ) noexcept;
};

};      // end namespaces
};
#endif  // end header latch


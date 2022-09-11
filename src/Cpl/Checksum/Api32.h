#ifndef Cpl_Checksum_Api32_h_
#define Cpl_Checksum_Api32_h_
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

#include <stdint.h>



///
namespace Cpl {
///
namespace Checksum {


/** This class provides an interface for calculate a 32 bit wide
	Checksum.  The specific of the checksum is determine by
	the concrete class implementing this interface.

	How to generate a Checksum:
		1. Call reset() initialize the Checksum
		2. Call accumulate() method for every byte being Checksum'd
		3. Call finalize() to get the CRC value.  The method can optionally
		   appended the CRC to the buffer being Checksum'd

	How to verify a Checksum:
		1. Call reset() initialize the Checksum
		2. Call accumulate() method for every byte being Checksum'd
		   INCLUDING the previously generated Checksum bytes.
		3. Call isOk() which returns true if the buffer passes the Checksum
		   check.
 */
class Api32
{
public:
	/// Used to re-use/restart the Checksum object
	virtual void reset( void ) noexcept = 0;

	/** Call the method for every byte being Checksum'd
	 */
	virtual void accumulate( void* bytes, unsigned numbytes=1 ) noexcept = 0;

	/** Call this method to finalize the Checksum.  The calculated Checksum
		value is returned.  If 'destBuffer' is NOT null, then the
		Checksum value is appended to the buffer starting at the address
		specified by 'destBuffer'.  Note: the application is responsible
		for ensure there is sufficient space (and additional 4 bytes) for
		the appended Checksum value.
	 */
	virtual uint32_t finalize( void* destBuffer=0 ) noexcept = 0;

	/** This method returns true if the data and the incoming Checksum bytes,
		that accumulate() has been called on, is good.  Returns true if
		the Checksum check passes; else false is returned
	 */
	virtual bool isOkay( void ) noexcept = 0;


public:
	/// Virtual destructor
	~Api32() {}

};

};      // end namespaces
};
#endif  // end header latch


#ifndef Cpl_Checksum_ApiMd5_h_
#define Cpl_Checksum_ApiMd5_h_
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

#include <stdint.h>
#include "Cpl/Text/String.h"


///
namespace Cpl {
///
namespace Checksum {


/** This class provides an interface for performing a MD5 Hash on a
	collection of bytes.

	How to generate a Hash result:
		1. Call reset() initialize the Hash
		2. Call accumulate() method for every byte being hash'd
		3. Call finalize() to get the Hashed value.
 */
class ApiMd5
{
public:
	/// Number of bytes in the digest
	enum { eDIGEST_LEN=16 };

	/// Digest/result of the hash
	typedef uint8_t Digest_T[eDIGEST_LEN];


public:
	/// Used to re-use/restart the hash object
	virtual void reset( void ) noexcept = 0;

	/** Call the method for every byte being hash'd
	 */
	virtual void accumulate( const void* bytes, unsigned numbytes=1 ) noexcept = 0;

	/** Call this method to finalize the Hash.  The calculated hash
		value is returned.  If 'convertToString' is NOT null, then the
		hash value, aka the digest is converted to a 'ASCII Hex String'

		Note: The return value is only valid/in-scope UNTIL reset() is
			  called
	 */
	virtual Digest_T& finalize( Cpl::Text::String* convertToString=0, bool uppercase=true, bool append=false ) = 0;


public:
	/// Virtual destructor
	~ApiMd5() {}

};

};      // end namespaces
};
#endif  // end header latch



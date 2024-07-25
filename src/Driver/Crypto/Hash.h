#ifndef Driver_Crypto_Hash_h_
#define Driver_Crypto_Hash_h_
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

#include "Driver/Crypto/Api.h"
#include <stdint.h>
#include <stdlib.h>


///
namespace Driver {
///
namespace Crypto {


/** This class defines an abstract interface for a Hashing Algorithms

	How to generate a Hash result:
		1. Call reset() initialize the Hash
		2. Call accumulate() method for every byte being hash'd
		3. Call finalize() to get the Hashed value.
 */
class Hash
{
public:
	/// Used to re-use/restart the hash object. Returns DRIVER_CRYPTO_SUCCESS when successful.
	virtual DriverCryptoStatus_T reset( void ) noexcept = 0;

	/** Call the method for every byte being hash'd. Returns DRIVER_CRYPTO_SUCCESS when successful.
	 */
	virtual DriverCryptoStatus_T accumulate( const void* bytes, size_t numbytes=1 ) noexcept = 0;

	/** Call this method to finalize the Hash.  The calculated hash
		value is returned.  
	 */
	virtual DriverCryptoStatus_T finalize( void* dstHashDigest, size_t dstHashDigestSize ) noexcept = 0;

public:
	/// Returns the number of bytes in the digest
	virtual size_t digestSize() const noexcept = 0;


public:
	/// Virtual destructor
	~Hash() {}

};

}       // end namespaces
} 
#endif  // end header latch



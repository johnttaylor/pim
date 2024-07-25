#ifndef Driver_Crypto_Orlp_SHA512_h_
#define Driver_Crypto_Orlp_SHA512_h_
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

#include "Driver/Crypto/Hash.h"
#include "orlp/ed25519/sha512.h"
#include "Cpl/System/Assert.h"

///
namespace Driver {
///
namespace Crypto {
///
namespace Orlp {


/** This class implements the Hash interface using Orson Peter's SHA512 
	algorithm
 */
class SHA512: public Driver::Crypto::Hash
{
public:
	/// Constructor
	SHA512() { sha512_init( &m_context ); }

public:
	/// See Driver::Crypto::Hash
	DriverCryptoStatus_T reset( void ) noexcept 
	{ 
		return sha512_init( &m_context ); 
	}

	/// See Driver::Crypto::Hash
	DriverCryptoStatus_T accumulate( const void* bytes, size_t numbytes=1 ) noexcept
	{
		return sha512_update( &m_context, (const uint8_t*) bytes, numbytes );
	}

	/// See Driver::Crypto::Hash
	DriverCryptoStatus_T finalize( void* dstHashDigest, size_t dstHashDigestSize ) noexcept
	{
		CPL_SYSTEM_ASSERT( dstHashDigest >= 64 );
		return sha512_final( &m_context, (uint8_t*) dstHashDigest );
	}

public:
	/// See Driver::Crypto::Hash
	size_t digestSize() const noexcept { return 64; }

protected:
	/// Algorithm context
	sha512_context	m_context;
};

}       // end namespaces
} 
} 
#endif  // end header latch



#ifndef Cpl_Container_HashFunction_h_
#define Cpl_Container_HashFunction_h_
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


/// 
namespace Cpl {
/// 
namespace Container {

/** This class defines the interface for a hashing function that operates
    on a key stored in contiguous RAM.
 */
class HashFunction
{
public:
    /// Generates a Hash Index (0 to maxBuckets-1) based on the specified key
    virtual unsigned int hashKey( const void* keystart, unsigned keylen, unsigned int maxBuckets ) const noexcept = 0;


public:
    /// Ensure a Virtual destructor
    virtual ~HashFunction() {}

};


};      // end namespaces
};
#endif  // end header latch


#ifndef Cpl_Dm_Mp_BitArray_h_
#define Cpl_Dm_Mp_BitArray_h_
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


#include "colony_config.h"
#include "Cpl/Dm/Mp/Numeric.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/atob.h"


///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This template class provides a concrete implementation for a Point who's
    data is a a bit array of N bits.  The underlying storage of the bit array is
    16 bit unsigned integers.  A side effect of this storage
    mechanism the bit ordering in the JSON 'val' string is dependent on the
    target platform's Endian architecture.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:"<bits>" }

        where <bits> is a string of N digits ('1' or '0') where the left most digit is
        is the MSb of byte[0] and the right most digit is the LSb of byte[N].
        Whether byte[0] is the MSB or LSB is dependent on the big/little Endian
        architecture of the target platform.

        For example a 16bit Array (as binary hex: dataword[0]=0x30, dataword[1]=0x09)

            val:"0011000000001001"

        \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.

 */
class BitArray16 : public BitArray_<uint16_t, BitArray16>
{
public:
        /** Constructor. Invalid MP.
     */
    BitArray16( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : BitArray_<uint16_t, BitArray16>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    BitArray16( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, uint16_t initialValue )
        : BitArray_<uint16_t, BitArray16>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<BitArray16> Observer;

public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::BitArray16";
    }
};



};      // end namespaces
};
};
#endif  // end header latch

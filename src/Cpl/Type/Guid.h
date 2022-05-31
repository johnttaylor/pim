#ifndef Cpl_Type_Guid_h_
#define Cpl_Type_Guid_h_
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

#include "Cpl/Text/String.h"
#include <stdint.h>


/// The number of binary bytes in a GUID
#define CPL_TYPE_GUID_BINARY_LENGTH						16

/** Maximum string length (not including the null terminator) for a formatted
	GUI string WITHOUT leading/trailing brackets
 */
#define CPL_TYPE_GUID_MAX_FORMATTED_LENGTH              36

 /** Maximum string length (not including the null terminator) for a formatted
	 GUI string WITH leading/trailing brackets
  */
#define CPL_TYPE_GUID_MAX_FORMATTED_WITH_BRACES_LENGTH  38


/// 
namespace Cpl {
/// 
namespace Type {

/** This structure defines a type for a 16 Byte GUID/UUID.
	See https://en.wikipedia.org/wiki/Universally_unique_identifier for additional
	details.
 */

struct Guid_T
{
public:
    /// Value as single memory block, and/or internal structure
    union
    {
        uint8_t block[CPL_TYPE_GUID_BINARY_LENGTH];		//!< GUID value as single block of memory
        struct
        {
            uint8_t time_low[4];                        //!< Low 32 bits of the time
            uint8_t time_mid[2];                        //!< Middle 16 bits of the time
            uint8_t time_hi_version[2];                 //!< Version (4bits) and high 12 bits of time
            uint8_t clock_seq_hi_lo[2];                 //!< Variant (1-3 bits), Clock sequence (13-15 bits)
            uint8_t node[6];                            //!< Node ID (48 bits)
        };
    };


public:
    /// Comparison
    bool operator ==( const Guid_T other ) const;

public:
    /** This method converts the in binary format to a text string in
        8-4-4-4-12 format.

        The minimum memory required for the formatted output string is 36 bytes
        or 38 bytes when including braces.

        Returns true if conversion was successful; else false is returned (e.g.
        insufficient memory in formattedOutput).
     */
    bool toString( Cpl::Text::String& formattedOutput, bool withBraces=false );

    /** This method sets the GUID's value from a text representation of GUID in
        8-4-4-4-12 format.

        If the 1st character of 'stringGuid' is a leading '{', there must be a
        trailing '}'

        The 'stringGuid' should never contain any whitespace.

        Returns true if conversion was successful; else false is returned (e.g.
        incorrect/invalid formatting).
     */
    bool fromString( const char* stringGuid );

};

};      // end namespaces
};
#endif  // end header latch


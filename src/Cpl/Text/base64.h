#ifndef Cpl_Text_Base64_h
#define Cpl_Text_Base64_h
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

#include <string.h>

///
namespace Cpl {
///
namespace Text {


/** Encode a string of characters as base64 to a null terminated string.

    @param output      The output buffer for the encoding, stores the encoded string
                       the string is null terminated.
    @param outputLen   Length, in bytes, of the output buffer
    @param input       The input buffer for the encoding, stores the binary to be encoded
    @param inputLen    The length of the input buffer, in bytes

    @returns the length of the encoded string (not including the null terminator)
    @returns -1 an error occurred, e.g. size of 'output' buffer is too small to hold the result
 */
int base64Encode( char *output, int outputLen, const void *input, int inputLen );

/** Decode a base64 encoded string into bytes

    @param output      The output buffer for the decoding, stores the decoded binary
    @param outputLen   Length, in bytes, of the output buffer
    @param input       The input buffer for the decoding, stores the base64 string 
                       to be decoded
    @param inputLen:   The length of the input buffer, in bytes
    
    @returns the length, in bytes, of the decoded data. 
    @returns -1 if an error occurred, e.g. size of 'output' buffer is too small to hold the result
 */
int base64Decode( void *output, int outputLen, const char *input, int inputLen );

/** Returns the length of a base64 encoded string whose decoded form is inputLen 
    bytes long.

 	Parameters:
 		inputLen: the length of the decoded string
 	
    Return value:
 		The length of a base64 encoded string whose decoded form
 		is inputLen bytes long
 */
int base64EncodeLen( int inputLen );

/** Returns the length of the decoded form of a base64 encoded string

 	Parameters:
 		input:      the base64 encoded string to be measured
 		inputLen:   the length of the base64 encoded string

 	Return value:
 		Returns the length of the decoded form of a base64 encoded string
 */
int base64DecodeLen( const char *input, int inputLen );

};      // end namespaces
};
#endif  // end header latch


#ifndef Cpl_Text_Encoding_Base64_h_
#define Cpl_Text_Encoding_Base64_h_
/*-----------------------------------------------------------------------------
* Base64 encoding/decoding (RFC1341)
* Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
*
* This software may be distributed under the terms of the BSD license.

* This file is modification of Jouni Malinen code to remove the dynamic
* memory allocation.
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Text/String.h"



///
namespace Cpl {
///
namespace Text {
///
namespace Encoding {

/** This method encodes the specified binary data as Base64 (standard, with
    padding).  See https://datatracker.ietf.org/doc/html/rfc4648#section-4

    The caller is required to provide the memory to hold the encoded output.

    'dstSize' is ASSUMED to include space for the null terminator.

    When 'insertMIMELineFeeds' is true, the output has non-encoded line feeds
    added to the output string to restrict the output line length to 76 
    characters.

    The method returns true if the encoding succeeded; else false is returned
    (e.g. insufficient output memory)
 */
bool base64Encode( const void *binarySrc,
                   size_t      binarySrcLen,
                   char*       dstEncodedText,
                   size_t      dstSize,
                   size_t&     encodedOutputStringLen,
                   bool        insertMIMELineFeeds=false);



/** This method decodes a Base64 string to its binary representation

    The caller is required to provide the memory to hold the encoded output.

    The method returns true if the decoding succeeded; else false is returned
    (e.g. insufficient output memory)
*/
bool base64Decode( const char* encodedTextSrc,
                   size_t      encodedTextSrcLen,
                   void*       dstBinary,
                   size_t      dstSize,
                   size_t&     dstBinaryLen );

}       // end namespaces
}
}
#endif  // end header latch

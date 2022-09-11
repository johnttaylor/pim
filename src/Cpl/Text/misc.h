#ifndef Cpl_Text_misc_h_
#define Cpl_Text_misc_h_
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
/** @file

    This file contains a set of miscellaneous low-level text/string functions.

    NOTE: The application should be CAREFUL when using these method in that the
          provide little/to-none protection against memory errors/over-runs.
*/

#include <stdint.h>
#include <stdlib.h>


///
namespace Cpl {
///
namespace Text {


/** Returns the binary value for the 'ASCII HEX' character 'c'.  Returns -1
    if there is an error (e.g. 'c' is not valid 'hex' character)
 */
uint8_t unhexChar( char c );


/** This method converts up to 'numCharToScan' characters from the ASCII HEX
    text string.  The converted binary data is stored in 'outData'.

    The application is responsible for ENSURING that the 'outData' is large
    enough to hold the converted output!
 */
bool unhex( const char* inString, size_t numCharToScan, uint8_t* outData );


};      // end namespaces
};
#endif  // end header latch

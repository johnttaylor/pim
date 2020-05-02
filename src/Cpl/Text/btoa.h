#ifndef Cpl_Text_btoa_h_
#define Cpl_Text_btoa_h_
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
/** @file

	This file contains a collection of methods that convert a binary value
	to a text string. Typically snprinf() is/can-be used for this - however
	these routines explicitly DO NOT use snprintf for the conversation.  This
	is for platforms/situations where it is not possible or undesirable (i.e.
	stack usage) to call snprintf.

*/

#include <stdlib.h>


///
namespace Cpl {
///
namespace Text {


/** This method converts a long to a string. If the number of converted digits
	exceeds the size of 'dstString', then the MOST significant digit(s) are
	discarded.  When 'num' is negative there will always be a leading minus
	sign ('-') even if it means discarding a MOST significant digit.  There is
	no feedback if/when digits are discarded.

	@param num          number to convert
	@param dstString    buffer to hold the output
	@param maxChars     size, in bytes, of 'dstString'.  Note: This includes the space for the null terminator
	@param base         Number base for conversion
	@param padChar      The character to use to pad any unused leading
						characters in 'dstString'

	@returns a pointer to the beginning of the converted number when
			 successful; else 0 is returned on error (e.g. illegal 'base'
			 value).  The returned pointer is essentially a left justified
			 string of the converted value.  For a right justified result,
			 use the original pointer passed as 'dstString'.
 */
const char* longToStr( long num, char* dstString, size_t maxChars, unsigned base=10, char padChar=' ' );


/** This method is same as longToStr(), except it converts a unsigned long value
 */
const char* ulongToStr( unsigned long num, char* dstString, size_t maxChars, unsigned base=10, char padChar=' ' );


/** This method is same as longToStr(), except it converts a size_t value
 */
const char* sizetToStr( size_t num, char* dstString, size_t maxChars, unsigned base=10, char padChar=' ' );




};      // end namespaces
};
#endif  // end header latch

#ifndef Cpl_Text_atob_h_
#define Cpl_Text_atob_h_
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

	This file contains a collection of methods that wrap the standard C
	library functions for converting text/string to binary values.

*/

#include <stdlib.h>
#include "colony_map.h"
#include "Cpl/System/ElapsedTime.h"


///
namespace Cpl {
///
namespace Text {



/** This method converts the specified string to an integer.  The method returns
	true if the conversion was successful. When false is returned, the
	'convertedValue' argument is NOT updated. By default the conversion assumes
	a base 10 number and that the 'end-of-number' is end-of-string. If endptr
	is specified and the conversation was successful, a pointer to the first
	character 'after' the number is returned.
 */
bool a2i( int& convertedValue, const char* string, int base=10, const char* validStopChars=0, const char** endptr=0 );

/** This method is the same as a2i() except that it converts unsigned integer.
 */
bool a2ui( unsigned& convertedValue, const char* string, int base=10, const char* validStopChars=0, const char** endptr=0 );

/** This method is the same as a2i() except that it converts long integer.
 */
bool a2l( long& convertedValue, const char* string, int base=10, const char* validStopChars=0, const char** endptr=0 );

/** This method is the same as a2i() except that it converts unsigned long
	integer.
 */
bool a2ul( unsigned long& convertedValue, const char* string, int base=10, const char* validStopChars=0, const char** endptr=0 );


/** This method is the same as a2i() except that it converts long long
	integer.
 */
bool a2ll( long long& convertedValue, const char* string, int base=10, const char* validStopChars=0, const char** endptr=0 );

/** This method is the same as a2i() except that it converts unsigned long long
	integer.
 */
bool a2ull( unsigned long long& convertedValue, const char* string, int base=10, const char* validStopChars=0, const char** endptr=0 );


/** This method converts the specified string to an double. The method returns
	true if the conversion was successful. When false is returned, the
	'convertedValue' argument is NOT updated. By default the conversion assumes
	that the 'end-of-number' is end-of-string.  If endptr is specified and the
	method returns true, a pointer to the first character 'after' the number is
	returned.
 */
bool a2d( double& convertedValue, const char* string, const char* validStopChars=0, const char** endptr=0 );


/** This method convert the specified string to an boolean.  The method returns
	true if the conversion was successful. When false is returned, the
	'convertedValue' argument is NOT updated. If the first N characters of
	'string' match the specified boolean token - the conversion is consider
	successful (i.e. there is no required separator/terminator character for
	a boolean token). If endptr is specified and the method returns true, a
	pointer to the first character 'after' the boolean token is returned.
 */
bool a2b( bool& convertedValue, const char* string, const char* trueToken="T", const char* falseToken="F", const char** endptr=0 );


/** This method will convert an 'ASCII HEX' string to an equivalent binary
	buffer, i.e. the reverse of bufferToAsciiHex() in format.h.  If the entire
	string was not able to be converted then -1 is returned, else the number
	of converted bytes are returned.
 */
long asciiHexToBuffer( void* dstBinary, const char* srcString, size_t dstMaxLen );

/** This method will convert an 'ASCII BINARY' string to an equivalent binary
    buffer, i.e. the reverse of bufferToAsciiBinary() in format.h.  The 
    'reverse' argument when set to true will store the binary data starting
    with the last byte of 'dstBinary'. 
    
    If the number of '1' and '0' in 'srcString' is not a multiple of eight - the 
    'missing' bits will be set to zero in the binary output. 

    If the entire string was not able to be converted then -1 is returned OR if 
    there are non '1'/'0' characters in the srcString; else the number of 
    converted bits are returned.  
    
    Note: 'srcString' MUST be a null terminated string
 */
long asciiBinaryToBuffer( void* dstBinary, const char* srcString, size_t dstMaxLen, bool reverse=false );


/** This method parses a 'timestamp' with the following format: [DD ]HH:MM:SS[.sss]
    The method returns true if the parse was successful.
 */
bool parsePrecisionTimeStamp( const char* timeStampToParse, Cpl::System::ElapsedTime::Precision_T& convertedValue );


};      // end namespaces
};
#endif  // end header latch

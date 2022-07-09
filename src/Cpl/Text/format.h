#ifndef Cpl_Text_format_h_
#define Cpl_Text_format_h_
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

	This file contains some general purpose string formatting functions.
*/

#include "Cpl/Text/String.h"
#include "Cpl/System/ElapsedTime.h"
#include <stdlib.h>


///
namespace Cpl {
///
namespace Text {



/** This method will take a raw data buffer and convert it to an 'viewable'
	string.  Non-printable values will displayed as '.'.  Note: The default
	operation is to clear the destString before the conversion.  The method
	will return if buffer was successfully converted, i.e. the ENTIRE buffer
	was converted; else false is returned (this include the cases of null
	'buffer' pointer, 'len' equals zero, or not enough memory in 'destString'
	to contain the final result).
 */
bool bufferToString( const void* buffer, int len, Cpl::Text::String& destString, bool appendToString=false );


/** This method will convert a binary buffer to 'ASCII HEX', e.g.
	given the binary data of { 12, F2, 54 }, destString:= "12F254". The
	method will return true if buffer was successfully converted, i.e. the ENTIRE
	buffer was converted to a string; else false is returned (this include the
	cases of null 'buffer' pointer, 'len' equals zero, or not enough memory
	in 'destString' to contain the final result).

	Note: The default operation is to use uppercase text and to clear
		  the destString before the conversion.
 */
bool bufferToAsciiHex( const void* binaryData, int len, Cpl::Text::String& destString, bool upperCase=true, bool appendToString=false );

/** This method converts the binary buffer to a single string that is ASCII
    BINARY. The number of digits in the output string is always a multiple of
    8. The default order for traversing the 'binaryData' is to start with
    binaryData[0].
    
    The converted result is returned via 'buffer'. If the results where 
    truncated by the no enough memory in 'buffer' then false is returned; else 
    true is returned.  
    
    Examples:
        binaryData = 0x844A, reverse=false, destString = "1000010001001010" 
        binaryData = 0x844A, reverse=true,  destString = "0100101010000100"

 */
bool bufferToAsciiBinary( const void* binaryData, int len, Cpl::Text::String& destString, bool appendToString=false, bool reverse=false );

/** This method converts the binary buffer to a single string that is the
	ASCII HEX followed by 'separator', then by the binary data as 'viewable'
	text.  The converted result is returned via 'buffer'. If the results where
	truncated by the no enough memory in 'buffer' then false is returned; else
	true is returned.
 */
bool bufferToViewer( const void* binaryData, int len, Cpl::Text::String& destString, int bytesPerLine=16, const char* separator = "    ", bool upperCase=true, bool appendToString=false );



/** This method converts the binary millisecond count of 'timeStampInMsecs'
	to a string with the following format: "DD HH:MM:SS.SSS". The converted
	result is returned via 'buffer'. If the results where truncated by the no
	enough memory in 'buffer' then false is returned; else true is returned.
 */
bool formatMsecTimeStamp( Cpl::Text::String& buffer, unsigned long long timeStampInMsecs, bool encodeDay=true, bool appendToString=false );

/** This method converts the binary second count of 'timeStampInSecs'
	to a string with the following format: "DD HH:MM:SS". The converted result
	is returned via 'buffer'. If the results where truncated by the no enough
	memory in 'buffer' then false is returned; else true is returned.
 */
bool formatSecTimeStamp( Cpl::Text::String& buffer, unsigned long long timeStampInSecs, bool encodeDay=true, bool appendToString=false );

/** This method converts the Cpl::System::ElapsedTime::Precision_T value of
	time into a string with the following format: "DD HH:MM:SS.SSS". The
	converted result is returned via 'buffer'. If the results where truncated
	by the no enough memory in 'buffer' then false is returned; else true
	is returned.
 */
bool formatPrecisionTimeStamp( Cpl::Text::String& buffer, Cpl::System::ElapsedTime::Precision_T timeStamp, bool encodeDay=true, bool appendToString=false );

};      // end namespaces
};
#endif  // end header latch


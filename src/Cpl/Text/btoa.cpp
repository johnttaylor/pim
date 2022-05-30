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
/*-----------------------------------------------------------------------------
* FULL DISCLOSURE: This code is based the LTOSTR.C implementation by Jerry
* Coffin which is in the public domain.  The original code snippet can be
* found at:
*
*   https://github.com/vonj/snippets/blob/master/ltostr.c
*
*----------------------------------------------------------------------------*/


#include "btoa.h"
#include <string.h>


#include "Cpl/System/Trace.h"

#define SECT_   "_0test"


////////////////////////////////////////////
static const char* convert_( size_t num, char* dstString, size_t maxChars, unsigned base, char padChar, bool isNegative )
{
	// Error check the base argument
	if ( base < 2 || base > 36 )
	{
		return 0;
	}

	// When convert a negative value, I need to leave room for the minus sign
	size_t sign = isNegative ? 1 : 0;

	// Null terminate the string
	dstString[--maxChars] = '\0';

	// Handle  special case: original number is zero
	if ( num == 0 )
	{
		dstString[--maxChars] = '0';
	}

	// Convert the number 
	else
	{
		// Conversion is done LSB first
		while ( num != 0 && maxChars > sign )
		{
			char remainder = (char) ( num % base );
			if ( remainder <= 9 )
			{
				dstString[--maxChars] = remainder + '0';
			}
			else
			{
				dstString[--maxChars] = remainder - 10 + 'A';
			}

			num /= base;
		}
	}

	// Add the minus when needed
	if ( isNegative )
	{
		dstString[--maxChars] = '-';
	}

	// Add pad character(s) 
	if ( maxChars > 0 )
	{
		memset( dstString, padChar, maxChars );
	}


	// Return the 'left justified' string
	return dstString + maxChars;
}


//////////////////////////////////////////////////
const char* Cpl::Text::longToStr( long num, char* dstString, size_t maxChars, unsigned base, char padChar )
{
	bool negFlag = false;
	if ( num < 0 )
	{
		negFlag = true;
		num     = -num;
	}

	return convert_( num, dstString, maxChars, base, padChar, negFlag );
}

const char* Cpl::Text::ulongToStr( unsigned long num, char* dstString, size_t maxChars, unsigned base, char padChar )
{
	return convert_( num, dstString, maxChars, base, padChar, false );
}


const char* Cpl::Text::sizetToStr( size_t num, char* dstString, size_t maxChars, unsigned base, char padChar )
{
	return convert_( num, dstString, maxChars, base, padChar, false );
}


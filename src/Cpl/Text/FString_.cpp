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

#include "FString_.h"
#include "strapi.h"
#include <string.h>
#include <stdio.h>

//
using namespace Cpl::Text;

//
char FString_::m_noMemory[1] = { '\0' };


// 'ole basic min/max methods
inline static int my_min( int a, int b ) { return a < b ? a : b; }
//inline static int max(int a, int b) { return a>b? a:b;}


///////////////////////////////
FString_::FString_( const char* string, char* internalString, int maxLen )
	:String_( internalString ),
	m_internalMaxlen( maxLen )
{
	// Handle failed memory alloc from DFString
	if ( !internalString )
	{
		m_strPtr         = m_noMemory;
		m_internalMaxlen = 0;
		m_truncated      = true;
	}
	else if ( !string )      // trap the case of a NULL initializer string
	{
		internalString[0] = '\0';
	}
	else
	{
		int inStrLen = (int) strlen( string );
		m_truncated  = inStrLen <= maxLen ? false : true;
		strncpy( internalString, string, maxLen );
		internalString[maxLen] = '\0';
	}
}

FString_::FString_( char c, char* internalString, int maxLen )
	:String_( internalString ),
	m_internalMaxlen( maxLen )
{
	// Handle failed memory alloc from DFString
	if ( !internalString )
	{
		m_strPtr         = m_noMemory;
		m_internalMaxlen = 0;
		m_truncated      = true;
	}
	else if ( maxLen > 0 )
	{
		internalString[0] = c;
		internalString[1] = '\0';
	}

	// Handle the case of 'memsize' is zero
	else
	{
		internalString[0] = '\0';
	}
}

/// Constructor
FString_::FString_( int num, char* internalString, int maxLen )
	:String_( internalString ),
	m_internalMaxlen( maxLen )
{
	// Handle failed memory alloc from DFString
	if ( !internalString )
	{
		m_strPtr         = m_noMemory;
		m_internalMaxlen = 0;
		m_truncated      = true;
	}
	else
	{
		int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%d", num );
		validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
	}
}


/// Constructor
FString_::FString_( unsigned num, char* internalString, int maxLen )
	:String_( internalString ),
	m_internalMaxlen( maxLen )
{
	// Handle failed memory alloc from DFString
	if ( !internalString )
	{
		m_strPtr         = m_noMemory;
		m_internalMaxlen = 0;
		m_truncated      = true;
	}
	else
	{
		int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%u", num );
		validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
	}
}

/// Constructor
FString_::FString_( long num, char* internalString, int maxLen )
	:String_( internalString ),
	m_internalMaxlen( maxLen )
{
	// Handle failed memory alloc from DFString
	if ( !internalString )
	{
		m_strPtr         = m_noMemory;
		m_internalMaxlen = 0;
		m_truncated      = true;
	}
	else
	{
		int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%ld", num );
		validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
	}
}

/// Constructor
FString_::FString_( long long num, char* internalString, int maxLen )
	:String_( internalString ),
	m_internalMaxlen( maxLen )
{
	// Handle failed memory alloc from DFString
	if ( !internalString )
	{
		m_strPtr         = m_noMemory;
		m_internalMaxlen = 0;
		m_truncated      = true;
	}
	else
	{
		int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%lld", num );
		validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
	}
}


/// Constructor
FString_::FString_( unsigned long num, char* internalString, int maxLen )
	:String_( internalString ),
	m_internalMaxlen( maxLen )
{
	// Handle failed memory alloc from DFString
	if ( !internalString )
	{
		m_strPtr         = m_noMemory;
		m_internalMaxlen = 0;
		m_truncated      = true;
	}
	else
	{
		int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%lu", num );
		validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
	}
}

/// Constructor
FString_::FString_( unsigned long long num, char* internalString, int maxLen )
	:String_( internalString ),
	m_internalMaxlen( maxLen )
{
	// Handle failed memory alloc from DFString
	if ( !internalString )
	{
		m_strPtr         = m_noMemory;
		m_internalMaxlen = 0;
		m_truncated      = true;
	}
	else
	{
		int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%llu", num );
		validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
	}
}


///////////////////////////////
void
FString_::copyIn( const char* src, int len )
{
	// Trap null pointer for 'src' string
	if (!src)
	{
		m_truncated = true;
		m_strPtr[0] = '\0';
	}
	else
	{
		m_truncated = len <= m_internalMaxlen ? false : true;
		len = my_min(m_internalMaxlen, len);
		strncpy(m_strPtr, src, len);
		m_strPtr[len] = '\0';
	}
}

void
FString_::appendTo( const char* string, int len )
{
	// Note: Do NOTHING if null string pointer is passed
	if (string)
	{
		int   curlen = strlen(m_strPtr);
		int   avail = m_internalMaxlen - curlen;
		int   copylen = my_min(len, avail);
		m_truncated = copylen == len ? false : true;
		strncat(m_strPtr, string, copylen);
		m_strPtr[curlen + copylen] = '\0';
	}
}


void
FString_::insertAt( int insertOffset, const char* stringToInsert )
{
	// Insure offset is NOT negative
	if ( insertOffset < 0 )
	{
		insertOffset = 0;
	}

	// If insertOffset is past the '\0', then simply append stringToInsert
	int curlen    = strlen( m_strPtr );
	int insertlen = strlen( stringToInsert );
	if ( insertOffset >= curlen )
	{
		appendTo( stringToInsert, insertlen );
		return;
	}

	// Valid insertOffset
	int avail        = m_internalMaxlen - insertOffset;     // Amount left in the buffer starting with from insertOffset
	int copylen      = my_min( insertlen, avail );          // Amount of stringToInsert that can be copied in
	int remainderlen = avail - copylen;                     // Amount of room left after stringToInsert has been copied in
	int shiftlen     = curlen - insertOffset;               // Amount of characters needing to be shifted
	int movelen      = my_min( shiftlen, remainderlen );    // Amount of characters that can be SAFELY shifted
	if ( movelen > 0 )
	{
		memmove( m_strPtr + insertOffset + copylen, m_strPtr + insertOffset, movelen );
		m_strPtr[insertOffset + copylen + movelen] = '\0';
	}
	memmove( m_strPtr + insertOffset, stringToInsert, copylen );
	m_truncated = movelen != shiftlen || copylen != insertlen ? true : false;
	m_strPtr[m_internalMaxlen] = '\0';
}

int
FString_::maxLength() const
{
	return m_internalMaxlen;
}



////////////////////////////
Cpl::Text::String& FString_::operator=( int num ) { format( "%d", num ); return *this; }
Cpl::Text::String& FString_::operator=( unsigned int num ) { format( "%u", num ); return *this; }
Cpl::Text::String& FString_::operator=( long num ) { format( "%ld", num ); return *this; }
Cpl::Text::String& FString_::operator=( long long num ) { format( "%lld", num ); return *this; }
Cpl::Text::String& FString_::operator=( unsigned long num ) { format( "%lu", num ); return *this; }
Cpl::Text::String& FString_::operator=( unsigned long long num ) { format( "%llu", num ); return *this; }
Cpl::Text::String& FString_::operator +=( int num ) { formatAppend( "%d", num ); return *this; }
Cpl::Text::String& FString_::operator +=( unsigned int num ) { formatAppend( "%u", num ); return *this; }
Cpl::Text::String& FString_::operator +=( long num ) { formatAppend( "%ld", num ); return *this; }
Cpl::Text::String& FString_::operator +=( long long num ) { formatAppend( "%lld", num ); return *this; }
Cpl::Text::String& FString_::operator +=( unsigned long num ) { formatAppend( "%lu", num ); return *this; }
Cpl::Text::String& FString_::operator +=( unsigned long long num ) { formatAppend( "%llu", num ); return *this; }


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

#include "String_.h"
#include "strip.h"
#include "strapi.h"
#include <string.h>
#include <stdio.h>

//
using namespace Cpl::Text;


///////////////////////////////
String_::String_( char* internalString )
	:m_truncated( false ),
	m_strPtr( internalString )
{
}

///////////////////////////////
void
String_::clear()
{
	m_strPtr[0] = '\0';
	m_truncated = false;
}

String_::operator char() const
{
	return m_strPtr[0];
}

int
String_::length() const
{
	return strlen( m_strPtr );
}

bool
String_::isEmpty() const
{
	return *( m_strPtr ) == '\0';
}

bool String_::truncated() const
{
	return m_truncated;
}

bool
String_::operator ==( const char* string ) const
{
	return string ? strcmp( m_strPtr, string ) == 0 : false;
}

bool
String_::operator ==( const String& string ) const
{
	return strcmp( m_strPtr, string.getString() ) == 0;
}

bool
String_::operator !=( const char* string ) const
{
	return string ? strcmp( m_strPtr, string ) != 0 : true;
}

bool
String_::operator !=( const String& string ) const
{
	return strcmp( m_strPtr, string.getString() ) != 0;
}

int String_::compare( const char* string ) const
{
	return string ? strcmp( m_strPtr, string ) : -1;
}

int String_::compareIgnoreCase( const char* string ) const
{
	return string ? strcasecmp( m_strPtr, string ) : -1;
}


bool
String_::isEqualIgnoreCase( const char* string ) const
{
	return string ? strcasecmp( m_strPtr, string ) == 0 : false;
}

bool
String_::isEqualSubstring( int startpos, int endpos, const char* string ) const
{
	if ( !string )
	{
		return false;
	}

	int len = strlen( m_strPtr );
	if ( startpos < 0 )
	{
		startpos = 0;
	}
	if ( startpos >= len )
	{
		startpos = len - 1;
	}
	if ( endpos >= len )
	{
		endpos   = len - 1;
	}
	if ( endpos < startpos )
	{
		endpos   = startpos;
	}
	return strncmp( m_strPtr + startpos, string, endpos - startpos + 1 ) == 0;
}

bool
String_::isEqualSubstringIgnoreCase( int startpos, int endpos, const char* string ) const
{
	if ( !string )
	{
		return false;
	}

	int len = strlen( m_strPtr );
	if ( startpos < 0 )
	{
		startpos = 0;
	}
	if ( startpos >= len )
	{
		startpos = len - 1;
	}
	if ( endpos >= len )
	{
		endpos   = len - 1;
	}
	if ( endpos < startpos )
	{
		endpos   = startpos;
	}

	return strncasecmp( m_strPtr + startpos, string, endpos - startpos + 1 ) == 0;
}

bool
String_::operator ==( const char c ) const
{
	return c == m_strPtr[0];
}

bool
String_::startsWith( const char* string ) const
{
	return String_::startsWith( string, 0 );
}

bool
String_::startsWith( const char* string, int startOffset ) const
{
	if ( !string )
	{
		return false;
	}

	int len  = strlen( m_strPtr );
	int len2 = strlen( string );
	if ( startOffset < 0 || ( startOffset + len2 ) > len || len2 == 0 )
	{
		return false;
	}

	return strncmp( m_strPtr + startOffset, string, len2 ) == 0;
}

bool
String_::endsWith( const char* string ) const
{
	if ( !string )
	{
		return false;
	}

	int len  = strlen( m_strPtr );
	int len2 = strlen( string );
	if ( len2 > len || len2 == 0 )
	{
		return false;
	}

	return strncmp( m_strPtr + len - len2, string, len2 ) == 0;
}


void
String_::format( const char* format, ... )
{
	if ( !format )
	{
		return;
	}

	va_list ap;
	va_start( ap, format );
	vformat( format, ap );
	va_end( ap );
}

void
String_::formatAppend( const char* format, ... )
{
	if ( !format )
	{
		return;
	}

	va_list ap;
	va_start( ap, format );
	vformatAppend( format, ap );
	va_end( ap );
}

void
String_::formatOpt( bool appendFlag, const char* format, ... )
{
	if ( !format )
	{
		return;
	}

	va_list ap;
	va_start( ap, format );
	if ( appendFlag )
	{
		vformatAppend( format, ap );
	}
	else
	{
		vformat( format, ap );
	}

	va_end( ap );
}

void
String_::vformat( const char* format, va_list ap )
{
	if ( !format )
	{
		return;
	}

	int flen = vsnprintf( m_strPtr, maxLength() + 1, format, ap );
	validateSizeAfterFormat( maxLength(), flen, maxLength() );
}

void
String_::vformatAppend( const char* format, va_list ap )
{
	if ( !format )
	{
		return;
	}

	int   len   = strlen( m_strPtr );
	int   avail = maxLength() - len;
	char* ptr   = m_strPtr + len;
	int   flen  = vsnprintf( ptr, avail + 1, format, ap );
	validateSizeAfterFormat( avail, flen, maxLength() );
}

void
String_::vformatOpt( bool appendFlag, const char* format, va_list ap )
{
	if ( !format )
	{
		return;
	}

	if ( appendFlag )
	{
		vformatAppend( format, ap );
	}
	else
	{
		vformat( format, ap );
	}
}


const char*
String_::toUpper()
{
	strupr( m_strPtr );
	return m_strPtr;
}

const char*
String_::toLower()
{
	strlwr( m_strPtr );
	return m_strPtr;
}

void
String_::removeLeadingSpaces()
{
	m_truncated   = false;
	const char* p = Cpl::Text::stripSpace( m_strPtr );

	// do nothing if there is no leading spaces
	if ( p == m_strPtr )
	{
		return;
	}

	// else re-adjust the string to remove the spaces
	size_t i = 0;
	for ( ; i < strlen( p ); i++ )
	{
		m_strPtr[i] = p[i];
	}

	m_strPtr[i] = '\0';
}

void
String_::removeTrailingSpaces()
{
	m_truncated = false;
	Cpl::Text::removeTrailingSpace( m_strPtr );
}

void
String_::removeLeadingChars( const char* charsSet )
{
	m_truncated   = false;
	const char* p = Cpl::Text::stripChars( m_strPtr, charsSet );

	// do nothing if there is no leading chars
	if ( p == m_strPtr )
	{
		return;
	}

	// else re-adjust the string to remove the chars
	size_t i = 0;
	for ( ; i < strlen( p ); i++ )
	{
		m_strPtr[i] = p[i];
	}

	m_strPtr[i] = '\0';
}

void
String_::removeTrailingChars( const char* charsSet )
{
	m_truncated = false;
	Cpl::Text::removeTrailingChars( m_strPtr, charsSet );
}

int
String_::indexOf( char c ) const
{
	return String_::indexOf( c, 0 );
}

int
String_::indexOf( char c, int startpos ) const
{
	int len = strlen( m_strPtr );
	if ( startpos < 0 || startpos >= len )
	{
		return -1;
	}

	char* ptr = strchr( m_strPtr + startpos, c );
	if ( ptr )
	{
		return ptr - m_strPtr;
	}

	return -1;
}

int
String_::indexOf( const char* str ) const
{
	return String_::indexOf( str, 0 );
}

int
String_::indexOf( const char* str, int startpos ) const
{
	int len = strlen( m_strPtr );
	if ( startpos < 0 || startpos >= len || !str )
	{
		return -1;
	}

	char* ptr = strstr( m_strPtr + startpos, str );
	if ( ptr )
	{
		return ptr - m_strPtr;
	}

	return -1;
}
int
String_::lastIndexOf( char c ) const
{
	char* ptr = strrchr( m_strPtr, c );
	if ( ptr )
	{
		return ptr - m_strPtr;
	}

	return -1;
}

int
String_::count( char c ) const
{
	int   count = 0;
	char* ptr   = m_strPtr;
	while ( ( ptr=strchr( ptr, c ) ) )
	{
		count++;
		ptr++;
	}

	return count;
}

void
String_::cut( int startpos, int endpos )
{
	m_truncated = false;
	int len     = strlen( m_strPtr );
	if ( startpos < 0 || startpos > endpos || startpos >= len || endpos >= len )
	{
		return;
	}

	if ( endpos + 1 >= len )
	{
		trimRight( len - startpos );
		return;
	}
	int movelen = len - ( endpos + 1 );
	memmove( m_strPtr + startpos, m_strPtr + endpos + 1, movelen );
	m_strPtr[startpos + movelen] = '\0';
}

void
String_::trimRight( int n )
{
	m_truncated = false;
	int len     = strlen( m_strPtr );
	int index   = len - n;
	if ( index < 0 || n < 0 )
	{
		return; // Do nothing -->error case: 'n' is greater than the length of the string OR 'n' is negative
	}
	else
	{
		m_strPtr[index] = '\0';
	}

}

void String_::setChar( int atPosition, char newchar )
{
	m_truncated = false;
	int len     = strlen( m_strPtr );
	if ( atPosition < 0 || atPosition >= len )
	{
		return;
	}

	m_strPtr[atPosition] = newchar;
}

int
String_::replace( char targetChar, char newChar )
{
	int replaceCount = 0;
	if ( targetChar != newChar )
	{
		char* ptr 	     = m_strPtr;
		while ( *ptr )
		{
			if ( *ptr == targetChar )
			{
				*ptr = newChar;
				replaceCount++;
			}
			ptr++;
		}
	}

	m_truncated = false;
	return replaceCount;
}

const char*
String_::getString() const
{
	return m_strPtr;
}

char *
String_::getBuffer( int& maxAllowedLength )
{
	m_truncated      = false;
	maxAllowedLength = maxLength();
	return m_strPtr;
}


////////////////////////////////////
int String_::compareKey( const Key& key ) const
{
	unsigned    otherLen = 0;
	const char* otherPtr = (const char*) key.getRawKey( &otherLen );
	return Cpl::Container::KeyStringBuffer::compare( m_strPtr, strlen( m_strPtr ), otherPtr, otherLen );
}

const void* String_::getRawKey( unsigned* returnRawKeyLenPtr ) const
{
	if ( returnRawKeyLenPtr )
	{
		*returnRawKeyLenPtr = strlen( m_strPtr );
	}

	return m_strPtr;
}


////////////////////////////////////
void String_::validateSizeAfterFormat( int availableLen, int formattedLen, int maxlen )
{
	if ( formattedLen <= availableLen && formattedLen >= 0 )
	{
		m_truncated = false;
	}
	else
	{
		m_truncated      = true;
		m_strPtr[maxlen] = '\0';    // ENSURE the string is terminated
	}
}

////////////////////////////////////
Cpl::Text::String& String_::operator=( const String& string ) { copyIn( string, string.length() ); return *this; }
Cpl::Text::String& String_::operator=( const char* string ) { copyIn( string, string ? strlen( string ) : 0 ); return *this; }
Cpl::Text::String& String_::operator=( char c ) { copyIn( &c, 1 ); return *this; }
Cpl::Text::String& String_::operator +=( const String& string ) { appendTo( string, string.length() ); return *this; }
Cpl::Text::String& String_::operator +=( const char* string ) { appendTo( string, string ? strlen( string ) : 0 ); return *this; }
Cpl::Text::String& String_::operator +=( char c ) { appendTo( &c, 1 ); return *this; }

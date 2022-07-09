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

#include "strip.h"
#include <string.h>
#include <ctype.h>


////////////////
bool Cpl::Text::isCharInString( const char* charsSet, const char c )
{
	for ( size_t i=0; i < strlen( charsSet ); i++ )
	{
		if ( c == charsSet[i] )
		{
			return true;
		}
	}
	return false;
}


////////////////
const char* Cpl::Text::stripSpace( const char *s )
{
	if ( !s )
	{
		return 0;
	}

	while ( *s && isspace( (int) *s ) )
	{
		s++;
	}
	return s;
}


const char* Cpl::Text::stripNotSpace( const char *s )
{
	if ( !s )
	{
		return 0;
	}

	while ( *s && !( isspace( (int) *s ) ) )
	{
		s++;
	}
	return s;
}


const char* Cpl::Text::stripTrailingSpace( const char *s )
{
	if ( !s )
	{
		return 0;
	}

	// do nothing if empty string
	size_t len = strlen( s );
	if ( !len )
	{
		return s;
	}

	// Walk the string right to left
	const char* p = s + len - 1;
	while ( p != s && isspace( (int) *p ) )
	{
		p--;
	}
	return p;
}


void Cpl::Text::removeTrailingSpace( char* s )
{
	if ( !s )
	{
		return;
	}

	char* ptr = (char*) Cpl::Text::stripTrailingSpace( s );
	if ( isspace( (int) *ptr ) )
	{
		*ptr = '\0';
	}
	else
	{
		*( ptr + 1 ) = '\0';
	}
}


//////////////////////	
const char* Cpl::Text::stripChars( const char *s, const char* charsSet )
{
	if ( !s )
	{
		return 0;
	}

	if ( !charsSet )
	{
		return s;
	}

	while ( *s && isCharInString( charsSet, *s ) )
	{
		s++;
	}
	return s;
}


const char* Cpl::Text::stripNotChars( const char* s, const char* charsSet )
{
	if ( !s )
	{
		return 0;
	}

	if ( !charsSet )
	{
		return s;
	}

	while ( *s && !( isCharInString( charsSet, *s ) ) )
	{
		s++;
	}
	return s;
}


const char* Cpl::Text::stripTrailingChars( const char *s, const char* charsSet )
{
	if ( !s )
	{
		return 0;
	}

	if ( !charsSet )
	{
		return s;
	}

	// do nothing if empty string
	size_t len = strlen( s );
	if ( !len )
	{
		return s;
	}

	// Walk the string right to left
	const char* p = s + len - 1;
	while ( p != s && isCharInString( charsSet, *p ) )
	{
		p--;
	}
	return p;
}


void Cpl::Text::removeTrailingChars( char* s, const char* charsSet )
{
	if ( !s || !charsSet )
	{
		return;
	}

	char* ptr = (char*) Cpl::Text::stripTrailingChars( (const char*) s, charsSet );
	if ( isCharInString( charsSet, *ptr ) )
	{
		*ptr = '\0';        // trap the case of string is ALL spaces
	}
	else
	{
		*( ptr + 1 ) = '\0';
	}
}

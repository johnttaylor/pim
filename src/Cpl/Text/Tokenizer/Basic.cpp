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

#include "Basic.h"
#include "Cpl/Text/strip.h"

//
using namespace Cpl::Text;
using namespace Cpl::Text::Tokenizer;


///////////////////////////////
Basic::Basic( char* stringToParse )
	:m_ptr( 0 ),
	m_delimiters( whiteSpace() ),
	m_base( 0 ),
	m_count( 0 )
{
	m_base = m_ptr = stringToParse;
}


Basic::Basic( char* stringToParse, const char* delimiterSet )
	:m_ptr( 0 ),
	m_delimiters( delimiterSet ),
	m_base( 0 ),
	m_count( 0 )
{
	m_base = m_ptr = stringToParse;
}


///////////////////////////////
const char* Basic::next() noexcept
{
	// Trap error: null pointer for input string
	if ( !m_base )
	{
		return 0;
	}

	char* startptr = (char*) stripChars( m_ptr, m_delimiters );

	if ( *startptr != '\0' )
	{
		m_ptr = (char*) stripNotChars( startptr, m_delimiters );
		if ( *m_ptr != '\0' )
		{
			*m_ptr++ = '\0';
		}

		m_count++;
		return startptr;
	}

	return 0;
}


///////////////////////////////
const char* Basic::getToken( unsigned n ) const noexcept
{
	// Trap out-of-bounds index
	if ( n >= m_count || !m_base )
	{
		return 0;
	}

	// Traverse string for the Nth token
	const char* token = stripChars( m_base, m_delimiters );
	unsigned    i;
	for ( i=0; i < n; i++ )
	{
		while ( *token != '\0' )
		{
			token++;
		}
		token = stripChars( ++token, m_delimiters );
	}

	return token;
}

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

#include "TextBlock.h"
#include "Cpl/Text/strip.h"
#include <string.h>
#include <ctype.h>

//
using namespace Cpl::Text;
using namespace Cpl::Text::Tokenizer;


static void removeCharacter_( char* charToRemovePtr, size_t numCharsToRemove=1 ) noexcept;


///////////////////////////////
TextBlock::TextBlock( char* string, char delimiter, char terminator, char quote, char escape )
	:m_base( (char*) stripSpace( string ) )
	, m_ptr( m_base )
	, m_validTokens( true )
	, m_terminatorFound( false )
	, m_count( 0 )
{
	// Flag an error if 'string' is null
	if ( !string )
	{
		m_validTokens = false;
		return;
	}

	// Do nothing (aka zero parameter count) if the string is empty AFTER removing whitespace
	if ( *m_ptr == '\0' )
	{
		return;
	}

	// Housekeeping
	char* firstNonSpacePtr = 0;
	char* lastNonSpacePtr  = 0;
	char* startOfTokenPtr  = m_ptr;

	// Parse the string
	while ( *m_ptr != '\0' )
	{
		// Check for terminator character
		if ( *m_ptr == terminator )
		{
			// Remove leading/trailing whitespace
			removeWhiteSpace( startOfTokenPtr, firstNonSpacePtr, lastNonSpacePtr );

			// Finalize the found token and STOP parsing
			m_terminatorFound = true;
			*m_ptr++          = '\0';
			m_count++;
			return;
		}

		// Check for delimiter
		if ( *m_ptr == delimiter )
		{
			// Remove leading/trailing whitespace
			removeWhiteSpace( startOfTokenPtr, firstNonSpacePtr, lastNonSpacePtr );

			// Finalize the found token
			*m_ptr++ = '\0';
			m_count++;

			// SPECIAL CASE when the delimiter is a space -->need to strip any leading spaces or 
			// each 'extra' space would be an empty parameter field
			if ( delimiter == ' ' )
			{
				char* ptr = m_ptr;
				while ( *ptr != '\0' && isspace( (int) *ptr ) )
				{
					ptr++;
				}

				// Adjust token count when removing trailing whitespace at the last token (i.e. the rule: delimiter+EOS equals an empty token does NOT apply in this case)
				if ( *ptr == '\0' || *ptr == terminator )
				{
					m_count--;
				}

				// Remove trailing/leading whitespace
				removeCharacter_( m_ptr, ptr - m_ptr );
			}

			// Housekeeping
			firstNonSpacePtr = 0;
			lastNonSpacePtr  = 0;
			startOfTokenPtr  = m_ptr;
		}


		// Start of Text String
		else if ( *m_ptr == quote )
		{
			// Remove the starting quote character
			removeCharacter_( m_ptr );
			m_validTokens  = false;
			bool escaping  = false;
			if ( !firstNonSpacePtr )
			{
				firstNonSpacePtr = m_ptr;
			}

			// consume the text string
			while ( *m_ptr != '\0' )
			{
				// process the escaped character
				if ( escaping )
				{
					escaping = false;
					m_ptr++;
				}

				// Exit the text string
				else if ( *m_ptr == quote )
				{
					// Remove the ending quote character
					removeCharacter_( m_ptr );
					lastNonSpacePtr = m_ptr - 1;
					m_validTokens   = true;
					break;
				}

				// trap escape sequence
				else if ( *m_ptr == escape )
				{
					escaping = true;
					removeCharacter_( m_ptr );
				}

				// Normal character
				else
				{
					m_ptr++;
				}
			}
		}

		// Normal parameter character
		else
		{
			// Mark leading whitespace
			if ( !firstNonSpacePtr )
			{
				if ( !isspace( (int) *m_ptr ) )
				{
					firstNonSpacePtr = m_ptr;
					lastNonSpacePtr  = m_ptr;
				}
			}

			// Mark trailing whitespace
			else if ( lastNonSpacePtr )
			{
				if ( !isspace( (int) *m_ptr ) )
				{
					lastNonSpacePtr = m_ptr;
				}
			}

			// move on to the next character
			m_ptr++;
		}
	}

	// If I get here, the Text block did NOT have a terminator (which is okay).
	// However, I need to the count the last token since the counting scheme
	// above relies on finding the next delimiter/terminator to increment the
	// token count.
	m_count++;

	// Remove leading/trailing whitespace on the last token
	removeWhiteSpace( startOfTokenPtr, firstNonSpacePtr, lastNonSpacePtr );
}


///////////////////////////////
const char* TextBlock::getParameter( unsigned index ) const noexcept
{
	// Trap out-of-bounds index
	if ( !m_validTokens || index >= m_count )
	{
		return 0;
	}

	const char* token = m_base;
	unsigned    i;
	for ( i=0; i < index; i++ )
	{
		while ( *token != '\0' )
		{
			token++;
		}
		token++;
	}

	return token;
}


void TextBlock::removeWhiteSpace( char* startOfTokenPtr, char* firstNonSpacePtr, char* lastNonSpacePtr ) noexcept
{
	size_t numSpaces = 0;

	// Remove any leading whitespace
	if ( firstNonSpacePtr )
	{
		numSpaces = firstNonSpacePtr - startOfTokenPtr;
		removeCharacter_( startOfTokenPtr, numSpaces );
		m_ptr -= numSpaces;
	}

	// NO non-whitespace was found
	else
	{
		numSpaces = m_ptr - startOfTokenPtr;
		removeCharacter_( startOfTokenPtr, numSpaces );
		m_ptr -= numSpaces;
	}

	// Remove any trailing whitespace
	if ( lastNonSpacePtr )
	{
		// Adjust for any removed spaces from leading whitespace
		lastNonSpacePtr -= numSpaces;

		// Advance my pointer to the first (if there is any) trailing whitespace char AND determine if there is any whitespace to remove
		if ( ++lastNonSpacePtr < m_ptr )
		{
			size_t numSpaces = m_ptr - lastNonSpacePtr;
			removeCharacter_( lastNonSpacePtr, numSpaces );
			m_ptr -= numSpaces;
		}
	}
}

///////////////////////////////
void removeCharacter_( char* charToRemovePtr, size_t numCharsToRemove ) noexcept
{
	if ( numCharsToRemove == 0 )
	{
		return;
	}

	char*  nextChar = charToRemovePtr + numCharsToRemove;
	size_t len      = strlen( nextChar ) + 1;

	// the move will include the null terminator
	memmove( charToRemovePtr, nextChar, len );
}



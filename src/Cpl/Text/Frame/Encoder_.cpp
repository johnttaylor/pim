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


#include "Encoder_.h"
#include "Cpl/Io/NewLine.h"
#include "Cpl/System/FatalError.h"
#include <string.h>


///
using namespace Cpl::Text::Frame;



///////////////////////////////////
Encoder_::Encoder_( char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline )
	:m_inFrame( false )
	, m_sof( startOfFrame )
	, m_eof( endOfFrame )
	, m_esc( escapeChar )
	, m_appendNewline( appendNewline )
{
}


///////////////////////////////////
bool Encoder_::startFrame() noexcept
{
	if ( m_inFrame )
	{
		Cpl::System::FatalError::logf( "Cpl::Text::Frame::Encoder_::startFrame() - Protocol Error." );
	}
	else
	{
		m_inFrame = true;
		if ( m_sof != '\0' )
		{
			return start( m_sof );
		}
		else
		{
			return start();
		}
	}

	return false;
}

bool Encoder_::output( char src ) noexcept
{
	if ( !m_inFrame )
	{
		Cpl::System::FatalError::logf( "Cpl::Text::Frame::Encoder_::output - Protocol Error." );
	}
	else
	{
		if ( m_esc != '\0' && ( src == m_esc || src == m_eof ) )
		{
			bool result = append( m_esc );
			result     &= append( src );
			return result;
		}
		else
		{
			return append( src );
		}
	}

	return false;
}


bool Encoder_::output( const char* src ) noexcept
{
	// Do NOTHING if invalid input
	if ( !src )
	{
		return true;
	}

	return output( src, strlen( src ) );
}


bool Encoder_::output( const char* src, size_t numBytes ) noexcept
{
	// Do NOTHING if invalid input
	if ( !src )
	{
		return true;
	}

	size_t i;
	for ( i=0; i < numBytes; i++ )
	{
		if ( !output( *src++ ) )
		{
			return false;
		}
	}

	return true;
}

bool Encoder_::endFrame() noexcept
{
	if ( !m_inFrame )
	{
		Cpl::System::FatalError::logf( "Cpl::Text::Frame::Encoder_::endFrame() - Protocol Error." );
	}
	else
	{
		m_inFrame = false;

		if ( append( m_eof ) )
		{
			if ( !m_appendNewline )
			{
				return true;
			}

			// Append Newline (makes it easier to read raw output)
			else
			{
				// Temporarily set the 'inframe' flag back to true to use my output method to write a string 
				m_inFrame   = true;
				bool result = output( Cpl::Io::NewLine::standard() );
				m_inFrame   = false;
				return result;
			}
		}
	}

	return false;
}

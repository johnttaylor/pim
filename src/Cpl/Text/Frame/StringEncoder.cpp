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


#include "StringEncoder.h"


///
using namespace Cpl::Text::Frame;


///////////////////////////////////
StringEncoder::StringEncoder( Cpl::Text::String& dst, char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline, bool appendToDst )
	:Encoder_( startOfFrame, endOfFrame, escapeChar, appendNewline )
	, m_dst( dst )
	, m_append( appendToDst )
{
}



///////////////////////////////////
bool StringEncoder::start( char src ) noexcept
{
	m_dst.formatOpt( m_append, "%c", src );
	return m_dst.truncated() == false;
}

bool StringEncoder::start() noexcept
{
	if ( !m_append )
	{
		m_dst.clear();
	}

	return m_dst.truncated() == false;
}

bool StringEncoder::append( char src ) noexcept
{
	m_dst += src;
	return m_dst.truncated() == false;
}


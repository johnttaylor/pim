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


#include "StreamEncoder.h"


///
using namespace Cpl::Text::Frame;



///////////////////////////////////
StreamEncoder::StreamEncoder( Cpl::Io::Output* dstPtr, char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline )
	:Encoder_( startOfFrame, endOfFrame, escapeChar, appendNewline )
	, m_dstPtr( dstPtr )
{
}


///////////////////////////////////
void StreamEncoder::setOutput( Cpl::Io::Output& newOutfd ) noexcept
{
	m_dstPtr = &newOutfd;
}


///////////////////////////////////
bool StreamEncoder::start( char src ) noexcept
{
	if ( !m_dstPtr )
	{
		return false;
	}

	return m_dstPtr->write( src );
}

bool StreamEncoder::start() noexcept
{
	// Nothing to do for stream output
	return true;
}


bool StreamEncoder::append( char src ) noexcept
{
	if ( !m_dstPtr )
	{
		return false;
	}

	return m_dstPtr->write( src );
}

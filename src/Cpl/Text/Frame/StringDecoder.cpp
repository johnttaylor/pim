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


#include "StringDecoder.h"
#include <string.h>


///
using namespace Cpl::Text::Frame;



////////////////////////////////////
StringDecoder::StringDecoder( char startOfFrame, char endOfFrame, char escapeChar, const char* inputSourceAsNullTerminatedString )
	:Decoder_( 0, 1 )	// There no actual parent buffer is provided (i.e. use the actual string storage). but I need to set the buffer size to 1 so the that read() semantics are honored
	, m_startPtr( 0 )
	, m_endPtr( 0 )
	, m_srcPtr( inputSourceAsNullTerminatedString )
	, m_srcLen( inputSourceAsNullTerminatedString ? strlen( inputSourceAsNullTerminatedString ) : 0 )
	, m_sof( startOfFrame )
	, m_eof( endOfFrame )
	, m_esc( escapeChar )
{
}



////////////////////////////////////
bool StringDecoder::scan( size_t maxSizeOfFrame, char* frame, size_t& frameSize ) noexcept
{
	bool result = Decoder_::scan( maxSizeOfFrame, frame, frameSize );
	m_endPtr    = m_dataPtr;
	m_startPtr  = m_endPtr;
	return result;
}


const char* StringDecoder::getRemainder() const noexcept
{
	return m_endPtr;
}


////////////////////////////////////
void StringDecoder::setInput( const char* inputSourceAsNullTerminatedString ) noexcept
{
	m_srcPtr = inputSourceAsNullTerminatedString;
	m_srcLen = inputSourceAsNullTerminatedString ? strlen( inputSourceAsNullTerminatedString ) : 0;

}


void StringDecoder::setInput( const char* inputSoruce, int sizeInBytesOfSource ) noexcept
{
	m_srcPtr = inputSoruce;
	m_srcLen = sizeInBytesOfSource;
}



////////////////////////////////////////////
bool StringDecoder::isStartOfFrame() noexcept
{
	return *m_dataPtr == m_sof;
}

bool StringDecoder::isEofOfFrame() noexcept
{
	return *m_dataPtr == m_eof;
}

bool StringDecoder::isEscapeChar() noexcept
{
	return *m_dataPtr == m_esc;
}

bool StringDecoder::isLegalCharacter() noexcept
{
	return *m_dataPtr != '\0';
}


////////////////////////////////////////////
bool StringDecoder::read( void* buffer, int numBytes, int& bytesRead )
{
	// Trap missing input source
	if ( m_srcPtr == nullptr )
	{
		Cpl::System::FatalError::logf( "Cpl::Text::Frame::StringDecoder::read() - Input source has NOT be set!" );
		return false;  // Should never get here! -->but needed for unittests
	}

	// Return end-of-input if already been decoded/read
	if ( m_srcLen < 0 )
	{
		bytesRead = 0;
		return false;
	}

	// Directly update the parent's data member because I don't want to do a copy (and I didn't provide the parent class a 'work buffer' anyway)
	m_buffer = (char*) m_srcPtr;

	// Determine how many bytes actually get 'consumed'
	if ( numBytes > m_srcLen )
	{
		bytesRead  = m_srcLen;
		m_srcLen   = -1;			// Mark the input string as 'decoded', i.e. a subsequent call to read() will return end-of-input
		m_startPtr = m_srcPtr;		// Handle the 'remainder' case when starting with a 'fresh' string
	}
	else
	{
		bytesRead  = numBytes;
		m_srcLen  -= numBytes;
		m_srcPtr  += numBytes;
	}
	return true;
}

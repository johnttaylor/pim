#ifndef Cpl_Text_Frame_LineDecoder_h_
#define Cpl_Text_Frame_LineDecoder_h_
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
/** @file */


#include "Cpl/Text/Frame/StreamDecoder.h"


///
namespace Cpl {
///
namespace Text {
///
namespace Frame {



/** This concrete template class provide a basic implementation of the
	Frame::Decoder interface that uses Cpl::Io::Input stream as the input
	source.  In addition this class only accepts PRINTABLE ASCII characters
	within the frame; AND the start-of-frame character is ANY printable ASCII;
	AND the end-of-frame character is a newline character (`\r` or `\n`); AND
	there is NO escape sequence since the escape sequence is need to embedded
	EOF characters - but in this case EOF (newline) is NOT a printable ASCII
	character -->so an escape sequence is not meaningful.

	NOTE: If a non-printable ASCII character is encounter within a frame, the
		  current frame is aborted AND a newline character is required before
		  accepting/detecting a new SOF character(s).


	Template args:
		BUFSIZE     Size of the internal buffer to use when reading raw
					characters from the Input stream.
 */
template <int BUFSIZE>
class LineDecoder : public StreamDecoder
{
protected:
	/// Track if I have encountered an invalid character in the 'middle' of a frame
	bool  m_illegal;

	/// Remember my tabs option
	char m_convertTabs;


	/// Raw input buffer for reading characters in 'chunks' from my Input stream (i.e. minimize the calls to read())
	char  m_buffer[BUFSIZE];


public:
	/** Constructor. If the 'convertTabs' argument is set to a value OTHER
		than a tab character, then any tab characters encounter will be
		converted to the value of 'convertTabs'.
	 */
	LineDecoder( Cpl::Io::Input* inputSource=0, char convertTabs = '\t', bool blocking = true )
		:StreamDecoder( m_buffer, BUFSIZE, inputSource, blocking )
		, m_illegal( false )
		, m_convertTabs( convertTabs )
	{
	}


protected:
	/// See Cpl::Text::Frame::Decoder_
	bool isStartOfFrame() noexcept
	{
		// Convert tabs (when feature is enabled)
		if ( *m_dataPtr == '\t' && m_convertTabs != '\t' )
		{
			*m_dataPtr = m_convertTabs;
		}

		// Reset my illegal-character-in-middle-of-frame flag when encountering an newline
		if ( isEofOfFrame() )
		{
			m_illegal = false;
		}

		if ( !m_illegal && *m_dataPtr >= 0x20 && *m_dataPtr <= 0x7E )
		{
			// Adjust my internal data pointer/len since I am NOT discarding the SOF character
			m_dataPtr--;
			m_dataLen++;
			return true;
		}

		return false;
	}

	/// See Cpl::Text::Frame::Decoder_
	bool isEofOfFrame() noexcept { return *m_dataPtr == 0x0A || *m_dataPtr == 0x0D; }

	/// See Cpl::Text::Frame::Decoder_
	bool isEscapeChar() noexcept { return false; }

	/// See Cpl::Text::Frame::Decoder_
	bool isLegalCharacter() noexcept
	{
		// Convert tabs (when feature is enabled)
		if ( *m_dataPtr == '\t' && m_convertTabs != '\t' )
		{
			*m_dataPtr = m_convertTabs;
		}

		// Newline is always 'valid' 
		if ( isEofOfFrame() )
		{
			m_illegal = false;
			return true;
		}

		// Reject all non printable ASCII character
		if ( *m_dataPtr > 0x7E || *m_dataPtr < 0x20 )
		{
			m_illegal = true;
			return false;
		}

		// If I get here, the character is valid
		return true;
	}
};





};      // end namespaces
};
};
#endif  // end header latch

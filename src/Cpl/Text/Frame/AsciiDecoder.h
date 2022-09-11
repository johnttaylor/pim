#ifndef Cpl_Text_Frame_AsciiDecoder_h_
#define Cpl_Text_Frame_AsciiDecoder_h_
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
	source.  In addition, this class only accepts ASCII characters within the
	the frame.

	Template args:
		BUFSIZE     Size of the internal buffer to use when reading raw
					characters from the Input stream.
 */
template <int BUFSIZE>
class AsciiDecoder : public StreamDecoder
{
protected:
	/// SOF character
	const char      m_sof;

	/// EOF character
	const char      m_eof;

	/// Escape character
	const char      m_esc;

	/// Remember printable ASCII characters ONLY option
	const bool      m_restricted;

	/// Raw input buffer for reading characters in 'chunks' from my Input stream (i.e. minimize the calls to read())
	char            m_buffer[BUFSIZE];


public:
	/** Constructor.  If 'restrict' is set to true ONLY printable ASCII
		characters (0x20-0x7E) are accepted inside a frame.  If false, then
		all ASCII characters (0x00-0x7F) are accepted inside a frame.  When
		a illegal character is detected, it causes the Decoder's state machine
		to reset and begin searching/looking-for the next start-of-frame
		character.
	 */
	AsciiDecoder( char startOfFrame, char endOfFrame, char escapeChar, bool restrict=true, Cpl::Io::Input* inputSource=0, bool blocking = true )
		:StreamDecoder( m_buffer, BUFSIZE, inputSource, blocking )
		, m_sof( startOfFrame )
		, m_eof( endOfFrame )
		, m_esc( escapeChar )
		, m_restricted( restrict )
	{
	}


protected:
	/// See Cpl::Text::Frame::Decoder_
	bool isStartOfFrame() noexcept { return *m_dataPtr == m_sof; }

	/// See Cpl::Text::Frame::Decoder_
	bool isEofOfFrame() noexcept { return *m_dataPtr == m_eof; }

	/// See Cpl::Text::Frame::Decoder_
	bool isEscapeChar() noexcept { return *m_dataPtr == m_esc; }

	/// See Cpl::Text::Frame::Decoder_
	bool isLegalCharacter() noexcept { return *m_dataPtr < 0x80 && ( !m_restricted || ( *m_dataPtr <= 0x7E && *m_dataPtr >= 0x20 ) ); }
};





};      // end namespaces
};
};
#endif  // end header latch

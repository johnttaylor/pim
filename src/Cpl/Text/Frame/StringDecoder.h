#ifndef Cpl_Text_Frame_StringDecoder_h_
#define Cpl_Text_Frame_StringDecoder_h_
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
/** @file */


#include "Cpl/Text/Frame/Decoder_.h"
#include "Cpl/Text/String.h"


///
namespace Cpl {
///
namespace Text {
///
namespace Frame {



/** This concrete class defines an interface a Text "Decoder" that
	accepts a null terminated string as its input source. See
	Cpl::Text::Frame::Decoder for details on what is a decoder.
 */
class StringDecoder : public Decoder_
{
protected:
	/// From the last scan: start of the data being scanned
	const char*         m_startPtr;

	/// From the last scan: pointer to the next character AFTER the LAST character decoded
	const char*         m_endPtr;

	/// Input source
	const char*         m_srcPtr;

	/// Input source length
	int                 m_srcLen;

	/// SOF character
	const char          m_sof;

	/// EOF character
	const char          m_eof;

	/// Escape character
	const char          m_esc;



public:
	/** Constructor. The optional 'inputSource' argument points to the raw
		frame data to be decoded.  If 'inputSource' is not set at construction
		time, it MUST be set BEFORE scan() is called or a fatal error will be
		generated.

		NOTE: Once scan() has be called the 'inputSouce' is considered to be
			  consumed and the Application must make a call to setInput()
			  before calling scan() again (if not, the scan() call will fail
			  - i.e. return false - with an end-of-input error).
	 */
	StringDecoder( char startOfFrame, char endOfFrame, char escapeChar, const char* inputSourceAsNullTerminatedString = 0 );



public:
	/** This method allows the Application/consumer to change/Set the Input
		source.
	 */
	virtual void setInput( const char* inputSourceAsNullTerminatedString ) noexcept;

	/** This method allows the Application/consumer to change/Set the Input
		source.  Note: 'sizeInBytesOfSource' does NOT include the/a null
		terminator
	 */
	virtual void setInput( const char* inputSoruce, int sizeInBytesOfSource ) noexcept;

	/** This method return a pointer to the next character AFTER the LAST character
		decoded.  The value returned from this method is ONLY valid after a
		call to scan() and BEFORE a subsequent call to scan() or setInput().
	 */
	virtual const char* getRemainder() const noexcept;



public:
	/// See Cpl::Text::Frame::Decoder
	bool scan( size_t maxSizeOfFrame, char* frame, size_t& frameSize ) noexcept;



protected:
	/// See Cpl::Text::Frame::Decoder_
	bool isStartOfFrame() noexcept;

	/// See Cpl::Text::Frame::Decoder_
	bool isEofOfFrame() noexcept;

	/// See Cpl::Text::Frame::Decoder_
	bool isEscapeChar() noexcept;

	/// See Cpl::Text::Frame::Decoder_
	bool isLegalCharacter() noexcept;


protected:
	/// See Cpl::Text::Frame::Decoder_
	bool read( void* buffer, int numBytes, int& bytesRead );

};




};      // end namespaces
};
};
#endif  // end header latch

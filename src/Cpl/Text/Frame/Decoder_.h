#ifndef Cpl_Text_Frame_Decoder_x_h_
#define Cpl_Text_Frame_Decoder_x_h_
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


#include "Cpl/Text/Frame/Decoder.h"


///
namespace Cpl {
///
namespace Text {
///
namespace Frame {



/** This private partially concrete class implements the basic/common logic for
	a frame decoder. A specialized subclass is required to supplied the details
	of the decoder.
 */
class Decoder_ : public Decoder
{
protected:
	/// Current number of characters remaining in my raw input buffer
	int             m_dataLen;

	/// Pointer to the next unprocessed character in my raw input buffer
	char*           m_dataPtr;

	/// Raw input buffer for reading characters in 'chunks' from my Input stream (i.e. minimize the calls to read())
	char*           m_buffer;

	/// Size of my raw input buffer
	size_t          m_bufSize;


public:
	/** Constructor.  The size of the rawInputBuffer determines how big of
		'chunks' data is read from the "input source", i.e. it is a working
		buffer and does NOT have to be the size of the maximum possible input
		frame.
	 */
	Decoder_( char rawInputBuffer[], size_t sizeOfRawInputBuffer );


public:
	/// See Cpl::Text::Frame::Decoder
	bool scan( size_t maxSizeOfFrame, char* frame, size_t& frameSize ) noexcept;


protected:
	/// Returns true if at start-of-frame
	virtual bool isStartOfFrame() noexcept = 0;

	/// Returns true if at end-of-frame
	virtual bool isEofOfFrame() noexcept = 0;

	/// Returns true if the start of the start of a escape sequence has been detected
	virtual bool isEscapeChar() noexcept = 0;

	/// Returns true if the current character is a legal/valid within a frame
	virtual bool isLegalCharacter() noexcept = 0;

	/** Attempts to read the specified number of bytes from the "input source"
		in the supplied buffer.  The actual number of bytes read is returned via
		'bytesRead'. Returns true if successful, or false if End-of-Input
		was encountered.
	 */
	virtual bool read( void* buffer, int numBytes, int& bytesRead ) = 0;

};





};      // end namespaces
};
};
#endif  // end header latch

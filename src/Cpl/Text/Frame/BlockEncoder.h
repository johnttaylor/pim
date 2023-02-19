#ifndef Cpl_Text_Frame_BlockEncoder_h_
#define Cpl_Text_Frame_BlockEncoder_h_
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


#include "Cpl/Text/Frame/StreamEncoder.h"
#include "Cpl/Io/Output.h"



///
namespace Cpl {
///
namespace Text {
///
namespace Frame {



/** This concrete class implements extends the StreamDecoder implementation to
	be more efficient with respect to outputting data to a Stream.  The encoded
	output data is temporarily cached in RAM and then written to the Stream.
 */
class BlockEncoder: public StreamEncoder
{
public:
	/** Constructor.  The Output Stream is not required to be specified at
		construction time (i.e. 'dstPtr' can be zero).  However, the encoder 
		must have a valid Output Stream handle BEFORE the start() method is called.
		When 'appendNewline' argument is set to true - the encoder will append a newline
		character to the output stream AFTER the EOF character (this can make
		for more human readable output stream).

		NOTE: If 'startOfFrame' is '\0', then Encoder will begin the
			  frame WITHOUT inserting a SOF character.  This is useful
			  when there application desires/has multiple start-of-frame
			  characters for a given frame.
	 */
	BlockEncoder( void* blockBuffer, size_t blockSizeInBytes, Cpl::Io::Output* dstPtr, char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline=true );

public:
	/// See Cpl::Text::Frame::Encoder_
	bool endFrame() noexcept;

protected:
	/// See Cpl::Text::Frame::Encoder_
	bool start( char src ) noexcept;

	/// See Cpl::Text::Frame::Encoder_
	bool start() noexcept;

	/// See Cpl::Text::Frame::Encoder_
	bool append( char src ) noexcept;

	/// Helper method
	bool appendToBlock( char src ) noexcept;

protected:
	/// Point to the client supplied buffer for a block
	uint8_t*  m_buffer;

	/// Size, in bytes, of the block buffer
	size_t m_bufferSize;

	/// Number of bytes currently stored in the block buffer
	size_t m_bufferCount;
};




};      // end namespaces
};
};
#endif  // end header latch

#ifndef Cpl_Text_Frame_StreamDecoder_h_
#define Cpl_Text_Frame_StreamDecoder_h_
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


#include "Cpl/Text/Frame/Decoder_.h"
#include "Cpl/Io/Input.h"


///
namespace Cpl {
///
namespace Text {
///
namespace Frame {



/** This partially concrete class defines an interface a Text "Decoder" that
	has a Cpl::Io::Input stream as its input source.  See
	Cpl::Text::Frame::Decoder for details on what is a decoder.
 */
class StreamDecoder : public Decoder_
{
protected:
	/** Constructor.  The 'inputSource' can optionally be specified at
		construction time or defer till later (the default is: later).  However,
		the "input source" MUST be specified BEFORF scan() is called or a
		fatal error will be generated.  See Cpl::Text::Frame::Decoder_ for
		details about the other constructor arguments.

		When the 'blocking' argument is set to false - decoder will not 
		block while waiting on input data.  The default behavior is to BLOCK.

		NOTE: A sub-class IS required to actual create an instance of a
		StreamDecoder.
	 */
	StreamDecoder( char rawInputBuffer[], size_t sizeOfRawInputBuffer, Cpl::Io::Input* inputSource=0, bool blocking = true );


public:
	/** This method allows the Application/consumer to change/Set the Input
		Stream handle.
	 */
	void setInput( Cpl::Io::Input& newInFd ) noexcept;


protected:
	/// See Cpl::Text::Frame::Decoder_
	bool read( void* buffer, int numBytes, int& bytesRead );

protected:
	/// Input stream
	Cpl::Io::Input* volatile m_srcPtr;

	/// Block semantics
	bool			m_blocking;
};




};      // end namespaces
};
};
#endif  // end header latch

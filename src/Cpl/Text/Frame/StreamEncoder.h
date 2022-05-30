#ifndef Cpl_Text_Frame_StreamEncoder_h_
#define Cpl_Text_Frame_StreamEncoder_h_
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


#include "Cpl/Text/Frame/Encoder_.h"
#include "Cpl/Io/Output.h"



///
namespace Cpl {
///
namespace Text {
///
namespace Frame {



/** This concrete class implements the Encoder API where the Output destination
	is a Cpl::Io::Output stream.  There is no checking/enforcement of the
	content of the Frame (e.g. it will accept non-ASCII character) except
	for the SOF, EOF, and ESC characters.
 */
class StreamEncoder : public Encoder_
{
protected:
	/// Output stream
	Cpl::Io::Output*    m_dstPtr;




public:
	/** Constructor.  The Output Stream is not required to be specified at
		construction time (i.e. 'dstPtr' can be zero).  However, the must
		by a valid Output Stream handle BEFORE the start() method is called.
		The 'appendNewline' argument when set to true will append a newline
		character to the output stream AFTER the EOF character (this can make
		for more human readable output stream).

		NOTE: If 'startOfFrame' is '\0', then Encoder will begin the
			  frame WITHOUT inserting a SOF character.  This is useful
			  when there application desires/has multiple start-of-frame
			  characters for a given frame.
	 */
	StreamEncoder( Cpl::Io::Output* dstPtr, char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline=true );

public:
	/// Allow the consumer to change/Set the Output stream handle.  Note: No guarantees on what happens if this method is called in the 'middle of a frame'
	void setOutput( Cpl::Io::Output& newOutfd ) noexcept;


protected:
	/// See Cpl::Text::Frame::Encoder_
	bool start( char src ) noexcept;

	/// See Cpl::Text::Frame::Encoder_
	bool start() noexcept;

	/// See Cpl::Text::Frame::Encoder_
	bool append( char src ) noexcept;

};




};      // end namespaces
};
};
#endif  // end header latch

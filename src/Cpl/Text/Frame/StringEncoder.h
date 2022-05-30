#ifndef Cpl_Text_Frame_StringEncoder_h_
#define Cpl_Text_Frame_StringEncoder_h_
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
#include "Cpl/Text/String.h"



///
namespace Cpl {
///
namespace Text {
///
namespace Frame {



/** This concrete class implements the Encoder API where the Output destination
	is a Cpl::Text::String.  There is no checking/enforcement of the content of
	the Frame (e.g. it will accept non-ASCII character) except for the SOF, EOF,
	and ESC characters.
 */
class StringEncoder : public Encoder_
{
protected:
	/// Output stream
	Cpl::Text::String&  m_dst;

	/// Append flag
	bool                m_append;


public:
	/** Constructor.  The 'appendNewline' argument when set to true will
		append a newline character to the output stream AFTER the EOF
		character (this can make for more human readable output stream). The
		'appendToDst' flag when true will not clear contents of 'dst' on
		the start-of-frame - rather it will ALWAYS append all output to 'dst'

		NOTES:

			o If 'startOfFrame' is '\0', then Encoder will begin the
			  frame WITHOUT inserting a SOF character.  This is useful
			  when there application desires/has multiple start-of-frame
			  characters for a given frame.

			o If 'escapeChar' is '\0', then Encoder will NOT escape any
			  characters - buyers beware when using this feature!
	 */
	StringEncoder( Cpl::Text::String& dst, char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline=true, bool appendToDst=false );

public:
	/// Returns a reference to the String that holds the encoded frame.  The contents of the buffer depends on the state of frame being built.
	Cpl::Text::String& getFramedOutput();


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

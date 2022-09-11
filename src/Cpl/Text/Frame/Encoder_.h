#ifndef Cpl_Text_Frame_Encoder_x_h_
#define Cpl_Text_Frame_Encoder_x_h_
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


#include "Cpl/Text/Frame/Encoder.h"



///
namespace Cpl {
///
namespace Text {
///
namespace Frame {



/** This Private Namespace partially concrete class implements common/helper
	method for supporting the Encoder API. There is no checking/enforcement
	of the content of the Frame (e.g. it will accept non-ASCII character)
	except for the SOF, EOF, and ESC characters.
 */
class Encoder_ : public Encoder
{
protected:
	/// Output/Frame state
	bool                m_inFrame;

	/// SOF character
	const char          m_sof;

	/// EOF character
	const char          m_eof;

	/// Escape character
	const char          m_esc;

	/// Remember my newline option
	const bool          m_appendNewline;



protected:
	/** Constructor.  The 'appendNewline' argument when set to true will
		append a newline character to the output stream AFTER the EOF
		character (this can make for more human readable output stream).

		NOTES:

			o If 'startOfFrame' is '\0', then Encoder will begin the
			  frame WITHOUT inserting a SOF character.  This is useful
			  when there application desires/has multiple start-of-frame
			  characters for a given frame.

			o If 'escapeChar' is '\0', then Encoder will NOT escape any
			  characters - buyers beware when using this feature!
	 */
	Encoder_( char startOfFrame, char endOfFrame, char escapeChar, bool appendNewline=true );


public:
	/// See Cpl::Text::Frame::Encoder
	bool startFrame( void ) noexcept;

	/// See Cpl::Text::Frame::Encoder
	bool output( char src ) noexcept;

	/// See Cpl::Text::Frame::Encoder
	bool output( const char* src ) noexcept;

	/// See Cpl::Text::Frame::Encoder
	bool output( const char* src, size_t numBytes ) noexcept;

	/// See Cpl::Text::Frame::Encoder
	bool endFrame( void ) noexcept;


protected:
	/// Helper method - implemented by the child class
	virtual bool start( char src ) noexcept = 0;

	/// Helper method - implemented by the child class
	virtual bool start() noexcept = 0;

	/// Helper method - implemented by the child class
	virtual bool append( char src ) noexcept = 0;

	/** Returns the encoded/escaped value for the specified special character.  
		The default implementation simply returns 'charToBeEscaped'
	 */
	virtual char encodeChar( char charToBeEscaped );
};




};      // end namespaces
};
};
#endif  // end header latch

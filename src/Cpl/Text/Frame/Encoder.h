#ifndef Cpl_Text_Frame_Encoder_h_
#define Cpl_Text_Frame_Encoder_h_
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


#include <stdlib.h>


///
namespace Cpl {
///
namespace Text {
///
namespace Frame {



/** This class defines an interface for 'transmitted' (encoding) a frame.  What
	is a frame ?  See the Decoder API for the detailed answer to this question.
	The Encoder interface will consume a raw sequence of characters and
	properly insert (when necessary) the framing characters to construct a
	valid frame. Below are some examples some framed sequences of characters:
	<pre>

		Given:
			SOF:= '.'
			EOF:= ';'
			ESC:= '~'


		Raw Stream      Encoded Sequence(s)
		-----------     --------------------
		"abcde"         ".abcde;"
		"a"             ".a;"
		""              ".;"
		"a;bcd"         ";a~;bcd;"
		"~"             ".~~;"
		".abc"          "..abc;"


		Notes:
			o The quotes (") in the above example are NOT part of the frame
			  and/or character sequences - the quotes are only used to
			  illustrate sets of characters.
			o The SOF character does not need to be escaped within a frame
			  because once a SOF has been found - the SOF character is NOT
			  looked/scanned for until after an EOF character has been
			  detected.  Escaping an SOF character within a frame will
			  behave as expected, i.e. same behavior/semantics as escaping
			  the EOF character.
	</pre>

 */
class Encoder
{
public:
	/** Begins the frame.  The method will return false if there an
		error occurred while writing to the output destination; else true is
		returned.  If this method is called twice - without an intervening
		call to endFrame() - a Fatal error is generated
	 */
	virtual bool startFrame( void ) noexcept = 0;

	/** Outputs the single character to the output destination.  The method
		will return false if there an error occurred while writing to the
		output destination; else true is returned.  If this method is called
		without a previous call to startFrame(), i.e. the frame has NOT been
		started, a Fatal error is generated
	 */
	virtual bool output( char src ) noexcept = 0;

	/** Outputs the null terminated string to the output destination.  The
		method  will return false if there an error occurred while writing to
		the  output destination; else true is returned.  If this method is
		called  without a previous call to startFrame(), i.e. the frame has
		NOT been started, a Fatal error is generated
	 */
	virtual bool output( const char* src ) noexcept = 0;

	/** Outputs 'numBytes' of data (from 'src') to the output destination.
		The  method will return false if there an error occurred while
		writing to  the output destination; else true is returned.  If this
		method is called  without a previous call to startFrame(), i.e. the
		frame has NOT been started, a Fatal error is generated
	 */
	virtual bool output( const char* src, size_t numBytes ) noexcept = 0;

	/** Ends the frame.  The method will return false if there an error
		occurred while writing to the output destination; else true is
		returned.  If this method is called twice - without an intervening
		call to startFrame() - a Fatal error is generated
	 */
	virtual bool endFrame( void ) noexcept = 0;


public:
	/// Virtual Destructor
	virtual ~Encoder() {}
};




};      // end namespaces
};
};
#endif  // end header latch

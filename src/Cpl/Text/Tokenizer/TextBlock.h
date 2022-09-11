#ifndef Cpl_Text_Tokenizer_TextBlock_h_
#define Cpl_Text_Tokenizer_TextBlock_h_
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


///
namespace Cpl {
///
namespace Text {
///
namespace Tokenizer {


/** This concrete class tokenizes a Text Block that has the following
	format: <pre>

	parameter [(delimiter parameter)*] [terminator]

	parameter := { all printable characters except for QUOTE, DEL, TERM, and ESC }
	QUOTE     := { specified quote character (used to start/end TEXT STRINGS) }
	DEL       := { specified delimiter character }
	TERM      := { specified terminator character }
	ESC       := { specified Escape character used inside of TEXT STRINGS }

	The tokenizer replaces the delimiter and terminator characters with an '\0'
	character. The tokenizer supports the parameter fields being "text strings".
	In addition, any leading and trailing whitespace is removed from each
	parameter. Note: no whitespace is altered/removed from the contents of a
	text string.

	If the string being parsed has two DEL characters in row, then tokenized
	results will return an 'empty parameter' (a parameter will a zero string
	length) for this sequence.  The EXCEPTION to this rule is when the
	delimiter is the space character, then the tokenizer will never generated
	and/or detect any 'empty parameters'.

	The original string is "corrupted" since the tokenizer changes characters.
	Also, the tokenizer will shorten the string when dealing with 'text
	strings'.

	A TEXT STRING can contain any alphanumeric and/or punctuation characters
	including the QUOTE, DEL, TERM, and ESC characters.  A text string is
	always enclosed with beginning and ending QUOTE characters.  When a QUOTE
	character is encounter outside of a text string, it always starts a text
	string. To enter a literal QUOTE character within a text string, precede the
	QUOTE character with the ESC character (e.g., \").  To enter a literal ESC
	character within a text string, precede the character with ESC character
	(e.g., \\). IMPORTANT NOTE: The QUOTE and ESC characters are REMOVED from
	the string when it is tokenized!


</pre>*/
class TextBlock
{
private:
	/// Pointer to the first data field
	char*       m_base;

	/// Pointer to the current token
	char*       m_ptr;

	/// Indicates if the tokens are valid
	bool        m_validTokens;

	/// Indicates if the terminator character was encountered
	bool        m_terminatorFound;

	/// Number of data fields
	unsigned    m_count;



public:
	/** Constructor.  Requires a pointer to the 'raw' string to be tokenized.
		Note: All of the parsing occurs in this method.
	 */
	TextBlock( char* string, char delimiter=',', char terminator=';', char quote='"', char escape='\\' );



public:
	/** Returns true if the string was successfully tokenized.  The string
		will fail to parse if it does not match the format/syntax described
		in the class definition.

		THIS METHOD MUST BE CALLED BEFORE ANY OF THE FOLLOWING
		METHODS.  THE PARSED TOKENS ARE ONLY VALID IF THIS METHOD
		RETURNS TRUE!
	 */
	inline bool isValidTokens() const noexcept { return m_validTokens; }


	/// Returns the number of parameter fields in the Text block
	inline unsigned numParameters() const noexcept { return m_count; }


	/// Returns the Nth parameter (index starts with 0). Return a null pointer if index is out-of-range
	const char* getParameter( unsigned index ) const noexcept;


	/// Returns true if the parsing stopped because the terminator character was encounter (vs. end-of-string)
	inline bool isTerminated() const noexcept { return m_terminatorFound; }


	/** Returns a pointer to the portion of the string that has not
		been tokenized, i.e. the first character AFTER the terminator
		character.
	 */
	inline const char* remaining() const noexcept { return m_ptr; }


protected:
	/// Helpter method
	void removeWhiteSpace( char* startOfTokenPtr, char* firstNonSpacePtr, char* lastNonSpacePtr ) noexcept;

};


};      // end namespaces
};
};
#endif  // end header latch

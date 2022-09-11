#ifndef Cpl_Text_Tokenizer_Basic_h_
#define Cpl_Text_Tokenizer_Basic_h_
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


/** This concrete class performs basic tokenizing/parse functions on a
	string. A token is a one or more characters separated by one or
	more delimiter characters. The parsing is a destructive parsing in
	that it modifies the original string.  The tokenizing is done
	one token at time i.e. when the next() method is called.
 */
class Basic
{
private:
	/// Definition of whitespace
	static const char* whiteSpace() { return " \t\n\r"; };

private:
	/// Pointer to the current token
	char*       m_ptr;

	/// Pointer to the token delimiters
	const char* m_delimiters;

	/// Pointer to the start of the original string
	char*       m_base;

	/// Number of tokens parsed to date;
	unsigned    m_count;

public:
	/// Constructor. Uses whitespace for token delimiters
	Basic( char* stringToParse );

	/// Constructor. Uses the specified character set for token delimiters
	Basic( char* stringToParse, const char* delimiterSet );

public:
	/** Returns a pointer to the next token.  Returns 0 if no tokens exist,
		or the string has already been completed tokenized. NOTE: This
		method is also used to get the 'first' token.
	 */
	const char* next() noexcept;

	/** Returns a pointer to the portion of the string that has not
		been tokenized, i.e. the first character AFTER the delimiter character
		that marked the end of the current token to End-of-String.
	 */
	inline const char* remaining() const noexcept { return m_ptr; }


public:
	/** This method returns the Nth token.  Where N is the zero-based token
		index, i.e. 0:=first token, 1:=second token, etc. If N is out-of-bound
		then 0 is returned.  Typically this method is called once the entire
		string has been tokenized.
	 */
	const char* getToken( unsigned n ) const noexcept;

	/** Returns the number of tokens parsed to-date.
	 */
	inline unsigned numTokens() const noexcept { return m_count; }

};


};      // end namespaces
};
};
#endif  // end header latch

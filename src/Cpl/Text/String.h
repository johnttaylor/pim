#ifndef Cpl_Text_String_h_
#define Cpl_Text_String_h_
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

#include "colony_map.h"
#include "Cpl/Container/Key.h"
#include <stdarg.h>
#include <iostream>

///
namespace Cpl {
///
namespace Text {

/** This abstract class defines the operations that can be before on
	a NULL terminated string. The intent is to provide a simple mechanism
	for manipulating and "passing-around" strings rather than using error
	prone explicit string operations.

	NOTES:

		o The memory management for the string data is determined by
		  the concrete sub-class!
		o The 'String' class IS A Cpl::Container::Key, but it is NOT a
		  Cpl::Container::xxxItem instance.  Use the 'StringItem' class
		  for storing a string directly in a container.
 */

class String : public Cpl::Container::Key
{
public:
	/// Virtual destructor!
	virtual ~String() {}


public:
	///@{
	/** Assignment of a string. The argument string is copied
		to the internal storage of the object.
	 */
	virtual Cpl::Text::String& operator=( const String& string ) = 0;

	/** Assignment of a string. The argument string is copied
		to the internal storage of the object.
	 */
	virtual Cpl::Text::String& operator=( const char* string ) = 0;

	/** Assigns the character to first position in string and then
		NULL terminates the string.
	 */
	virtual Cpl::Text::String& operator=( char c ) = 0;

	/** Converts the 'number' to a string and stores its in
		the internal storage of the object.
	 */
	virtual Cpl::Text::String& operator=( int num ) = 0;

	/** Converts the 'number' to a string and stores its in
		the internal storage of the object.
	 */
	virtual Cpl::Text::String& operator=( unsigned int num ) = 0;

	/** Converts the 'number' to a string and stores its in
		the internal storage of the object.
	 */
	virtual Cpl::Text::String& operator=( long num ) = 0;

	/** Converts the 'number' to a string and stores its in
		the internal storage of the object.
	 */
	virtual Cpl::Text::String& operator=( long long num ) = 0;

	/** Converts the 'number' to a string and stores its in
		the internal storage of the object.
	 */
	virtual Cpl::Text::String& operator=( unsigned long num ) = 0;

	/** Converts the 'number' to a string and stores its in
		the internal storage of the object.
	 */
	virtual Cpl::Text::String& operator=( unsigned long long num ) = 0;

	/** This method copies the content of the source string,
		up to 'n' characters, into internal storage of the object.
		NOTE: The internal storage is always guaranteed to be NULL
		terminated.
	 */
	virtual void copyIn( const char* string, int n ) = 0;

	/** This method inserts the specified string starting at
		the specified offset.  The original contents of the string
		beginning with 'insertOffset' is "shifted to the right" to
		make room for the string being inserted. If an attempt is
		made to insert the string past the current end-of-string, then
		the stringToInsert is simply appended.  If an attempt is made
		to insert the string at a negative offset, then the stringToInsert
		is prepended to the string.
	 */
	virtual void insertAt( int insertOffset, const char* stringToInsert ) = 0;

	/** Clears the internal string (i.e. string[0] = '\0').
	 */
	virtual void clear() = 0;
	///@}

public:
	///@{
	/** Append a string. The argument string is concatenated
		to the end of the string object.
	 */
	virtual Cpl::Text::String& operator +=( const Cpl::Text::String& string ) = 0;

	/** Append a string. The argument string is concatenated
		to the end of the string object.
	 */
	virtual Cpl::Text::String& operator +=( const char* string ) = 0;

	/** Appends the character to the end of the string.
	 */
	virtual Cpl::Text::String& operator +=( char c ) = 0;

	/** Converts the 'number' to a string and appends its to the
		end of the string.
	 */
	virtual Cpl::Text::String& operator +=( int num ) = 0;

	/** Converts the 'number' to a string and appends its to the
		end of the string.
	 */
	virtual Cpl::Text::String& operator +=( unsigned int num ) = 0;

	/** Converts the 'number' to a string and appends its to the
		end of the string.
	 */
	virtual Cpl::Text::String& operator +=( long num ) = 0;

	/** Converts the 'number' to a string and appends its to the
		end of the string.
	 */
	virtual Cpl::Text::String& operator +=( long long num ) = 0;

	/** Converts the 'number' to a string and appends its to the
		end of the string.
	 */
	virtual Cpl::Text::String& operator +=( unsigned long num ) = 0;

	/** Converts the 'number' to a string and appends its to the
		end of the string.
	 */
	virtual Cpl::Text::String& operator +=( unsigned long long num ) = 0;

	/** This method appends the content of the source string,
		up to 'n' characters, into internal storage of the object.
		NOTE: The internal storage is always guaranteed to be NULL
		terminated.
	 */
	virtual void appendTo( const char* string, int n ) = 0;

	///@}


public:
	///@{
	/// Cast to character -->i.e returns the first character in the string.
	virtual operator char() const = 0;

	/// Read-only Access to the "raw" string.
	virtual const char* getString() const = 0;

	/// Cast to read-only character string pointer.
	inline operator const char* ( ) const { return getString(); }

	/// Returns a Read-only pointer to the "raw" (short-hand for getString())
	inline const char* operator()() const { return getString(); }

	/// Returns the specified character. If n is out-bounds, then '\0' is returned
	inline char operator[] ( int i ) const { return i<0 || i>length() ? '\0' : getString()[i]; }
	///@}


public:
	///@{
	/** Returns the length, in bytes, of the string.  The size returned does
		NOT include NULL terminator (i.e. if the returned size is 15, my
		internal storage must be at least 15+1)
	 */
	virtual int length() const = 0;

	/** Returns the max allowed size of the string. The size returned does
		NOT include NULL terminator (i.e. if the returned size is 15, my
		internal storage must be at least 15+1)
	 */
	virtual int maxLength() const = 0;

	/** Returns the available space left in the internal buffer. The size
		returned does NOT include NULL terminator (i.e. if the returned size
		is 15, my internal storage must be at least 15+1).
	 */
	inline int  availLength() { return maxLength() - length(); }

	/// Returns true if the string is empty (i.e. string[0] == '\0')
	virtual bool isEmpty() const = 0;

	/** Returns true if the last String "write" operation caused the
		String contents to be truncated.  This flag is reset after every
		string "write" operation.
	 */
	virtual bool truncated() const = 0;
	///@}


public:
	///@{
	/** Compares two Strings and returns true if they are equal
		NOTE: if 'string' is a null pointer, then false is returned.
	 */
	virtual bool operator ==( const char* string ) const = 0;

	/// Compares two Strings and returns true if they are equal
	virtual bool operator ==( const Cpl::Text::String& string ) const = 0;

	/** Compares two Strings and returns true if they are NOT equal
		NOTE: if 'string' is a null pointer, then true is returned.
	 */
	virtual bool operator !=( const char* string ) const = 0;

	/// Compares two Strings and returns true if they are NOT equal
	virtual bool operator !=( const Cpl::Text::String& string ) const = 0;

	/** Compares two strings independently of case and return true if equal
		NOTE: if 'string' is a null pointer, then false is returned.
	 */
	virtual bool isEqualIgnoreCase( const char* string ) const = 0;

	/// Compares the specified character to the first character in the string
	virtual bool operator ==( char c ) const = 0;

	/** Compares two strings and returns <0, 0, >0 if this string is less than,
		equal, or greater than respectively to the specified string.
		NOTE: if 'string' is a null pointer, then -1 is returned.
	 */
	virtual int compare( const char* string ) const = 0;

	/// Same as compare(), but case insensitive.
	virtual int compareIgnoreCase( const char* string ) const = 0;

	/** Compares the specified substring with the specified string.  Substring
		is inclusive of the endpoints.  If the endpoints are out-of-range, they
		are clamped by the actual bounds of the string.  Position parameters are
		zero-based indexes into this String object.
		NOTE: if 'string' is a null pointer, then false is returned.
	 */
	virtual bool isEqualSubstring( int startOffset, int endOffset, const char* string ) const = 0;

	/// Same as above, except case insensitive comparison.
	virtual bool isEqualSubstringIgnoreCase( int startOffset, int endOffset, const char* string ) const = 0;

	/** Returns true if the string starts with the specified sub-string.
		Note: if 'string' is a null pointer, then false is returned
	 */
	virtual bool startsWith( const char* string ) const = 0;

	/** Sames as starsWith(), but begins the search at index 'startOffset'.
		Note: If 'string' is a null pointer or 'startOffset' is out-of-bound,
		then false is returned.
	 */
	virtual bool startsWith( const char* string, int startOffset ) const = 0;

	/** Returns true if the string ends with the specified sub-string.
		Note: if 'string' is a null pointer, then false is returned
	 */
	virtual bool endsWith( const char* string ) const = 0;

	///@}

public:
	///@{
	/** Returns the index of the first match for the specified character.  If
		no match is found, -1 is returned.
	 */
	virtual int indexOf( char c ) const = 0;

	/** Same as indexOf(char), but starts at 'startOffset' instead of the beginning
		of the string.

		Note: If 'startOffset' is out-of-bounds than -1 is returned.
	 */
	virtual int indexOf( char c, int startOffset ) const = 0;

	/** Returns the index (of the starting character) of the first match
		for the specified sub-string.  If no match is found, -1 is returned.

		Note: If 'string' is null, then -1 is returned
	 */
	virtual int indexOf( const char* string ) const = 0;

	/** Same as indexOf(const char*), but starts at 'startOffset' instead of the
		beginning of the string.

		Note: If 'string' is null or 'startOffset' is out-of-bounds than -1
			  is returned.
	 */
	virtual int indexOf( const char* string, int startOffset ) const = 0;

	/** Returns the index of the last match for the specified character.  If
		no match is found, -1 is returned.
	 */
	virtual int lastIndexOf( char c ) const = 0;

	/** Returns the number of times the specified character appears in
		the string.
	 */
	virtual int count( char c ) const = 0;
	///@}


public:
	///@{
	/** Allows "printf" formatting of the string. NOTE: Since the
		amount of information being written to the string is variable,
		this method does NOT guarantee how much, if any, of the
		information is actually written to the string.  The method
		does guarantee that the string's internal storage will NOT
		be exceeded.

		NOTE: if 'format' is null, then nothing is done
	 */
	virtual void format( const char* format, ... ) = 0;

	/// Same as format() - but appends "formatting" to the end of the string
	virtual void formatAppend( const char* format, ... ) = 0;

	/** This method is the same as format(), except when 'appendFlag' is true
		then it behaves as formatAppend().
	 */
	virtual void formatOpt( bool appendFlag, const char* format, ... ) = 0;


	/** Same as format(), except that it is called with a va_list
		instead of a variable number of arguments.
	 */
	virtual void vformat( const char* format, va_list ap ) = 0;

	/** Same as formatAppend(), except that it is called with a va_list
		instead of a variable number of arguments.
	 */
	virtual void vformatAppend( const char* format, va_list ap ) = 0;

	/** This method is the same as vformat(), except when 'appendFlag' is true
		then it behaves as vformatAppend().
	 */
	virtual void vformatOpt( bool appendFlag, const char* format, va_list ap ) = 0;
	///@}


public:
	///@{
	/** Forces the entire string to upper case characters.  Also returns
		the newly modified string contents.
	 */
	virtual const char* toUpper() = 0;

	/** Forces the entire string to lower case characters.  Also returns
		the newly modified string contents.
	 */
	virtual const char* toLower() = 0;

	/** Removes any leading white-space from the string (white-space
		is defined by the standard isspace() function).
	 */
	virtual void removeLeadingSpaces() = 0;

	/** Removes any trailing white-space from the string (white-space
		is defined by the standard isspace() function).
	 */
	virtual void removeTrailingSpaces() = 0;

	/** Removes the specified leading characters from the string.

		NOTE: if 'charsSet' is null, nothing is done.
	 */
	virtual void removeLeadingChars( const char* charsSet ) = 0;

	/** Removes the specified trailing characters from the string.

		NOTE: if 'charsSet' is null, nothing is done.
	 */
	virtual void removeTrailingChars( const char* charsSet ) = 0;

	/** Removes the characters beginning with 'startOffset' and ending with
		'endOffset'.  The characters after 'endOffset' are shifted left
		to form a continuous string. If the position fields are invalid
		or out-of-bounds, nothing is done.
	 */
	virtual void cut( int startOffset, int endOffset ) = 0;

	/** Removes 'n' characters from the start-of-string. If 'n' is greater
		than the length of the string or less than zero, nothing is done.
	 */
	inline void  trimLeft( int n ) { cut( 0, n - 1 ); }

	/** Removes 'n' characters from the end-of-string. If 'n' is greater
		than the length of the string or less than zero, nothing is done.
	 */
	virtual void trimRight( int n ) = 0;

	/** Replaces the character at index 'atOffset' with 'newchar'.
		If 'atOffset' is out-of-bounds or greater than the length of the
		actual string, then nothing is done.
	 */
	virtual void setChar( int atOffset, char newchar ) = 0;

	/** Replaces all instances of targetChar in the string with
		newChar.  Returns the number characters (if any) replaced.
		NOTE: If targetChar == newChar, nothing is done and zero
			  is returned.
	 */
	virtual int  replace( char targetChar, char newChar ) = 0;
	///@}


public:
	///@{
	/** Returns a "writable" pointer to the string's internal storage.
		The caller is RESPONSIBLE for insuring that the string is
		left in a valid state - that is the string is NULL terminated
		and has NOT exceeded the 'maxAllowedLength'.  Also The pointer
		and length have a VERY LIMITED life span - the values become
		invalid when any of the another string methods are called!
		NOTE: This method always clears the 'truncated' flags/status.

		*** THIS METHOD SHOULD BE USED WITH EXTREME CAUTION AND AVOIDED WHENEVER POSSIBLE! ***
	 */
	virtual char* getBuffer( int& maxAllowedLength ) = 0;
	///@}
};


};      // end namespaces
};



/* Note: I could only make the stream operators work if there were NOT declared
		 within the Cpl::Text namespace.  Fortunately, I don't actually have to
		 make the stream operators 'friends' for the necessary implementation.

		 If someone could explain/show me how to make it work using the
		 'friend' approach - please contact me.
*/
namespace std {

/// Support stream output operator directly
ostream& operator<< ( ostream &out, const Cpl::Text::String& outstring );

/// Support stream input operator directly
istream& operator>> ( istream &in, Cpl::Text::String& instring );

};

#endif  // end header latch

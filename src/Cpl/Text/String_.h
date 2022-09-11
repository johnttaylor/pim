#ifndef Cpl_Text_String_x_h_
#define Cpl_Text_String_x_h_
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

#include "Cpl/Text/String.h"


///
namespace Cpl {
///
namespace Text {


/** This partially concrete class implements the portions of the String Api
	that is independent of the internal storage mechanisms.
 */

class String_ : public String
{
protected:
	/// Flag to track when the internal string get truncated
	bool  m_truncated;

	/// Pointer to the actual internal string
	char* m_strPtr;


protected:
	/// Constructor
	String_( char* internalStringPtr );


public:
	/// See Cpl::Text::String
	void clear();

	/// See Cpl::Text::String
	operator char() const;

	/// See Cpl::Text::String
	int  length() const;

	/// See Cpl::Text::String
	bool isEmpty() const;

	/// See Cpl::Text::String
	bool truncated() const;

	/// See Cpl::Text::String
	bool operator ==( const char* string ) const;

	/// See Cpl::Text::String
	bool operator ==( const String& string ) const;

	/// See Cpl::Text::String
	bool operator !=( const char* string ) const;

	/// See Cpl::Text::String
	bool operator !=( const String& string ) const;

	/// See Cpl::Text::String
	bool isEqualIgnoreCase( const char* string ) const;

	/// See Cpl::Text::String
	int  compare( const char* string ) const;

	/// See Cpl::Text::String
	int  compareIgnoreCase( const char* string ) const;

	/// See Cpl::Text::String
	bool operator ==( char c ) const;

	/// See Cpl::Text::String
	bool isEqualSubstring( int startpos, int endpos, const char* string ) const;

	/// See Cpl::Text::String
	bool isEqualSubstringIgnoreCase( int startpos, int endpos, const char* string ) const;

	/// See Cpl::Text::String
	bool startsWith( const char* string ) const;

	/// See Cpl::Text::String
	bool startsWith( const char* string, int startOffset ) const;

	/// See Cpl::Text::String
	bool endsWith( const char* string ) const;

	/// See Cpl::Text::String
	void format( const char* format, ... );

	/// See Cpl::Text::String
	void formatAppend( const char* format, ... );

	/// See Cpl::Text::String
	void formatOpt( bool appendFlag, const char* format, ... );

	/// See Cpl::Text::String
	void vformat( const char* format, va_list ap );

	/// See Cpl::Text::String
	void vformatAppend( const char* format, va_list ap );

	/// See Cpl::Text::String
	void vformatOpt( bool appendFlag, const char* format, va_list ap );

	/// See Cpl::Text::String
	int  indexOf( char c ) const;

	/// See Cpl::Text::String
	int  indexOf( char c, int startpos ) const;

	/// See Cpl::Text::String
	int  indexOf( const char* str ) const;

	/// See Cpl::Text::String
	int  indexOf( const char* str, int startpos ) const;

	/// See Cpl::Text::String
	int  lastIndexOf( char c ) const;

	/// See Cpl::Text::String
	int count( char c ) const;

	/// See Cpl::Text::String
	const char* toUpper();

	/// See Cpl::Text::String
	const char* toLower();

	/// See Cpl::Text::String
	void removeLeadingSpaces();

	/// See Cpl::Text::String
	void removeTrailingSpaces();

	/// See Cpl::Text::String
	void removeLeadingChars( const char* charsSet );

	/// See Cpl::Text::String
	void removeTrailingChars( const char* charsSet );

	/// See Cpl::Text::String
	void cut( int startpos, int endpos );

	/// See Cpl::Text::String
	void trimRight( int n );

	/// See Cpl::Text::String
	void setChar( int atPosition, char newchar );

	/// See Cpl::Text::String
	int  replace( char targetChar, char newChar );

	/// See Cpl::Text::String
	const char* getString() const;

	/// See Cpl::Text::String
	char* getBuffer( int& maxAllowedLength );

	/// Make parent method visible
	using Cpl::Text::String::operator=;

	/// Make parent method visible
	using Cpl::Text::String::operator+=;

	/// Assignment
	Cpl::Text::String& operator=( const String& string );

	/// Assignment
	Cpl::Text::String& operator=( const char* string );

	/// Assignment
	Cpl::Text::String& operator=( char c );

	/// Append
	Cpl::Text::String & operator +=( const String& string );

	/// Append
	Cpl::Text::String& operator +=( const char* string );

	/// Append
	Cpl::Text::String& operator +=( char c );

public:
	/// See Cpl::Container::Key
	int compareKey( const Key& key ) const;

	/// See Cpl::Container::Key
	const void* getRawKey( unsigned* returnRawKeyLenPtr = 0 ) const;



protected: // Helpers
	/// Sets the truncated flag if needed and insures string is null terminated
	void validateSizeAfterFormat( int availableLen, int formattedLen, int maxlen );

};

};      // end namespaces
};
#endif  // end header latch

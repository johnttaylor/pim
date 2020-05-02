#ifndef Cpl_Text_FString_h_
#define Cpl_Text_FString_h_
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

#include "Cpl/Text/FString_.h"
#include <string.h>

///
namespace Cpl {
namespace Text {



/** This template class represents a NULL terminated string of a
	specific length.  The size (aka in the internal storage) of string cannot
	be changed. Any 'write' operations that exceed the length of the internal
	storage - the results are silently truncated and the string is ALWAYS
	left in null terminated state.

	Template Args:  S:=  Max Size of the String WITHOUT the null
						 terminator!

	NOTE: See base class - String - for a complete listing/description of
		  the class's methods.
 */
template <int S>
class FString : public FString_
{
private:
	/// Storage for the string
	char    m_strMem[S + 1];

public:
	/// Constructor
	FString( const FString<S>& string ) :FString_( string.getString(), m_strMem, S ) {}

	/// Constructor
	FString( const Cpl::Text::String& string ) :FString_( string.getString(), m_strMem, S ) {}

	/// Constructor
	FString( const char* string="" ) :FString_( string, m_strMem, S ) {}

	/// Constructor
	FString( char c ) :FString_( c, m_strMem, S ) {}

	/// Constructor
	FString( int num ) :FString_( num, m_strMem, S ) {}

	/// Constructor
	FString( unsigned num ) :FString_( num, m_strMem, S ) {}

	/// Constructor
	FString( long num ) :FString_( num, m_strMem, S ) {}

	/// Constructor
	FString( long long num ) :FString_( num, m_strMem, S ) {}

	/// Constructor
	FString( unsigned long num ) :FString_( num, m_strMem, S ) {}

	/// Constructor
	FString( unsigned long long num ) :FString_( num, m_strMem, S ) {}


public:
	///@{
	/// Assignment
	inline Cpl::Text::String& operator =( const FString<S>& string ) { copyIn( string, string.length() ); return *this; }

	/// Assignment
    Cpl::Text::String& operator=( const Cpl::Text::String& string ) { copyIn( string, string.length() ); return *this; }

	/// Assignment
    Cpl::Text::String& operator=( const char* string ) { copyIn( string, string ? strlen( string ) : 0 ); return *this; }

	/// Assignment
    Cpl::Text::String& operator=( char c ) { copyIn( &c, 1 ); return *this; }

	/// Assignment
    Cpl::Text::String& operator=( int num ) { format( "%d", num ); return *this; }

	/// Assignment
    Cpl::Text::String& operator=( unsigned int num ) { format( "%u", num ); return *this; }

	/// Assignment
    Cpl::Text::String& operator=( long num ) { format( "%ld", num ); return *this; }

	/// Assignment
    Cpl::Text::String& operator=( long long num ) { format( "%lld", num ); return *this; }

	/// Assignment
    Cpl::Text::String& operator=( unsigned long num ) { format( "%lu", num ); return *this; }

	/// Assignment
    Cpl::Text::String& operator=( unsigned long long num ) { format( "%llu", num ); return *this; }
	///@}

public:
	///@{
	/// Append
    Cpl::Text::String & operator +=( const String& string ) { appendTo( string, string.length() ); return *this; }

	/// Append
    Cpl::Text::String& operator +=( const char* string ) { appendTo( string, string ? strlen( string ) : 0 ); return *this; }

	/// Append
    Cpl::Text::String& operator +=( char c ) { appendTo( &c, 1 ); return *this; }

	/// Append
    Cpl::Text::String& operator +=( int num ) { formatAppend( "%d", num ); return *this; }

	/// Append
    Cpl::Text::String& operator +=( unsigned int num ) { formatAppend( "%u", num ); return *this; }

	/// Append
    Cpl::Text::String& operator +=( long num ) { formatAppend( "%ld", num ); return *this; }

	/// Append
    Cpl::Text::String& operator +=( long long num ) { formatAppend( "%lld", num ); return *this; }

	/// Append
    Cpl::Text::String& operator +=( unsigned long num ) { formatAppend( "%lu", num ); return *this; }

	/// Append
    Cpl::Text::String& operator +=( unsigned long long num ) { formatAppend( "%llu", num ); return *this; }
	///@}
};


};      // end namespaces
};
#endif  // end header latch

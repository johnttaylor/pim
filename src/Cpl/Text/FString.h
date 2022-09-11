#ifndef Cpl_Text_FString_h_
#define Cpl_Text_FString_h_
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
	/// Make parent method visible
	using Cpl::Text::String::operator=;

	/// Assignment 
	Cpl::Text::String& operator =( const FString<S>& string ) { copyIn( string, string.length() );return *this; }

public:
	/// Make parent method visible
	using Cpl::Text::String::operator+=;

	/// Append 
	Cpl::Text::String& operator +=( const FString<S>& string ) { appendTo( string, string.length() ); return *this; }

};


};      // end namespaces
};
#endif  // end header latch

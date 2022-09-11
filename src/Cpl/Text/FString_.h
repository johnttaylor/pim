#ifndef Cpl_Text_FString_x_h_
#define Cpl_Text_FString_x_h_
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

#include "colony_config.h"
#include "Cpl/Text/String_.h"

///
namespace Cpl {
///
namespace Text {


/** This mostly concrete class implements a "fixed storage" String Type.
	This class is built on the assumption that "behind-the-scenes"
	dynamic memory allocation is a bad thing (for a long list of reasons)!
	Therefore, NO memory allocation is done in this base class.  The sub-class
	is responsible for allocating ALL of storage used by the FString_.

	NOTE: All methods that "modify" the string  guarantee that the internal
		  storage limit is not exceeded and the string is always NULL
		  terminated.  The "incoming" data is silently truncated if the data
		  size exceeds the storage limit.
 */

class FString_ : public String_
{
protected:
	/// Maximum length of the internal string (not including the null terminator)
	int m_internalMaxlen;

	/// Empty string that is used to return a 'string' when my internal string is not valid
	static char m_noMemory[1];

protected:
	/// Constructor
	FString_( const char* string, char* internalString, int maxLen );

	/// Constructor
	FString_( char c, char* internalString, int maxLen );

	/// Constructor
	FString_( int num, char* internalString, int maxLen );

	/// Constructor
	FString_( unsigned num, char* internalString, int maxLen );

	/// Constructor
	FString_( long num, char* internalString, int maxLen );

	/// Constructor
	FString_( long long num, char* internalString, int maxLen );

	/// Constructor
	FString_( unsigned long num, char* internalString, int maxLen );

	/// Constructor
	FString_( unsigned long long num, char* internalString, int maxLen );


public:
	/// see Cpl::Text::String
	void copyIn( const char* string, int n );

	/// see Cpl::Text::String
	void appendTo( const char* string, int n );

	/// see Cpl::Text::String
	void insertAt( int insertOffset, const char* stringToInsert );

	/// see Cpl::Text::String
	int maxLength() const;

public:
	/// Make parent method visible
	using Cpl::Text::String::operator=;

	///@{
	/// Assignment
	Cpl::Text::String& operator=( int num );

	/// Assignment
	Cpl::Text::String& operator=( unsigned int num );

	/// Assignment
	Cpl::Text::String& operator=( long num );

	/// Assignment
	Cpl::Text::String& operator=( long long num );

	/// Assignment
	Cpl::Text::String& operator=( unsigned long num );

	/// Assignment
	Cpl::Text::String& operator=( unsigned long long num );
	///@}

public:
	/// Make parent method visible
	using Cpl::Text::String::operator+=;

	///@{

	/// Append
	Cpl::Text::String& operator +=( int num );

	/// Append
	Cpl::Text::String& operator +=( unsigned int num );

	/// Append
	Cpl::Text::String& operator +=( long num );

	/// Append
	Cpl::Text::String& operator +=( long long num );

	/// Append
	Cpl::Text::String& operator +=( unsigned long num );

	/// Append
	Cpl::Text::String& operator +=( unsigned long long num );
	///@}
};


};      // end namespaces
};
#endif  // end header latch

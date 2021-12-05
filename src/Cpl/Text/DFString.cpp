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

#include "DFString.h"
#include <string.h>
#include <new>

///
using namespace Cpl::Text;

///
// Helper macro to reduce the clutter on the constructors
#define MEMSIZE calcMemSize(memSize)



////////////////////////////
DFString::DFString( void ) :FString_( (const char*) 0, new( std::nothrow ) char[OPTION_CPL_TEXT_DFSTRING_ALLOC_BLOCK_SIZE], OPTION_CPL_TEXT_DFSTRING_ALLOC_BLOCK_SIZE - 1 ) {}
DFString::DFString( const DFString& string ) : FString_( string.getString(), new( std::nothrow ) char[string.maxLength() + 1], string.maxLength() ) {}
DFString::DFString( const Cpl::Text::String& string ) : FString_( string.getString(), new( std::nothrow ) char[string.maxLength() + 1], string.maxLength() ) {}
DFString::DFString( size_t memSize, const DFString& string ) : FString_( string.getString(), new( std::nothrow ) char[MEMSIZE], MEMSIZE - 1 ) {}
DFString::DFString( size_t memSize, const Cpl::Text::String& string ) : FString_( string.getString(), new( std::nothrow ) char[MEMSIZE], MEMSIZE - 1 ) {}
DFString::DFString( size_t memSize, const char* string ) : FString_( string, new( std::nothrow ) char[MEMSIZE], MEMSIZE - 1 ) {}
DFString::DFString( size_t memSize, char c ) : FString_( c, new( std::nothrow ) char[MEMSIZE], MEMSIZE - 1 ) {}
DFString::DFString( size_t memSize, int num ) : FString_( num, new( std::nothrow ) char[MEMSIZE], MEMSIZE - 1 ) {}
DFString::DFString( size_t memSize, unsigned num ) : FString_( num, new( std::nothrow ) char[MEMSIZE], MEMSIZE - 1 ) {}
DFString::DFString( size_t memSize, long num ) : FString_( num, new( std::nothrow ) char[MEMSIZE], MEMSIZE - 1 ) {}
DFString::DFString( size_t memSize, long long num ) : FString_( num, new( std::nothrow ) char[MEMSIZE], MEMSIZE - 1 ) {}
DFString::DFString( size_t memSize, unsigned long num ) : FString_( num, new( std::nothrow ) char[MEMSIZE], MEMSIZE - 1 ) {}
DFString::DFString( size_t memSize, unsigned long long num ) : FString_( num, new( std::nothrow ) char[MEMSIZE], MEMSIZE - 1 ) {}

DFString::~DFString()
{
	if ( m_strPtr != m_noMemory )
	{
		delete[] m_strPtr; // Pointer is stored in root class
	}
}

////////////////////////////
Cpl::Text::String& DFString::operator=( const String& string ) { copyIn( string, string.length() ); return *this; }
Cpl::Text::String& DFString::operator=( const char* string ) { copyIn( string, string ? strlen( string ) : 0 ); return *this; }
Cpl::Text::String& DFString::operator=( char c ) { copyIn( &c, 1 ); return *this; }
Cpl::Text::String& DFString::operator=( int num ) { format( "%d", num ); return *this; }
Cpl::Text::String& DFString::operator=( unsigned int num ) { format( "%u", num ); return *this; }
Cpl::Text::String& DFString::operator=( long num ) { format( "%ld", num ); return *this; }
Cpl::Text::String& DFString::operator=( long long num ) { format( "%lld", num ); return *this; }
Cpl::Text::String& DFString::operator=( unsigned long num ) { format( "%lu", num ); return *this; }
Cpl::Text::String& DFString::operator=( unsigned long long num ) { format( "%llu", num ); return *this; }
Cpl::Text::String& DFString::operator +=( const String& string ) { appendTo( string, string.length() ); return *this; }
Cpl::Text::String& DFString::operator +=( const char* string ) { appendTo( string, string ? strlen( string ) : 0 ); return *this; }
Cpl::Text::String& DFString::operator +=( char c ) { appendTo( &c, 1 ); return *this; }
Cpl::Text::String& DFString::operator +=( int num ) { formatAppend( "%d", num ); return *this; }
Cpl::Text::String& DFString::operator +=( unsigned int num ) { formatAppend( "%u", num ); return *this; }
Cpl::Text::String& DFString::operator +=( long num ) { formatAppend( "%ld", num ); return *this; }
Cpl::Text::String& DFString::operator +=( long long num ) { formatAppend( "%lld", num ); return *this; }
Cpl::Text::String& DFString::operator +=( unsigned long num ) { formatAppend( "%lu", num ); return *this; }
Cpl::Text::String& DFString::operator +=( unsigned long long num ) { formatAppend( "%llu", num ); return *this; }


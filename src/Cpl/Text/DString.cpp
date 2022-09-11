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

#include "DString.h"
#include "strip.h"
#include "FString.h"
#include <stdlib.h>



///
using namespace Cpl::Text;


// 'ole basic min/max methods
inline static int my_min( int a, int b ) { return a < b ? a : b; }
inline static int my_max( int a, int b ) { return a > b ? a : b; }

///
static const int maxIntegerChars_ = 20; // Large enough to a signed & unsigned 64bit int
static char      noMemory_[1]     = { '\0' };


///////////////////////////////
DString::DString( const Cpl::Text::String& string, int initialSize, int blocksize )
	:String_( 0 ),
	m_blockSize( blocksize ),
	m_storageLen( calcMemSize( my_max( string.length(), initialSize ) ) )
{
	m_strPtr = new( std::nothrow ) char[m_storageLen];
	validateAndCopy( string, string.length() );
}

DString::DString( const DString& string, int initialSize, int blocksize )
	:String_( 0 ),
	m_blockSize( blocksize ),
	m_storageLen( calcMemSize( my_max( string.length(), initialSize ) ) )
{
	m_strPtr = new( std::nothrow ) char[m_storageLen];
	validateAndCopy( string, string.length() );
}

DString::DString( const char* string, int initialSize, int blocksize )
	:String_( 0 ),
	m_blockSize( blocksize ),
	m_storageLen( calcMemSize( my_max( ( string ? strlen( string ) : 1 ), initialSize ) ) )
{
	m_strPtr = new( std::nothrow ) char[m_storageLen];
	validateAndCopy( string, (string ? strlen( string ) : 0) );
}

DString::DString( char c, int initialSize, int blocksize )
	:String_( 0 ),
	m_blockSize( blocksize ),
	m_storageLen( calcMemSize( my_max( sizeof( c ), initialSize ) ) )
{
	m_strPtr = new( std::nothrow ) char[m_storageLen];
	validateAndCopy( &c, 1 );
}

DString::DString( int num, int initialSize, int blocksize )
	:String_( 0 ),
	m_blockSize( blocksize ),
	m_storageLen( calcMemSize( my_max( maxIntegerChars_, initialSize ) ) )
{
	m_strPtr = new( std::nothrow ) char[m_storageLen];
	FString<maxIntegerChars_> string( num );
	validateAndCopy( string, string.length() );
}

DString::DString( unsigned num, int initialSize, int blocksize )
	:String_( 0 ),
	m_blockSize( blocksize ),
	m_storageLen( calcMemSize( my_max( maxIntegerChars_, initialSize ) ) )
{
	m_strPtr = new( std::nothrow ) char[m_storageLen];
	FString<maxIntegerChars_> string( num );
	validateAndCopy( string, string.length() );
}

DString::DString( long num, int initialSize, int blocksize )
	:String_( 0 ),
	m_blockSize( blocksize ),
	m_storageLen( calcMemSize( my_max( maxIntegerChars_, initialSize ) ) )
{
	m_strPtr = new( std::nothrow ) char[m_storageLen];
	FString<maxIntegerChars_> string( num );
	validateAndCopy( string, string.length() );
}

DString::DString( long long num, int initialSize, int blocksize )
	:String_( 0 ),
	m_blockSize( blocksize ),
	m_storageLen( calcMemSize( my_max( maxIntegerChars_, initialSize ) ) )
{
	m_strPtr = new( std::nothrow ) char[m_storageLen];
	FString<maxIntegerChars_> string( num );
	validateAndCopy( string, string.length() );
}


DString::DString( unsigned long num, int initialSize, int blocksize )
	:String_( 0 ),
	m_blockSize( blocksize ),
	m_storageLen( calcMemSize( my_max( maxIntegerChars_, initialSize ) ) )
{
	m_strPtr = new( std::nothrow ) char[m_storageLen];
	FString<maxIntegerChars_> string( num );
	validateAndCopy( string, string.length() );
}

DString::DString( unsigned long long num, int initialSize, int blocksize )
	:String_( 0 ),
	m_blockSize( blocksize ),
	m_storageLen( calcMemSize( my_max( maxIntegerChars_, initialSize ) ) )
{
	m_strPtr = new( std::nothrow ) char[m_storageLen];
	FString<maxIntegerChars_> string( num );
	validateAndCopy( string, string.length() );
}


DString::~DString()
{
	freeCurrentString();
}


///////////////////////////////
void DString::validateAndCopy( const char* string, int len )
{
	// Trap failed memory allocation
	if ( !m_strPtr || m_strPtr == noMemory_ )
	{
		m_strPtr     = noMemory_;
		m_storageLen = 1;
		m_truncated  = true;
	}

	// try to copy the new string value
	else
	{
		// Trap null pointer for 'src' string
		if ( !string )
		{
			m_truncated  = true;
			m_strPtr[0]  = '\0';
		}

		// Everything is good!
		else
		{
			strncpy( m_strPtr, string, len ); // Assumes 'len' has been checked for not overflowing the buffer BEFORE this method is called!
			m_strPtr[len] = '\0';
		}
	}
}


void DString::freeCurrentString( void )
{
	if ( m_strPtr && m_strPtr != noMemory_ )
	{
		delete[] m_strPtr;
		m_strPtr     = noMemory_;
		m_storageLen = 1;
	}
}

///////////////////////////////
void
DString::copyIn( const char* src, int len )
{
	// Allocate new memory if it is needed
	m_truncated = false;
	if ( len > maxStrLen() )
	{
		int   newsize = calcMemSize( len );
		char* ptr     = new( std::nothrow ) char[newsize];
		if ( !ptr )
		{
			m_truncated = true;
			len         = maxStrLen();
		}
		else
		{
			freeCurrentString();
			m_storageLen = newsize;
			m_strPtr     = ptr;
		}
	}

	// Copy the string
	validateAndCopy( src, len );
}

void
DString::appendTo( const char* string, int len )
{
	// Note: Do NOTHING if null string pointer is passed
	if ( string )
	{
		// Allocate new memory if it is needed
		m_truncated  = false;
		int curlen   = strlen( m_strPtr );
		int avail    = maxStrLen() - curlen;
		if ( len > avail )
		{
			int   newsize = calcMemSize( maxStrLen() + len );
			char* ptr     = new( std::nothrow ) char[newsize];
			if ( !ptr )
			{
				m_truncated = true;
				len         = avail;
			}
			else
			{
				strcpy( ptr, m_strPtr );
				freeCurrentString();
				m_storageLen = newsize;
				m_strPtr     = ptr;
			}
		}

		// Append the string (if there is something to append)
		if ( len )
		{
			strncat( m_strPtr, string, len );    // the case of no memory for m_strPtr is handled by 'len' being zero (i.e. this block is skipped)
			m_strPtr[curlen + len] = '\0';
		}
	}
}


void
DString::insertAt( int insertOffset, const char* stringToInsert )
{
	// Note: Do NOTHING if null string pointer is passed
	if ( stringToInsert )
	{
		// Insure offset is NOT negative
		if ( insertOffset < 0 )
		{
			insertOffset = 0;
		}

		// If insertOffset is past the '\0', then simply append stringToInsert
		// Note: This also handles the case of 'noMemory_' since the strlen(noMemory_) will always be zero
		int curlen    = strlen( m_strPtr );
		int insertlen = strlen( stringToInsert );
		if ( insertOffset >= curlen )
		{
			appendTo( stringToInsert, insertlen );
			return;
		}

		// Need more memory -->lets go allocate some
		if ( curlen + insertlen > maxStrLen() )
		{
			int   newsize = calcMemSize( curlen + insertlen );
			char* ptr     = new( std::nothrow ) char[newsize];

			// Failed to get more memory
			if ( !ptr )
			{
				// Insert WITHOUT allocating extra memory
				int avail        = maxStrLen() - insertOffset;      // Amount left in the buffer starting with from insertOffset
				int copylen      = my_min( insertlen, avail );      // Amount of stringToInsert that can be copied in
				int remainderlen = avail - copylen;                 // Amount of room left after stringToInsert has been copied in
				int shiftlen     = curlen - insertOffset;           // Amount of characters needing to be shifted
				int movelen      = my_min( shiftlen, remainderlen );// Amount of characters that can be SAFELY shifted
				if ( movelen > 0 )
				{
					memmove( m_strPtr + insertOffset + copylen, m_strPtr + insertOffset, movelen );
					m_strPtr[insertOffset + copylen + movelen] = '\0';
				}
				memmove( m_strPtr + insertOffset, stringToInsert, copylen );
				m_truncated           = true;
				m_strPtr[maxStrLen()] = '\0';
				return;
			}

			// Got the Extra Memory (yea!)
			else
			{
				strcpy( ptr, m_strPtr );
				freeCurrentString();
				m_storageLen = newsize;
				m_strPtr     = ptr;
			}

		}

		// Enough Room for the new string to fit WITHOUT allocating memory
		int shiftlen = curlen - insertOffset;
		memmove( m_strPtr + insertOffset + insertlen, m_strPtr + insertOffset, shiftlen );
		m_strPtr[insertOffset + insertlen + shiftlen] = '\0';
		memmove( m_strPtr + insertOffset, stringToInsert, insertlen );
		m_truncated = false;
	}
}


Cpl::Text::String& DString::operator =( const DString& string )
{
	copyIn( string, string.length() );
	return *this;
}

Cpl::Text::String& DString::operator =( int num )
{
	FString<maxIntegerChars_> string( num );
	copyIn( string, string.length() );
	return *this;
}

Cpl::Text::String& DString::operator =( unsigned int num )
{
	FString<maxIntegerChars_> string( num );
	copyIn( string, string.length() );
	return *this;
}

Cpl::Text::String& DString::operator =( long num )
{
	FString<maxIntegerChars_> string( num );
	copyIn( string, string.length() );
	return *this;
}

Cpl::Text::String& DString::operator =( long long num )
{
	FString<maxIntegerChars_> string( num );
	copyIn( string, string.length() );
	return *this;
}

Cpl::Text::String& DString::operator =( unsigned long num )
{
	FString<maxIntegerChars_> string( num );
	copyIn( string, string.length() );
	return *this;
}

Cpl::Text::String& DString::operator =( unsigned long long num )
{
	FString<maxIntegerChars_> string( num );
	copyIn( string, string.length() );
	return *this;
}


Cpl::Text::String& DString::operator +=( const DString& string )
{
	appendTo( string, string.length() );
	return *this;
}


Cpl::Text::String& DString::operator +=( int num )
{
	FString<maxIntegerChars_> string( num );
	appendTo( string, string.length() );
	return *this;
}

Cpl::Text::String& DString::operator +=( unsigned int num )
{
	FString<maxIntegerChars_> string( num );
	appendTo( string, string.length() );
	return *this;
}

Cpl::Text::String& DString::operator +=( long num )
{
	FString<maxIntegerChars_> string( num );
	appendTo( string, string.length() );
	return *this;
}

Cpl::Text::String& DString::operator +=( long long num )
{
	FString<maxIntegerChars_> string( num );
	appendTo( string, string.length() );
	return *this;
}

Cpl::Text::String& DString::operator +=( unsigned long num )
{
	FString<maxIntegerChars_> string( num );
	appendTo( string, string.length() );
	return *this;
}

Cpl::Text::String& DString::operator +=( unsigned long long num )
{
	FString<maxIntegerChars_> string( num );
	appendTo( string, string.length() );
	return *this;
}

int DString::maxLength() const
{
	return maxStrLen();
}


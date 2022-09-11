#ifndef Cpl_Text_DFString_h_
#define Cpl_Text_DFString_h_
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
#include "Cpl/Text/FString_.h"


/** This magic constant defines the default block size that the DString class
	uses when allocating memory, i.e. the size of all chunks of memory allocated
	is a multiple of the block size.
 */
#ifndef OPTION_CPL_TEXT_DFSTRING_ALLOC_BLOCK_SIZE
#define OPTION_CPL_TEXT_DFSTRING_ALLOC_BLOCK_SIZE    16
#endif


///
namespace Cpl {
///
namespace Text {


/** This concrete class is a mix of FString and a DString - the
	initial memory is dynamically allocated - but no additional
	memory allocation will occur after the instance is created, i.e.
	it is a 'non-template' version - so to speak - of an FString.

	NOTE: See base class - String - for a complete listing/description of
		  the class's methods.
 */
class DFString : public FString_
{
public:
	/// Constructor.  The 'memsize' defaults to OPTION_CPL_TEXT_DFSTRING_ALLOC_BLOCK_SIZE - 1
	DFString( void );

	/// Constructor.  The new instance uses the 'memSize' of 'string'
	DFString( const DFString& string );

	/// Constructor.  The new instance uses the 'memSize' of 'string'
	DFString( const Cpl::Text::String& string );

	/// Constructor.  'memSize' is the max length of the string storage WITHOUT the null terminator.  If 'memSize' is zero, the storage is defaulted to OPTION_CPL_TEXT_DFSTRING_ALLOC_BLOCK_SIZE - 1. 
	DFString( size_t memSize, const DFString& string );

	/// Constructor.
	DFString( size_t memSize, const Cpl::Text::String& string );

	/// Constructor
	DFString( size_t memSize, const char* string="" );

	/// Constructor
	DFString( size_t memSize, char c );

	/// Constructor
	DFString( size_t memSize, int num );

	/// Constructor
	DFString( size_t memSize, unsigned num );

	/// Constructor
	DFString( size_t memSize, long num );

	/// Constructor
	DFString( size_t memSize, long long num );

	/// Constructor
	DFString( size_t memSize, unsigned long num );

	/// Constructor
	DFString( size_t memSize, unsigned long long num );

public:
	/// Destructor
	~DFString();


public:
	/// Make parent method visible
	using Cpl::Text::String::operator=;

	/// Append
	Cpl::Text::String& operator =( const DFString& string );

public:
	/// Make parent method visible
	using Cpl::Text::String::operator+=;

	/// Append
	Cpl::Text::String& operator +=( const DFString& string );

protected: // Helper methods
	/** Returns the need memory size in "block units".  Note: The size calculation
		includes the memory for the trailing '\0' string terminator.
	 */
	inline int calcMemSize( int len ) { return ( ( len + OPTION_CPL_TEXT_DFSTRING_ALLOC_BLOCK_SIZE ) / OPTION_CPL_TEXT_DFSTRING_ALLOC_BLOCK_SIZE )*OPTION_CPL_TEXT_DFSTRING_ALLOC_BLOCK_SIZE; }

};

};      // end namespaces
};
#endif  // end header 
#ifndef Cpl_Text_DString_h_
#define Cpl_Text_DString_h_
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

#include "Cpl/Text/String_.h"
#include "colony_config.h"
#include <new>

///
namespace Cpl {
///
namespace Text {


/** This magic constant defines the default block size that the DString class
	uses when allocating memory, i.e. the size of all chunks of memory allocated
	is a multiple of the block size.
 */
#ifndef OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE
#define OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE    16
#endif


 /** This concrete class implements a simple "dynamic storage" String Type.
	 All memory is allocated from the heap.  For memory allocation errors,
	 the following happens:
	 1) The _truncated flag is set to true.
	 2) If the error occurred in the constructor, then the internal string
		is set an empty string. If the error occurred because of a requested
		size increase, the internal string is simply truncated.
  */

class DString : public String_
{
protected:
	/// Block size in bytes
	int  m_blockSize;

	/// Size, in bytes, of allocated storage
	int  m_storageLen;


public:
	/** Constructor.  The amount of storage initially allocated for the string is
		the maximum of the size of the source string AND the value of initialSize.
		Whatever value is chosen, it is then rounded up to the nearest block size
		multiple.  There are two main reasons to specify an intialSize. 1) To
		reduce the number of malloc/free that occur over the life of the String.
		If you supply a large enough initialSize - no additional mallocs will be
		needed.  2) If you plan to use the format(..) methods.  The format()
		methods will NOT allocate additional storage.  To prevent the format()
		methods from truncating, you must start with a large enough 'buffer'.
		The block size parameter can be used to control the size of the 'chunks'
		memory is allocated in.  This 'blocking' paradigm helps to reduce
		fragmentation and number of internal malloc/free operations.

		NOTE: The space reserved for the trailing null terminator is part of the
			  block calculation, i.e. if your initial size is 16, and your
			  block size is 16, then number of bytes allocated is 32 to allow
			  space for a 16 byte string and a one byte terminator and then
			  rounded up to the next block size.
	 */
	DString( const String& string, int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE );

	/// Constructor.  See above constructor for details
	DString( const DString& string, int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE );

	/// Constructor.  See above constructor for details
	DString( const char* string="", int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE );

	/// Constructor.  See above constructor for details
	DString( char c, int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE );

	/// Constructor.  See above constructor for details
	DString( int num, int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE );

	/// Constructor.  See above constructor for details
	DString( unsigned num, int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE );

	/// Constructor.  See above constructor for details
	DString( long num, int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE );

	/// Constructor.  See above constructor for details
	DString( long long num, int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE );

	/// Constructor.  See above constructor for details
	DString( unsigned long num, int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE );

	/// Constructor.  See above constructor for details
	DString( unsigned long long num, int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE );

	/// Destructor
	~DString();


public:
    ///@{
    /// Assignment
    Cpl::Text::String & operator =( const DString& string );

	/// Assignment
    Cpl::Text::String& operator =( const Cpl::Text::String& string );

	/// Assignment
    Cpl::Text::String& operator =( const char* string );

	/// Assignment
    Cpl::Text::String& operator =( char c );

	/// Assignment
    Cpl::Text::String& operator =( int num );

	/// Assignment
    Cpl::Text::String& operator =( unsigned int num );

	/// Assignment
    Cpl::Text::String& operator =( long num );

	/// Assignment
    Cpl::Text::String& operator =( long long num );

	/// Assignment
    Cpl::Text::String& operator =( unsigned long num );

	/// Assignment
    Cpl::Text::String& operator =( unsigned long long num );
	///@}

public:
	///@{
	/// Append
    Cpl::Text::String & operator +=( const String& string );

	/// Append
    Cpl::Text::String& operator +=( const char* string );

	/// Append
    Cpl::Text::String& operator +=( char c );

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


public:
	///@{
	/// Override base class
	void copyIn( const char* string, int n );

	/// Override base class
	void appendTo( const char* string, int n );

	/// Override base class
	void insertAt( int insertOffset, const char* stringToInsert );

	/// Override base class
	int  maxLength() const;
	///@}


protected: // Helper methods
	/** Returns the need memory size in "block units".  Note: The size calculation
		includes the memory for the trailing '\0' string terminator.
	 */
	inline int calcMemSize( int len ) { return ( ( len + m_blockSize ) / m_blockSize )*m_blockSize; }

	/// Frees the current string memory - IF it was previously allocated
	void freeCurrentString( void );

	/// Returns the max length of internal WITHOUT the '\0' string terminator
	inline int maxStrLen( void ) const { return m_storageLen - 1; }

	/// Validates the just created string is 'valid'    
	void validateAndCopy( const char* string, int len );
};


};      // end namespaces
};
#endif  // end header latch

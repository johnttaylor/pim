#ifndef Cpl_Text_DStringItem_h_
#define Cpl_Text_DStringItem_h_
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

#include "Cpl/Text/StringItem.h"
#include "Cpl/Text/DString.h"


///
namespace Cpl {
///
namespace Text {



/** This concrete class implements the StringItem class for a 'String' using
	a 'DString' as the internal storage for String instance.
 */
class DStringItem : public StringItem
{
protected:
	/// Underlying String
	DString m_internalString;

public:
	/// Constructor
	DStringItem( const String& string, int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) :StringItem( m_internalString ), m_internalString( string, initialSize, blocksize ) {}

	/// Constructor
	DStringItem( const char* string="", int initialSize=0, int blocksize=OPTION_CPL_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) :StringItem( m_internalString ), m_internalString( string, initialSize, blocksize ) {}


private:
	/// Prevent access to the copy constructor -->Container Items should not be copied!
	DStringItem( const DStringItem& m );

	/// Prevent access to the assignment operator -->Container Items should not be copied!
	const DStringItem& operator=( const DStringItem& m );

};


};      // end namespaces
};
#endif  // end header latch

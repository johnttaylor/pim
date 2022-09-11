#ifndef Cpl_Text_DFStringItem_h_
#define Cpl_Text_DFStringItem_h_
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

#include "Cpl/Text/StringItem.h"
#include "Cpl/Text/DFString.h"


///
namespace Cpl {
///
namespace Text {



/** This concrete class implements the StringItem class for a 'String' using
	a 'DFString' as the internal storage for String instance.
 */
class DFStringItem : public StringItem
{
protected:
	/// Underlying String
	DFString m_internalString;

public:
	/// Constructor.  The new instance uses the 'memSize' of 'string'
	DFStringItem( const Cpl::Text::String& string ) :StringItem( m_internalString ), m_internalString( string ) {}

	/// Constructor.  'memSize' is the max length of the string storage WITHOUT the null terminator.  If 'memSize' is zero, the storage is defaulted to OPTION_CPL_TEXT_DFSTRING_ALLOC_BLOCK_SIZE - 1. 
	DFStringItem( size_t memSize, const char* string="" ) :StringItem( m_internalString ), m_internalString( memSize, string ) {}

	/// Constructor.
	DFStringItem( size_t memSize, const Cpl::Text::String& string ) :StringItem( m_internalString ), m_internalString( memSize, string ) {}


private:
	/// Prevent access to the copy constructor -->Container Items should not be copied!
	DFStringItem( const DFStringItem& m );

	/// Prevent access to the assignment operator -->Container Items should not be copied!
	const DFStringItem& operator=( const DFStringItem& m );

};


};      // end namespaces
};
#endif  // end header latch

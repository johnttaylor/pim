#ifndef Cpl_Text_FStringItem_h_
#define Cpl_Text_FStringItem_h_
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
#include "Cpl/Text/FString.h"


///
namespace Cpl {
///
namespace Text {



/** This template class implements the StringItem class for a 'String' using
	a 'FString' as the internal storage for String instance.

	Template Args:  S:=  Max Size of the String WITHOUT the null
						 terminator!
 */
template <int S>
class FStringItem : public StringItem
{
protected:
	/// Underlying String
	FString<S> m_internalString;

public:
	/// Constructor
	FStringItem( const Cpl::Text::String& string ) :StringItem( m_internalString ), m_internalString( string ) {}

	/// Constructor
	FStringItem( const char* string="" ) :StringItem( m_internalString ), m_internalString( string ) {}


private:
	/// Prevent access to the copy constructor -->Container Items should not be copied!
	FStringItem( const FStringItem& m );

	/// Prevent access to the assignment operator -->Container Items should not be copied!
	const FStringItem& operator=( const FStringItem& m );

};


};      // end namespaces
};
#endif  // end header latch

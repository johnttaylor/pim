#ifndef Cpl_Text_StringItem_h_
#define Cpl_Text_StringItem_h_
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

#include "Cpl/Container/MapItem.h"
#include "Cpl/Text/String.h"


///
namespace Cpl {
///
namespace Text {



/** This partially concrete provides a wrapper class that allows a String
	object to be stored directly in one the CPL Containers.  A StringItem
	can be stored in a SList, DList, Dictionary, and Map.

	Notes:

		o The allocation of the actual String is deferred to child classes.
		o When storing a StringItem in a 'keyed' container (i.e. Dictionary
		  and Map) the application MUST NOT change the value of the internal
		  string since it is the 'key' value that is used to determine its
		  placement in the Dictionary/Map. Modifying the internal String while
		  the StringItem is in a Dictionary/Map will CORRUPT the Container.
		o StringItem instances CANNOT be copied.  This is to avoid potential
		  Container corruption since the individual Items contain the memory
		  for the Containers' linkage, i.e. a copied container Item is
		  analogous to storing a single Item in more than one Container at the
		  same time.
 */
class StringItem : public Cpl::Container::MapItem
{
protected:
	/// Constructor
	StringItem( String& string ) :m_myString( string ) {}

public:
	/// Returns a reference to the actual String
	inline String& get() const { return m_myString; }

	/// Returns a pointer to the internal C string
	inline const char* getString() const { return m_myString.getString(); }

	/// Cast to a String reference
	inline operator String& ( ) const { return m_myString; }

	/// Cast to read-only character string pointer.
	inline operator const char* ( ) const { return m_myString.getString(); }

	/// Returns a Read-only pointer to the "raw" (short-hand for getString())
	inline const char* operator()() const { return m_myString.getString(); }


public:
	/// Return Cpl::Container::DictItem (used with Maps and Dictionaries)
	const Cpl::Container::Key& getKey() const noexcept { return m_myString; }


protected:
	/// Reference to the actual String/storage
	String & m_myString;


private:
	/// Prevent access to the copy constructor -->Container Items should not be copied!
	StringItem( const StringItem& m );

	/// Prevent access to the assignment operator -->Container Items should not be copied!
	const StringItem& operator=( const StringItem& m );
};


};      // end namespaces
};
#endif  // end header latch

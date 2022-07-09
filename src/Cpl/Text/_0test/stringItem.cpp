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

#include "Catch/catch.hpp"
#include "Cpl/Container/MapItem.h"    
#include "Cpl/Container/SList.h"    
#include "Cpl/Container/DList.h"    
#include "Cpl/Container/Dictionary.h"    
#include "Cpl/Container/Map.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FStringItem.h"
#include "Cpl/Text/DFStringItem.h"
#include "Cpl/Text/DStringItem.h"

#include <stdio.h>

/// 
using namespace Cpl::Container;
using namespace Cpl::System;
using namespace Cpl::Text;


/// Short hand for brute forcing string compares when not using std::string
#define STRING_EQ(a,b)  strcmp(a,b) == 0


////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "StringItem", "[stringItem]" )
{
	Map<StringItem> map;
	FStringItem<15> apple( "apple" );
	FStringItem<15> orange( "orange" );
	FStringItem<15> cherry( "cherry" );
	FStringItem<15> pear( "pear" );
	FStringItem<15> plum( "plum" );
	StringItem*     ptr1;

	Shutdown_TS::clearAndUseCounter();

	SECTION( "Validate that an 'item' can be only in one Container" )
	{
		Map<StringItem> foo;
		Map<StringItem> bar;

		FStringItem<4> item( "bob" );

		foo.insert( item );
		bar.insert( item );

		REQUIRE( Shutdown_TS::getAndClearCounter() == 1u );
	}



	SECTION( "StingItem methods" )
	{
		String& myStringRef = apple.get();
		REQUIRE( myStringRef == "apple" );

		const char* ptr = plum.getString();
		REQUIRE( STRING_EQ( ptr, "plum" ) );

		ptr = (const char*) orange;
		REQUIRE( STRING_EQ( ptr, "orange" ) );

		String& myStringRef2 = (String&) pear;
		REQUIRE( myStringRef2 == "pear" );

		REQUIRE( STRING_EQ( cherry(), "cherry" ) );
	}


	SECTION( "MAP" )
	{
		map.insert( orange );
		map.insert( plum );
		map.insert( pear );
		map.insert( cherry );
		map.insert( apple );

		KeyLiteralString key1( "Hello" );
		ptr1 = map.find( key1 );
		REQUIRE( ptr1 == 0 );

		KeyLiteralString key2( "orange" );
		ptr1 = map.find( key2 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );

		ptr1 = map.last();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "plum" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "pear" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "cherry" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "apple" );
		REQUIRE( map.previous( *ptr1 ) == 0 );

		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "cherry" );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "pear" );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "plum" );
		REQUIRE( map.next( *ptr1 ) == 0 );

		bool result = map.removeItem( orange );
		REQUIRE( result == true );
		REQUIRE( map.removeItem( orange ) == false );
		REQUIRE( map.remove( key2 ) == 0 );
		map.insert( orange );
		ptr1 = map.remove( key2 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );

		Map<StringItem> dst;
		REQUIRE( dst.first() == 0 );
		REQUIRE( map.first() != 0 );
		map.move( dst );
		REQUIRE( dst.first() != 0 );
		REQUIRE( map.first() == 0 );
		ptr1 = dst.last();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "plum" );
		dst.clearTheMap();
		REQUIRE( dst.first() == 0 );
	}

	// 
	// Verify StringItem works in a Dictionary
	//
	SECTION( "DICT" )
	{
		static DList<DictItem> buckets_3[3];
		Dictionary<StringItem> dict( buckets_3, 3 );
		REQUIRE( dict.getFirst() == 0 );
		REQUIRE( dict.first() == 0 );

		dict.insert( apple );
		dict.insert( orange );
		dict.insert( cherry );

		int not_found  = 0;
		int found_apple  = 0;
		int found_orange = 0;
		int found_cherry = 0;
		ptr1 = dict.getFirst();
		REQUIRE( ptr1 != 0 );
		while ( ptr1 )
		{
			if ( ptr1->get() == "apple" )
			{
				found_apple++;
			}
			else if ( ptr1->get() == "orange" )
			{
				found_orange++;
			}
			else if ( ptr1->get() == "cherry" )
			{
				found_cherry++;
			}
			else
			{
				not_found++;
			}

			ptr1 = dict.getFirst();
		}
		REQUIRE( found_apple == 1 );
		REQUIRE( found_orange == 1 );
		REQUIRE( found_cherry == 1 );
		REQUIRE( not_found == 0 );
		REQUIRE( dict.first() == 0 );
	}


	// 
	// Verify MapItem works in a DList
	//
	SECTION( "DLIST" )
	{
		DList<StringItem> dlist;

		REQUIRE( dlist.get() == 0 );
		REQUIRE( dlist.head() == 0 );
		REQUIRE( dlist.tail() == 0 );

		dlist.put( apple );

		REQUIRE( dlist.head() != 0 );
		REQUIRE( dlist.head()->get() == "apple" );
		REQUIRE( dlist.tail() != 0 );
		REQUIRE( dlist.tail()->get() == "apple" );

		dlist.put( orange );

		REQUIRE( dlist.head() != 0 );
		REQUIRE( dlist.head()->get() == "apple" );
		REQUIRE( dlist.tail() != 0 );
		REQUIRE( dlist.tail()->get() == "orange" );

		dlist.put( cherry );

		REQUIRE( dlist.head() != 0 );
		REQUIRE( dlist.head()->get() == "apple" );
		REQUIRE( dlist.tail() != 0 );
		REQUIRE( dlist.tail()->get() == "cherry" );

		ptr1 = dlist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "apple" );
		REQUIRE( dlist.head() != 0 );
		REQUIRE( dlist.head()->get() == "orange" );
		REQUIRE( dlist.tail() != 0 );
		REQUIRE( dlist.tail()->get() == "cherry" );

		ptr1 = dlist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );
		REQUIRE( dlist.head() != 0 );
		REQUIRE( dlist.head()->get() == "cherry" );
		REQUIRE( dlist.tail() != 0 );
		REQUIRE( dlist.tail()->get() == "cherry" );

		ptr1 = dlist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "cherry" );
		REQUIRE( dlist.head() == 0 );
		REQUIRE( dlist.tail() == 0 );

		REQUIRE( dlist.get() == 0 );
		REQUIRE( dlist.head() == 0 );
		REQUIRE( dlist.tail() == 0 );
	}

	// 
	// Verify MapItem works in a DList
	//
	SECTION( "SLIST" )
	{
		SList<StringItem> slist;

		REQUIRE( slist.get() == 0 );
		REQUIRE( slist.head() == 0 );
		REQUIRE( slist.tail() == 0 );

		slist.put( apple );

		REQUIRE( slist.head() != 0 );
		REQUIRE( slist.head()->get() == "apple" );
		REQUIRE( slist.tail() != 0 );
		REQUIRE( slist.tail()->get() == "apple" );

		slist.put( orange );

		REQUIRE( slist.head() != 0 );
		REQUIRE( slist.head()->get() == "apple" );
		REQUIRE( slist.tail() != 0 );
		REQUIRE( slist.tail()->get() == "orange" );

		slist.put( cherry );

		REQUIRE( slist.head() != 0 );
		REQUIRE( slist.head()->get() == "apple" );
		REQUIRE( slist.tail() != 0 );
		REQUIRE( slist.tail()->get() == "cherry" );

		ptr1 = slist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "apple" );
		REQUIRE( slist.head() != 0 );
		REQUIRE( slist.head()->get() == "orange" );
		REQUIRE( slist.tail() != 0 );
		REQUIRE( slist.tail()->get() == "cherry" );

		ptr1 = slist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );
		REQUIRE( slist.head() != 0 );
		REQUIRE( slist.head()->get() == "cherry" );
		REQUIRE( slist.tail() != 0 );
		REQUIRE( slist.tail()->get() == "cherry" );

		ptr1 = slist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "cherry" );
		REQUIRE( slist.head() == 0 );
		REQUIRE( slist.tail() == 0 );

		REQUIRE( slist.get() == 0 );
		REQUIRE( slist.head() == 0 );
		REQUIRE( slist.tail() == 0 );
	}

	SECTION( "MAP: DFString" )
	{
		DFStringItem apple( 6, "apple" );
		DFStringItem orange( 6, "orange" );
		DFStringItem cherry( 6, "cherry" );
		DFStringItem pear( 6, "pear" );
		DFStringItem plum( 6, "plum" );

		map.insert( orange );
		map.insert( plum );
		map.insert( pear );
		map.insert( cherry );
		map.insert( apple );

		KeyLiteralString key1( "Hello" );
		ptr1 = map.find( key1 );
		REQUIRE( ptr1 == 0 );

		KeyLiteralString key2( "orange" );
		ptr1 = map.find( key2 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );

		ptr1 = map.last();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "plum" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "pear" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "cherry" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "apple" );
		REQUIRE( map.previous( *ptr1 ) == 0 );

		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "cherry" );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "pear" );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "plum" );
		REQUIRE( map.next( *ptr1 ) == 0 );

		bool result = map.removeItem( orange );
		REQUIRE( result == true );
		REQUIRE( map.removeItem( orange ) == false );
		REQUIRE( map.remove( key2 ) == 0 );
		map.insert( orange );
		ptr1 = map.remove( key2 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );

		Map<StringItem> dst;
		REQUIRE( dst.first() == 0 );
		REQUIRE( map.first() != 0 );
		map.move( dst );
		REQUIRE( dst.first() != 0 );
		REQUIRE( map.first() == 0 );
		ptr1 = dst.last();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "plum" );
		dst.clearTheMap();
		REQUIRE( dst.first() == 0 );
	}

	SECTION( "MAP: DString" )
	{
		DStringItem pear1( "pear1" );
		DStringItem apple( "apple" );
		DStringItem orange( "orange" );
		DStringItem cherry( "cherry" );
		DStringItem pear( "pear" );


		REQUIRE( map.insert( orange ) == true );
		REQUIRE( map.insert( pear1 ) == true );
		REQUIRE( map.insert( pear ) == true );
		REQUIRE( map.insert( cherry ) == true );
		REQUIRE( map.insert( apple ) == true );
		REQUIRE( map.insert( orange ) == false );

		KeyLiteralString key1( "Hello" );
		ptr1 = map.find( key1 );
		REQUIRE( ptr1 == 0 );

		KeyLiteralString key2( "orange" );
		ptr1 = map.find( key2 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );

		ptr1 = map.last();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "pear1" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "pear" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "cherry" );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "apple" );
		REQUIRE( map.previous( *ptr1 ) == 0 );

		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "cherry" );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "pear" );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "pear1" );
		REQUIRE( map.next( *ptr1 ) == 0 );

		bool result = map.removeItem( orange );
		REQUIRE( result == true );
		REQUIRE( map.removeItem( orange ) == false );
		REQUIRE( map.remove( key2 ) == 0 );
		map.insert( orange );
		ptr1 = map.remove( key2 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "orange" );

		Map<StringItem> dst;
		REQUIRE( dst.first() == 0 );
		REQUIRE( map.first() != 0 );
		map.move( dst );
		REQUIRE( dst.first() != 0 );
		REQUIRE( map.first() == 0 );
		ptr1 = dst.last();
		REQUIRE( ptr1 != 0 );
		REQUIRE( ptr1->get() == "pear1" );
		dst.clearTheMap();
		REQUIRE( dst.first() == 0 );
	}

	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}

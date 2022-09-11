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

#include "Catch/catch.hpp"
#include "Cpl/Container/MapItem.h"    
#include "Cpl/Container/SList.h"    
#include "Cpl/Container/DList.h"    
#include "Cpl/Container/Dictionary.h"    
#include "Cpl/Container/Map.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>

#include <stdio.h>

/// 
using namespace Cpl::Container;
using namespace Cpl::System;


/// Short hand for brute forcing string compares when not using std::string
#define STRING_EQ(a,b)  strcmp(a,b) == 0


////////////////////////////////////////////////////////////////////////////////

/// Use un-named namespace to make my class local-to-the-file in scope
namespace {


class MyItemMap : public MapItem,
	public KeyLiteralString
{
public:
	///
	MyItemMap( const char* name )
		:KeyLiteralString( name )
	{
	}

public:
	///  API from SortedItem
	const Key& getKey() const noexcept { return *this; }

};

void printMap( Map<MyItemMap>& map )
{
	printf( "Map (%p)\n", &map );
	MyItemMap* ptr = map.first();
	while ( ptr )
	{
		printf( "[%s]\n", ptr->getKeyValue() );
		ptr = map.next( *ptr );
	}

}

class ItemAutoAdd : public MapItem,
	public KeyInteger16_T
{
public:
	///
	ItemAutoAdd( Map<ItemAutoAdd>& list, const char* name, int16_t keyValue=0 )
		:KeyInteger16_T( keyValue ),
		m_name( name )
	{
		list.insert( *this );
	}

	///
	const char* m_name;

public:
	///  API from SortedItem
	const Key& getKey() const noexcept { return *this; }
};



}; // end namespace

////////////////////////////////////////////////////////////////////////////////
static Map<ItemAutoAdd> emptymap_( "static constructor" );
static Map<ItemAutoAdd> staticmap_( "static constructor" );
static ItemAutoAdd      static1Item_A( staticmap_, "staticItemA (-2)", -2 );
static ItemAutoAdd      static1Item_B( staticmap_, "staticItemB (6)", 6 );
static ItemAutoAdd      static1Item_C( staticmap_, "staticItemV (-3)", -3 );




////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "MAP: Validate member functions", "[map]" )
{
	Map<MyItemMap> map;
	MyItemMap      apple( "apple" );
	MyItemMap      orange( "orange" );
	MyItemMap      cherry( "cherry" );
	MyItemMap      pear( "pear" );
	MyItemMap      plum( "plum" );
	MyItemMap*     ptr1;

	Shutdown_TS::clearAndUseCounter();

	SECTION( "Validate that an 'item' can be only in one Container" )
	{
		Map<MyItemMap> foo;
		Map<MyItemMap> bar;

		MyItemMap item( "bob" );

		foo.insert( item );
		bar.insert( item );

		REQUIRE( Shutdown_TS::getAndClearCounter() == 1u );
	}

	SECTION( "Validate static Constructor" )
	{
		REQUIRE( emptymap_.first() == 0 );
		REQUIRE( emptymap_.last() == 0 );

		ItemAutoAdd* ptr2 = staticmap_.find( static1Item_A );
		REQUIRE( ptr2 != 0 );
		REQUIRE( ptr2->getKeyValue() == -2 );

		ptr2 = staticmap_.first();
		REQUIRE( ptr2 != 0 );
		REQUIRE( ptr2->getKeyValue() == -3 );
		ptr2 = staticmap_.next( *ptr2 );
		REQUIRE( ptr2 != 0 );
		REQUIRE( ptr2->getKeyValue() == -2 );
		ptr2 = staticmap_.next( *ptr2 );
		REQUIRE( ptr2 != 0 );
		REQUIRE( ptr2->getKeyValue() == 6 );
		ptr2 = staticmap_.next( *ptr2 );
		REQUIRE( ptr2 == 0 );

		// Look-up by key
		KeyInteger16_T key6  = 6;
		KeyInteger16_T key0  = 0;
		KeyInteger16_T key_2 = -2;

		ptr2 = staticmap_.find( key6 );
		REQUIRE( ptr2 != 0 );
		REQUIRE( ptr2->getKeyValue() == 6 );
		ptr2 = staticmap_.find( key0 );
		REQUIRE( ptr2 == 0 );
		ptr2 = staticmap_.find( key_2 );
		REQUIRE( ptr2 != 0 );
		REQUIRE( ptr2->getKeyValue() == -2 );


		// Sneak in test for bogus keys
		ptr2 = staticmap_.find( cherry );
		REQUIRE( Shutdown_TS::getAndClearCounter() >= 1u ); // Note: Since the behavior is undefined when FatalError::logf() returns -->I will hit more than one fatal error, how many more is somewhat 'random'
	}

	SECTION( "Basic" )
	{
		REQUIRE( map.getFirst() == 0 );
		REQUIRE( map.first() == 0 );
		REQUIRE( map.last() == 0 );

		map.insert( apple );

		REQUIRE( map.first() != 0 );
		REQUIRE( STRING_EQ( map.first()->m_stringKeyPtr, "apple" ) );
		REQUIRE( map.last() != 0 );
		REQUIRE( STRING_EQ( map.last()->m_stringKeyPtr, "apple" ) );

		map.insert( orange );

		REQUIRE( map.first() != 0 );
		REQUIRE( STRING_EQ( map.first()->m_stringKeyPtr, "apple" ) );
		REQUIRE( map.last() != 0 );
		REQUIRE( STRING_EQ( map.last()->m_stringKeyPtr, "orange" ) );

		map.insert( cherry );

		REQUIRE( map.first() != 0 );
		REQUIRE( STRING_EQ( map.first()->m_stringKeyPtr, "apple" ) );
		REQUIRE( map.last() != 0 );
		REQUIRE( STRING_EQ( map.last()->m_stringKeyPtr, "orange" ) );

		ptr1 = map.getLast();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
		REQUIRE( map.first() != 0 );
		REQUIRE( STRING_EQ( map.first()->m_stringKeyPtr, "apple" ) );
		REQUIRE( map.last() != 0 );
		REQUIRE( STRING_EQ( map.last()->m_stringKeyPtr, "cherry" ) );

		ptr1 = map.getFirst();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "apple" ) );
		REQUIRE( map.first() != 0 );
		REQUIRE( STRING_EQ( map.first()->m_stringKeyPtr, "cherry" ) );
		REQUIRE( map.last() != 0 );
		REQUIRE( STRING_EQ( map.last()->m_stringKeyPtr, "cherry" ) );

		ptr1 = map.getFirst();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
		REQUIRE( map.first() == 0 );
		REQUIRE( map.last() == 0 );

		REQUIRE( map.first() == 0 );
		REQUIRE( map.last() == 0 );
		REQUIRE( map.getFirst() == 0 );
		REQUIRE( map.getLast() == 0 );
	}

	SECTION( "More Basic ops" )
	{
		MyItemMap pear1( "pear1" );

		REQUIRE( map.insert( orange ) == true );
		REQUIRE( map.insert( pear1 ) == true );
		REQUIRE( map.insert( pear ) == true );
		REQUIRE( map.insert( cherry ) == true );
		REQUIRE( map.insert( apple ) == true );
		REQUIRE( map.insert( orange ) == false );

		printMap( map );

		KeyLiteralString key1( "Hello" );
		MyItemMap* ptr1 = map.find( key1 );
		REQUIRE( ptr1 == 0 );

		KeyLiteralString key2( "orange" );
		ptr1 = map.find( key2 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );

		ptr1 = map.last();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "pear1" ) );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "pear" ) );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
		ptr1 = map.previous( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "apple" ) );
		REQUIRE( map.previous( *ptr1 ) == 0 );

		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "pear" ) );
		ptr1 = map.next( *ptr1 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "pear1" ) );
		REQUIRE( map.next( *ptr1 ) == 0 );

		bool result = map.removeItem( orange );
		REQUIRE( result == true );
		REQUIRE( map.removeItem( orange ) == false );
		REQUIRE( map.remove( key2 ) == 0 );
		map.insert( orange );
		ptr1 = map.remove( key2 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );

		Map<MyItemMap> dst;
		REQUIRE( dst.first() == 0 );
		REQUIRE( map.first() != 0 );
		map.move( dst );
		REQUIRE( dst.first() != 0 );
		REQUIRE( map.first() == 0 );
		ptr1 = dst.last();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "pear1" ) );
		dst.clearTheMap();
		REQUIRE( dst.first() == 0 );
	}


	// 
	// Verify MapItem works in a Dictionary
	//
	SECTION( "DICT: Basic" )
	{
		static DList<DictItem> buckets_3[3];
		Dictionary<MyItemMap> dict( buckets_3, 3 );
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
			if ( STRING_EQ( ptr1->m_stringKeyPtr, "apple" ) )
			{
				found_apple++;
			}
			else if ( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) )
			{
				found_orange++;
			}
			else if ( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) )
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
	SECTION( "DLIST: FIFO with MapItem" )
	{
		DList<MyItemMap> dlist;

		REQUIRE( dlist.get() == 0 );
		REQUIRE( dlist.head() == 0 );
		REQUIRE( dlist.tail() == 0 );

		dlist.put( apple );

		REQUIRE( dlist.head() != 0 );
		REQUIRE( STRING_EQ( dlist.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( dlist.tail() != 0 );
		REQUIRE( STRING_EQ( dlist.tail()->m_stringKeyPtr, "apple" ) );

		dlist.put( orange );

		REQUIRE( dlist.head() != 0 );
		REQUIRE( STRING_EQ( dlist.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( dlist.tail() != 0 );
		REQUIRE( STRING_EQ( dlist.tail()->m_stringKeyPtr, "orange" ) );

		dlist.put( cherry );

		REQUIRE( dlist.head() != 0 );
		REQUIRE( STRING_EQ( dlist.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( dlist.tail() != 0 );
		REQUIRE( STRING_EQ( dlist.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = dlist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "apple" ) );
		REQUIRE( dlist.head() != 0 );
		REQUIRE( STRING_EQ( dlist.head()->m_stringKeyPtr, "orange" ) );
		REQUIRE( dlist.tail() != 0 );
		REQUIRE( STRING_EQ( dlist.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = dlist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
		REQUIRE( dlist.head() != 0 );
		REQUIRE( STRING_EQ( dlist.head()->m_stringKeyPtr, "cherry" ) );
		REQUIRE( dlist.tail() != 0 );
		REQUIRE( STRING_EQ( dlist.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = dlist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
		REQUIRE( dlist.head() == 0 );
		REQUIRE( dlist.tail() == 0 );

		REQUIRE( dlist.get() == 0 );
		REQUIRE( dlist.head() == 0 );
		REQUIRE( dlist.tail() == 0 );
	}

	// 
	// Verify MapItem works in a DList
	//
	SECTION( "SLIST: FIFO with MapItem" )
	{
		SList<MyItemMap> slist;

		REQUIRE( slist.get() == 0 );
		REQUIRE( slist.head() == 0 );
		REQUIRE( slist.tail() == 0 );

		slist.put( apple );

		REQUIRE( slist.head() != 0 );
		REQUIRE( STRING_EQ( slist.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( slist.tail() != 0 );
		REQUIRE( STRING_EQ( slist.tail()->m_stringKeyPtr, "apple" ) );

		slist.put( orange );

		REQUIRE( slist.head() != 0 );
		REQUIRE( STRING_EQ( slist.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( slist.tail() != 0 );
		REQUIRE( STRING_EQ( slist.tail()->m_stringKeyPtr, "orange" ) );

		slist.put( cherry );

		REQUIRE( slist.head() != 0 );
		REQUIRE( STRING_EQ( slist.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( slist.tail() != 0 );
		REQUIRE( STRING_EQ( slist.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = slist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "apple" ) );
		REQUIRE( slist.head() != 0 );
		REQUIRE( STRING_EQ( slist.head()->m_stringKeyPtr, "orange" ) );
		REQUIRE( slist.tail() != 0 );
		REQUIRE( STRING_EQ( slist.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = slist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
		REQUIRE( slist.head() != 0 );
		REQUIRE( STRING_EQ( slist.head()->m_stringKeyPtr, "cherry" ) );
		REQUIRE( slist.tail() != 0 );
		REQUIRE( STRING_EQ( slist.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = slist.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
		REQUIRE( slist.head() == 0 );
		REQUIRE( slist.tail() == 0 );

		REQUIRE( slist.get() == 0 );
		REQUIRE( slist.head() == 0 );
		REQUIRE( slist.tail() == 0 );
	}


	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}

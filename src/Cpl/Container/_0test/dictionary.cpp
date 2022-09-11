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
#include "Cpl/Container/DictItem.h"
#include "Cpl/Container/SList.h"    
#include "Cpl/Container/DList.h"    
#include "Cpl/Container/Dictionary.h"    
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


class MyItem : public DictItem,
	public KeyLiteralString
{
public:
	///
	MyItem( const char* name )
		:KeyLiteralString( name )
	{
	}

public:
	///  API from SortedItem
	const Key& getKey() const noexcept { return *this; }

};

class ItemIntKey : public DictItem,
	public KeyInteger16_T
{
public:
	///
	ItemIntKey( const char* name, int16_t keyValue=0 )
		:KeyInteger16_T( keyValue ),
		m_name( name )
	{
	}

	///
	const char* m_name;

public:
	///  API from SortedItem
	const Key& getKey() const noexcept { return *this; }
};


class Stupid : public HashFunction
{
public:
	/// Generates a Hash Index (0 to maxBuckets-1) based on the specified key
	unsigned int hashKey( const void* keystart, unsigned keylen, unsigned int maxBuckets ) const noexcept
	{
		return 0;
	}
};


}; // end namespace

////////////////////////////////////////////////////////////////////////////////
static DList<DictItem> buckets_10[10];
static DList<DictItem> buckets_3[3];
static DList<DictItem> buckets_5[5];

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "DICTIONARY: Validate member functions", "[dict]" )
{
	Dictionary<MyItem> dict( buckets_3, 3 );
	MyItem             apple( "apple" );
	MyItem             orange( "orange" );
	MyItem             cherry( "cherry" );
	MyItem             pear( "pear" );
	MyItem             plum( "plum" );
	MyItem             bob( "Bob's your uncle with a very long key and lots of unnecessary stuff goes from here." );
	MyItem*            ptr1;

	Shutdown_TS::clearAndUseCounter();

	SECTION( "Validate that an 'item' can be only in one Container" )
	{
		Dictionary<ItemIntKey> foo( buckets_10, 10 );
		Dictionary<ItemIntKey> bar( buckets_5, 5 );

		ItemIntKey item( "bob" );

		foo.insert( item );
		bar.insert( item );

		REQUIRE( Shutdown_TS::getAndClearCounter() == 1u );
	}

	SECTION( "User supplied hash function" )
	{
		DList<DictItem>    buckets_5[5];
		HashTable_::Stats   info;
		Stupid             myHashFunction;
		Dictionary<MyItem> dict( buckets_5, 5, myHashFunction );

		dict.insert( orange );
		dict.insert( plum );
		dict.insert( pear );
		dict.insert( cherry );
		dict.insert( apple );

		dict.stats( info );
		printf( "TableStats: items=%lu, buckets=%u, emptyBuckets=%u, averagePerBucket=%f, maxItemPerBucket=%lu\n",
			info.m_numItems,
			info.m_numBuckets,
			info.m_numEmptyBuckets,
			info.m_average_itemsPerBucket,
			info.m_max_itemsPerBucket );
		REQUIRE( info.m_numItems == 5 );
		REQUIRE( info.m_numBuckets == 5 );
		REQUIRE( info.m_numEmptyBuckets == 4 );
		REQUIRE( info.m_max_itemsPerBucket == 5 );

		// Clear the dictionary so I can re-use my fruit :)
		dict.clearTheDictionary();
	}

	SECTION( "Basic" )
	{
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

	SECTION( "More Basic ops" )
	{
		HashTable_::Stats info;
		dict.stats( info );
		REQUIRE( info.m_numItems == 0 );
		REQUIRE( info.m_numBuckets == 3 );
		REQUIRE( info.m_numEmptyBuckets == 3 );
		REQUIRE( info.m_max_itemsPerBucket == 0 );
		printf( "TableStats: items=%lu, buckets=%u, emptyBuckets=%u, averagePerBucket=%f, maxItemPerBucket=%lu\n",
			info.m_numItems,
			info.m_numBuckets,
			info.m_numEmptyBuckets,
			info.m_average_itemsPerBucket,
			info.m_max_itemsPerBucket );

		dict.insert( orange );
		dict.insert( plum );
		dict.insert( pear );
		dict.insert( cherry );
		dict.insert( apple );

		dict.stats( info );
		REQUIRE( info.m_numItems == 5 );
		REQUIRE( info.m_numBuckets == 3 );
		REQUIRE( info.m_numEmptyBuckets == 0 );
		REQUIRE( info.m_max_itemsPerBucket == 3 );
		printf( "TableStats: items=%lu, buckets=%u, emptyBuckets=%u, averagePerBucket=%f, maxItemPerBucket=%lu\n",
			info.m_numItems,
			info.m_numBuckets,
			info.m_numEmptyBuckets,
			info.m_average_itemsPerBucket,
			info.m_max_itemsPerBucket );

		KeyLiteralString key1( "Hello" );
		MyItem* ptr1 = dict.find( key1 );
		REQUIRE( ptr1 == 0 );

		KeyLiteralString key2( "orange" );
		ptr1 = dict.find( key2 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );

		int not_found  = 0;
		int found_apple  = 0;
		int found_orange = 0;
		int found_cherry = 0;
		int found_plum   = 0;
		int found_pear   = 0;
		ptr1 = dict.first();
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
			else if ( STRING_EQ( ptr1->m_stringKeyPtr, "plum" ) )
			{
				found_plum++;
			}
			else if ( STRING_EQ( ptr1->m_stringKeyPtr, "pear" ) )
			{
				found_pear++;
			}
			else
			{
				not_found++;
			}

			ptr1 = dict.next( *ptr1 );
		}
		REQUIRE( found_apple == 1 );
		REQUIRE( found_orange == 1 );
		REQUIRE( found_cherry == 1 );
		REQUIRE( found_plum == 1 );
		REQUIRE( found_pear == 1 );
		REQUIRE( not_found == 0 );

		dict.insert( bob );
		bool result = dict.removeItem( orange );
		REQUIRE( result == true );
		REQUIRE( dict.removeItem( orange ) == false );
		REQUIRE( dict.remove( key2 ) == 0 );
		dict.insert( orange );
		ptr1 = dict.remove( key2 );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
		REQUIRE( dict.remove( bob ) != 0 );

		DList<DictItem>    buckets_5[5];
		Dictionary<MyItem> dst( buckets_5, 5 );
		REQUIRE( dst.first() == 0 );
		dict.move( dst );
		dict.stats( info );
		REQUIRE( info.m_numItems == 0 );
		dst.stats( info );
		REQUIRE( info.m_numItems == 4 );
	}

	// 
	// Verify DictionaryItem works in a DList
	//
	SECTION( "DLIST: FIFO with DictionaryItem" )
	{
		DList<MyItem> dict;

		REQUIRE( dict.get() == 0 );
		REQUIRE( dict.head() == 0 );
		REQUIRE( dict.tail() == 0 );

		dict.put( apple );

		REQUIRE( dict.head() != 0 );
		REQUIRE( STRING_EQ( dict.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( dict.tail() != 0 );
		REQUIRE( STRING_EQ( dict.tail()->m_stringKeyPtr, "apple" ) );

		dict.put( orange );

		REQUIRE( dict.head() != 0 );
		REQUIRE( STRING_EQ( dict.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( dict.tail() != 0 );
		REQUIRE( STRING_EQ( dict.tail()->m_stringKeyPtr, "orange" ) );

		dict.put( cherry );

		REQUIRE( dict.head() != 0 );
		REQUIRE( STRING_EQ( dict.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( dict.tail() != 0 );
		REQUIRE( STRING_EQ( dict.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = dict.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "apple" ) );
		REQUIRE( dict.head() != 0 );
		REQUIRE( STRING_EQ( dict.head()->m_stringKeyPtr, "orange" ) );
		REQUIRE( dict.tail() != 0 );
		REQUIRE( STRING_EQ( dict.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = dict.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
		REQUIRE( dict.head() != 0 );
		REQUIRE( STRING_EQ( dict.head()->m_stringKeyPtr, "cherry" ) );
		REQUIRE( dict.tail() != 0 );
		REQUIRE( STRING_EQ( dict.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = dict.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
		REQUIRE( dict.head() == 0 );
		REQUIRE( dict.tail() == 0 );

		REQUIRE( dict.get() == 0 );
		REQUIRE( dict.head() == 0 );
		REQUIRE( dict.tail() == 0 );
	}

	// 
	// Verify DictionaryItem works in a DList
	//
	SECTION( "SLIST: FIFO with DictionaryItem" )
	{
		SList<MyItem> dict;

		REQUIRE( dict.get() == 0 );
		REQUIRE( dict.head() == 0 );
		REQUIRE( dict.tail() == 0 );

		dict.put( apple );

		REQUIRE( dict.head() != 0 );
		REQUIRE( STRING_EQ( dict.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( dict.tail() != 0 );
		REQUIRE( STRING_EQ( dict.tail()->m_stringKeyPtr, "apple" ) );

		dict.put( orange );

		REQUIRE( dict.head() != 0 );
		REQUIRE( STRING_EQ( dict.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( dict.tail() != 0 );
		REQUIRE( STRING_EQ( dict.tail()->m_stringKeyPtr, "orange" ) );

		dict.put( cherry );

		REQUIRE( dict.head() != 0 );
		REQUIRE( STRING_EQ( dict.head()->m_stringKeyPtr, "apple" ) );
		REQUIRE( dict.tail() != 0 );
		REQUIRE( STRING_EQ( dict.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = dict.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "apple" ) );
		REQUIRE( dict.head() != 0 );
		REQUIRE( STRING_EQ( dict.head()->m_stringKeyPtr, "orange" ) );
		REQUIRE( dict.tail() != 0 );
		REQUIRE( STRING_EQ( dict.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = dict.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
		REQUIRE( dict.head() != 0 );
		REQUIRE( STRING_EQ( dict.head()->m_stringKeyPtr, "cherry" ) );
		REQUIRE( dict.tail() != 0 );
		REQUIRE( STRING_EQ( dict.tail()->m_stringKeyPtr, "cherry" ) );

		ptr1 = dict.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
		REQUIRE( dict.head() == 0 );
		REQUIRE( dict.tail() == 0 );

		REQUIRE( dict.get() == 0 );
		REQUIRE( dict.head() == 0 );
		REQUIRE( dict.tail() == 0 );
	}


	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}

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
#include "Cpl/Container/Item.h"    
#include "Cpl/Container/DList.h"    
#include "Cpl/Container/SList.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


/// 
using namespace Cpl::Container;
using namespace Cpl::System;



/// Short hand for brute forcing string compares when not using std::string
#define STRING_EQ(a,b)  strcmp(a,b) == 0


////////////////////////////////////////////////////////////////////////////////

/// Use un-named namespace to make my class local-to-the-file in scope
namespace {

class MyItem : public ExtendedItem
{
public:
	///
	MyItem( const char* name ) : m_name( name ) {};
	///
	const char* m_name;
};

class ItemAutoAdd : public ExtendedItem
{
public:
	///
	ItemAutoAdd( const char* name, DList<ItemAutoAdd>& list )
		:m_name( name )
	{
		list.put( *this );
	}

	///
	const char* m_name;
};

}; // end namespace

////////////////////////////////////////////////////////////////////////////////
static DList<ItemAutoAdd> emptylist_( "static constructor" );
static DList<ItemAutoAdd> staticlist_( "static constructor" );
static ItemAutoAdd        staticItem_( "staticItem", staticlist_ );




////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "DLIST: Validate member functions", "[dlist]" )
{
	DList<MyItem> list;
	MyItem        apple( "apple" );
	MyItem        orange( "orange" );
	MyItem        cherry( "cherry" );
	MyItem        pear( "pear" );
	MyItem        plum( "plum" );
	MyItem*       ptr1;

	Shutdown_TS::clearAndUseCounter();

	SECTION( "Validate that an 'item' can be only in one Container" )
	{
		DList<MyItem> foo;
		DList<MyItem> bar;

		MyItem item( "bob" );

		foo.put( item );
		bar.put( item );

		REQUIRE( Shutdown_TS::getAndClearCounter() == 1u );
	}

	SECTION( "Validate static Constructor" )
	{
		REQUIRE( emptylist_.head() == 0 );
		REQUIRE( emptylist_.tail() == 0 );

		REQUIRE( staticlist_.head() != 0 );
		REQUIRE( staticlist_.tail() != 0 );
		REQUIRE( STRING_EQ( staticlist_.get()->m_name, "staticItem" ) );
	}

	SECTION( "List Operations" )
	{
		DList<MyItem> foo;
		DList<MyItem> bar;
		MyItem        bob( "bob" );
		MyItem        yours( "yours" );
		MyItem        uncle( "uncle" );

		foo.put( bob );
		foo.put( yours );
		foo.put( uncle );
		REQUIRE( STRING_EQ( foo.head()->m_name, "bob" ) );
		REQUIRE( STRING_EQ( foo.tail()->m_name, "uncle" ) );
		REQUIRE( bar.head() == 0 );
		REQUIRE( bar.tail() == 0 );

		foo.move( bar );
		REQUIRE( STRING_EQ( bar.head()->m_name, "bob" ) );
		REQUIRE( STRING_EQ( bar.tail()->m_name, "uncle" ) );
		REQUIRE( foo.head() == 0 );
		REQUIRE( foo.tail() == 0 );

		bar.clearTheList();
		REQUIRE( bar.head() == 0 );
		REQUIRE( bar.tail() == 0 );
	}

	SECTION( "FIFO" )
	{
		REQUIRE( list.get() == 0 );
		REQUIRE( list.head() == 0 );
		REQUIRE( list.tail() == 0 );

		list.put( apple );

		REQUIRE( list.head() != 0 );
		REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
		REQUIRE( list.tail() != 0 );
		REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );

		list.put( orange );

		REQUIRE( list.head() != 0 );
		REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
		REQUIRE( list.tail() != 0 );
		REQUIRE( STRING_EQ( list.tail()->m_name, "orange" ) );

		list.put( cherry );

		REQUIRE( list.head() != 0 );
		REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
		REQUIRE( list.tail() != 0 );
		REQUIRE( STRING_EQ( list.tail()->m_name, "cherry" ) );

		ptr1 = list.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
		REQUIRE( list.head() != 0 );
		REQUIRE( STRING_EQ( list.head()->m_name, "orange" ) );
		REQUIRE( list.tail() != 0 );
		REQUIRE( STRING_EQ( list.tail()->m_name, "cherry" ) );

		ptr1 = list.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
		REQUIRE( list.head() != 0 );
		REQUIRE( STRING_EQ( list.head()->m_name, "cherry" ) );
		REQUIRE( list.tail() != 0 );
		REQUIRE( STRING_EQ( list.tail()->m_name, "cherry" ) );

		ptr1 = list.get();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
		REQUIRE( list.head() == 0 );
		REQUIRE( list.tail() == 0 );

		REQUIRE( list.get() == 0 );
		REQUIRE( list.head() == 0 );
		REQUIRE( list.tail() == 0 );
	}

	SECTION( "STACK" )
	{
		// Note: pop() == get(), top() == head(), push() == putFirst()
		list.push( apple );
		REQUIRE( list.top() != 0 );
		REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
		REQUIRE( list.tail() != 0 );
		REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );

		ptr1 = list.pop();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
		REQUIRE( list.top() == 0 );
		REQUIRE( list.tail() == 0 );
		REQUIRE( list.pop() == 0 );

		list.push( apple );
		list.push( orange );
		list.push( cherry );
		ptr1 = list.pop();
		list.push( plum );
		list.push( pear );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
		REQUIRE( STRING_EQ( list.top()->m_name, "pear" ) );
		REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );

		REQUIRE( STRING_EQ( list.pop()->m_name, "pear" ) );
		REQUIRE( STRING_EQ( list.pop()->m_name, "plum" ) );
		REQUIRE( STRING_EQ( list.pop()->m_name, "orange" ) );
		REQUIRE( STRING_EQ( list.pop()->m_name, "apple" ) );
		REQUIRE( list.top() == 0 );
		REQUIRE( list.tail() == 0 );
		REQUIRE( list.pop() == 0 );
	}

	SECTION( "Ordered List" )
	{
		// Note: getFirst() == get(), putLast() == put(), putFirst() == push(), first() == head(), last() == tail()
		list.putLast( orange );
		list.putFirst( apple );
		REQUIRE( STRING_EQ( list.first()->m_name, "apple" ) );
		REQUIRE( STRING_EQ( list.tail()->m_name, "orange" ) );

		ptr1 = list.getFirst();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
		REQUIRE( list.head() != 0 );
		REQUIRE( STRING_EQ( list.head()->m_name, "orange" ) );
		REQUIRE( list.tail() != 0 );
		REQUIRE( STRING_EQ( list.tail()->m_name, "orange" ) );
		list.putFirst( *ptr1 );

		ptr1 = list.getLast();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
		REQUIRE( list.head() != 0 );
		REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
		REQUIRE( list.tail() != 0 );
		REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );
		list.putLast( *ptr1 );

		list.insertBefore( apple, cherry );
		list.insertAfter( orange, plum );
		REQUIRE( STRING_EQ( list.first()->m_name, "cherry" ) );
		REQUIRE( STRING_EQ( list.tail()->m_name, "plum" ) );

		ptr1 = list.first();
		REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
		ptr1 = list.next( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
		ptr1 = list.next( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
		ptr1 = list.next( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "plum" ) );

		list.insertBefore( plum, pear );
		list.remove( orange );
		list.insertAfter( cherry, orange );
		ptr1 = list.first();
		REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
		ptr1 = list.next( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
		ptr1 = list.next( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
		ptr1 = list.next( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "pear" ) );
		ptr1 = list.next( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "plum" ) );

		REQUIRE( list.remove( plum ) == true );
		REQUIRE( list.remove( cherry ) == true );
		REQUIRE( list.remove( plum ) == false );
		ptr1 = list.first();
		REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
		ptr1 = list.next( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
		ptr1 = list.next( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "pear" ) );

		REQUIRE( list.find( plum ) == false );
		REQUIRE( list.find( orange ) == true );
		REQUIRE( list.find( pear ) == true );
		REQUIRE( list.find( apple ) == true );
		REQUIRE( list.find( cherry ) == false );

		REQUIRE( list.remove( orange ) == true );
		REQUIRE( list.remove( apple ) == true );
		REQUIRE( list.remove( pear ) == true );

		// 
		// START 'new' to DList tests...
		//
		list.put( cherry );
		list.put( apple );
		list.put( orange );
		list.put( plum );
		ptr1 = list.last();
		REQUIRE( STRING_EQ( ptr1->m_name, "plum" ) );
		ptr1 = list.previous( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
		ptr1 = list.previous( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
		ptr1 = list.previous( *ptr1 );
		REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
	}

	// 
	// START 'new' to DList tests...
	//
	SECTION( "STACK: SList using ExtendedItem" )
	{
		SList<MyItem> list;

		// Note: pop() == get(), top() == head(), push() == putFirst()
		list.push( apple );
		REQUIRE( list.top() != 0 );
		REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
		REQUIRE( list.tail() != 0 );
		REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );

		ptr1 = list.pop();
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
		REQUIRE( list.top() == 0 );
		REQUIRE( list.tail() == 0 );
		REQUIRE( list.pop() == 0 );

		list.push( apple );
		list.push( orange );
		list.push( cherry );
		ptr1 = list.pop();
		list.push( plum );
		list.push( pear );
		REQUIRE( ptr1 != 0 );
		REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
		REQUIRE( STRING_EQ( list.top()->m_name, "pear" ) );
		REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );

		REQUIRE( STRING_EQ( list.pop()->m_name, "pear" ) );
		REQUIRE( STRING_EQ( list.pop()->m_name, "plum" ) );
		REQUIRE( STRING_EQ( list.pop()->m_name, "orange" ) );
		REQUIRE( STRING_EQ( list.pop()->m_name, "apple" ) );
		REQUIRE( list.top() == 0 );
		REQUIRE( list.tail() == 0 );
		REQUIRE( list.pop() == 0 );
	}


	REQUIRE( Shutdown_TS::getAndClearCounter() == 0u );
}

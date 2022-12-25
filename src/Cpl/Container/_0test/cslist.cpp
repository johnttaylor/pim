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
#include "Cpl/Container/cslist.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <string.h>


/// Short hand for brute forcing string compares when not using std::string
#define STRING_EQ(a,b)  strcmp(a,b) == 0


////////////////////////////////////////////////////////////////////////////////

typedef struct MyItem_T
{
    CPL_CONTAINTER_ITEM_SLIST;
    const char* name;
} MyItem_T;


////////////////////////////////////////////////////////////////////////////////
static CplContainerSList_T emptylist_;
static CplContainerSList_T staticlist_;



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "cslist" )
{
    CplContainerSList_T list; Cpl_Container_SList_initialize( &list );
    MyItem_T  apple; Cpl_Container_Item_initialize( &apple ); apple.name = "apple";
    MyItem_T  orange; Cpl_Container_Item_initialize( &orange ); orange.name = "orange";
    MyItem_T  cherry; Cpl_Container_Item_initialize( &cherry );  cherry.name = "cherry";
    MyItem_T  pear; Cpl_Container_Item_initialize( &pear );  pear.name = "pear";
    MyItem_T  plum; Cpl_Container_Item_initialize( &plum );  plum.name = "plum";;
    MyItem_T* ptr1;

    SECTION( "Validate that an 'item' can be only in one Container" )
    {
        CplContainerSList_T foo; Cpl_Container_SList_initialize( &foo );
        CplContainerSList_T bar; Cpl_Container_SList_initialize( &bar );

        MyItem_T  item; Cpl_Container_Item_initialize( &item ); item.name = "bob";

        REQUIRE( Cpl_Container_SList_putFirst( &foo, &item ) );
        REQUIRE( Cpl_Container_SList_put( &bar, &item ) == false );
        REQUIRE( Cpl_Container_SList_putFirst( &bar, &item ) == false );
    }

    SECTION( "Validate static Constructor" )
    {
        REQUIRE( Cpl_Container_SList_peekHead( &emptylist_ ) == 0 );
        REQUIRE( Cpl_Container_SList_peekTail( &emptylist_ ) == 0 );

        REQUIRE( Cpl_Container_SList_peekHead( &staticlist_ ) == 0 );
        REQUIRE( Cpl_Container_SList_peekTail( &staticlist_ ) == 0 );
    }

    SECTION( "List Operations" )
    {
        CplContainerSList_T foo; Cpl_Container_SList_initialize( &foo );
        CplContainerSList_T bar; Cpl_Container_SList_initialize( &bar );

        MyItem_T  bob; Cpl_Container_Item_initialize( &bob ); bob.name = "bob";
        MyItem_T  yours; Cpl_Container_Item_initialize( &yours ); yours.name = "yours";
        MyItem_T  uncle; Cpl_Container_Item_initialize( &uncle ); uncle.name = "uncle";

        REQUIRE( Cpl_Container_SList_put( &foo, &bob ) );
        REQUIRE( Cpl_Container_SList_put( &foo, &yours ) );
        REQUIRE( Cpl_Container_SList_put( &foo, &uncle ) );
        ptr1 = (MyItem_T *)Cpl_Container_SList_peekHead( &foo );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "bob" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &foo );
        REQUIRE( STRING_EQ( ptr1->name, "uncle" ) );
        REQUIRE( Cpl_Container_SList_peekHead( &bar ) == 0 );
        REQUIRE( Cpl_Container_SList_peekTail( &bar ) == 0 );

        Cpl_Container_SList_move( &bar, &foo );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &bar );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "bob" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &bar );
        REQUIRE( STRING_EQ( ptr1->name, "uncle" ) );
        REQUIRE( Cpl_Container_SList_peekHead( &foo ) == 0 );
        REQUIRE( Cpl_Container_SList_peekTail( &foo ) == 0 );

        Cpl_Container_SList_clear( &bar );
        REQUIRE( Cpl_Container_SList_peekHead( &bar ) == 0 );
        REQUIRE( Cpl_Container_SList_peekTail( &bar ) == 0 );
    }

    SECTION( "FIFO" )
    {
        REQUIRE( Cpl_Container_SList_get( &list ) == 0 );
        REQUIRE( Cpl_Container_SList_peekHead( &list ) == 0 );
        REQUIRE( Cpl_Container_SList_peekTail( &list ) == 0 );

        REQUIRE( Cpl_Container_SList_put( &list, &apple ) );

        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );

        REQUIRE( Cpl_Container_SList_put( &list, &orange ) );

        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "orange" ) );

        REQUIRE( Cpl_Container_SList_put( &list, &cherry ) );

        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "cherry" ) );

        ptr1 = (MyItem_T*)Cpl_Container_SList_get( &list );
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "orange" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "cherry" ) );

        ptr1 = (MyItem_T*) Cpl_Container_SList_get( &list );
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->name, "orange" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "cherry" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "cherry" ) );

        ptr1 = (MyItem_T*) Cpl_Container_SList_get( &list );
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->name, "cherry" ) );
        REQUIRE( Cpl_Container_SList_peekHead( &list ) == 0 );
        REQUIRE( Cpl_Container_SList_peekTail( &list ) == 0 );

        ptr1 = (MyItem_T*) Cpl_Container_SList_get( &list );
        REQUIRE( ptr1 == 0 );
        REQUIRE( Cpl_Container_SList_peekHead( &list ) == 0 );
        REQUIRE( Cpl_Container_SList_peekTail( &list ) == 0 );
    }

    SECTION( "Ordered List" )
    {
        REQUIRE( Cpl_Container_SList_put( &list, &orange ) );
        REQUIRE( Cpl_Container_SList_putFirst( &list, &apple ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "orange" ) );

        ptr1 = (MyItem_T*) Cpl_Container_SList_get( &list );
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "orange" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "orange" ) );

        REQUIRE( Cpl_Container_SList_putFirst( &list, &apple ) );
        ptr1 = (MyItem_T*) Cpl_Container_SList_getLast( &list );
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->name, "orange" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );

        REQUIRE( Cpl_Container_SList_put( &list, &orange ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "orange" ) );

        REQUIRE( Cpl_Container_SList_put( &list, &cherry ) );
        REQUIRE( Cpl_Container_SList_put( &list, &pear ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );
        ptr1 = (MyItem_T*) Cpl_Container_SList_next( ptr1 );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "orange" ) );
        ptr1 = (MyItem_T*) Cpl_Container_SList_next( ptr1 );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "cherry" ) );
        ptr1 = (MyItem_T*) Cpl_Container_SList_next( ptr1 );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "pear" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "pear" ) );



        REQUIRE( Cpl_Container_SList_remove( &list, &orange ) == true );
        REQUIRE( Cpl_Container_SList_remove( &list, &cherry ) == true );
        REQUIRE( Cpl_Container_SList_remove( &list, &orange ) == false );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "apple" ) );
        ptr1 = (MyItem_T*) Cpl_Container_SList_next( ptr1 );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "pear" ) );

        REQUIRE( Cpl_Container_SList_remove( &list, &apple ) == true );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekHead( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "pear" ) );
        ptr1 = (MyItem_T *) Cpl_Container_SList_peekTail( &list );
        REQUIRE( ptr1 );
        REQUIRE( STRING_EQ( ptr1->name, "pear" ) );

        REQUIRE( Cpl_Container_SList_isInList( &list, &plum ) == false );
        REQUIRE( Cpl_Container_SList_isInList( &list, &orange ) == false );
        REQUIRE( Cpl_Container_SList_isInList( &list, &pear ) == true );
        REQUIRE( Cpl_Container_SList_isInList( &list, &apple ) == false );
        REQUIRE( Cpl_Container_SList_isInList( &list, &cherry ) == false );

        REQUIRE( Cpl_Container_SList_remove( &list, &pear ) == true );
    }

}

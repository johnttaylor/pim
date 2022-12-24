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

#include "cdlist.h"
#include <assert.h>

void Cpl_Container_DList_initialize( CplContainerDList_T* listToInitialize )
{
    assert( listToInitialize );

    listToInitialize->headPtr = 0;
    listToInitialize->tailPtr = 0;
}

void Cpl_Container_DList_move( CplContainerDList_T* dstList, CplContainerDList_T* srcList )
{
    assert( dstList );
    assert( srcList );

    // clear the destination list
    Cpl_Container_DList_clear( dstList );

    // Copy each item (so the debug info is correct)
    CplContainerItemDListLinkage_T* nextPtr;
    while ( (nextPtr=(CplContainerItemDListLinkage_T*) Cpl_Container_DList_get( srcList )) )
    {
        Cpl_Container_DList_put( dstList, nextPtr );
    }
}

void Cpl_Container_DList_clear( CplContainerDList_T* listToClear )
{
    assert( listToClear );

    // Drain list so the debug traps work correctly
    while ( Cpl_Container_DList_get( listToClear ) )
        ;
}

void* Cpl_Container_DList_get( CplContainerDList_T* list )
{
    assert( list );

    CplContainerItemDListLinkage_T* firstPtr = list->headPtr;
    if ( firstPtr )
    {
        Cpl_Container_DList_remove( list, firstPtr );
    }
    return firstPtr;
}

bool Cpl_Container_DList_put( CplContainerDList_T* list, void* item )
{
    assert( list );
    assert( item );
    CplContainerItemDListLinkage_T* itemPtr = (CplContainerItemDListLinkage_T*) item;

    if ( Cpl_Container_Item_markAsContained_( itemPtr, list ) )
    {
        if ( list->headPtr )
        {
            list->tailPtr->nextPtr = itemPtr;
            itemPtr->prevPtr       = list->tailPtr;
        }
        else
        {
            list->headPtr    = itemPtr;
            itemPtr->prevPtr = 0;
        }
        itemPtr->nextPtr = 0;
        list->tailPtr    = itemPtr;

        return true;
    }

    return false;
}

void* Cpl_Container_DList_getLast( CplContainerDList_T* list )
{
    assert( list );
    CplContainerItemDListLinkage_T* lastPtr = list->tailPtr;

    if ( lastPtr )
    {
        Cpl_Container_DList_remove( list, lastPtr );
    }
    return lastPtr;
}

bool Cpl_Container_DList_putFirst( CplContainerDList_T* list, void* item )
{
    assert( list );
    assert( item );
    CplContainerItemDListLinkage_T* itemPtr = (CplContainerItemDListLinkage_T*) item;

    if ( Cpl_Container_Item_markAsContained_( itemPtr, list ) )
    {
        if ( list->headPtr )
        {
            itemPtr->nextPtr       = list->headPtr;
            list->headPtr->prevPtr = itemPtr;
            list->headPtr          = itemPtr;
        }
        else
        {
            list->headPtr    = list->tailPtr = itemPtr;
            itemPtr->nextPtr = 0;
        }
        itemPtr->prevPtr = 0;

        return true;
    }

    return false;
}

bool Cpl_Container_DList_insertBefore( CplContainerDList_T* list, void* beforeItem, void* newItem )
{
    assert( list );
    assert( newItem );
    assert( beforeItem );
    CplContainerItemDListLinkage_T* itemPtr   = (CplContainerItemDListLinkage_T*) newItem;
    CplContainerItemDListLinkage_T* beforePtr = (CplContainerItemDListLinkage_T*) beforeItem;

    if ( Cpl_Container_Item_markAsContained_( itemPtr, list ) )
    {
        itemPtr->prevPtr                       = beforePtr->prevPtr;
        CplContainerItemDListLinkage_T* prvPtr = (CplContainerItemDListLinkage_T*) itemPtr->prevPtr;
        itemPtr->nextPtr                       = beforePtr;
        beforePtr->prevPtr                     = itemPtr;
        if ( !prvPtr )
        {
            list->headPtr = itemPtr;
        }
        else
        {
            prvPtr->nextPtr = itemPtr;
        }

        return true;
    }

    return false;
}

bool Cpl_Container_DList_insertAfter( CplContainerDList_T* list, void* afterItem, void* newItem )
{
    assert( list );
    assert( newItem );
    assert( afterItem );
    CplContainerItemDListLinkage_T* itemPtr  = (CplContainerItemDListLinkage_T*) newItem;
    CplContainerItemDListLinkage_T* afterPtr = (CplContainerItemDListLinkage_T*) afterItem;

    if ( Cpl_Container_Item_markAsContained_( itemPtr, list ) )
    {
        itemPtr->nextPtr                       = afterPtr->nextPtr;
        CplContainerItemDListLinkage_T* nxtPtr = (CplContainerItemDListLinkage_T*) afterPtr->nextPtr;
        itemPtr->prevPtr                       = afterPtr;
        afterPtr->nextPtr                      = itemPtr;
        if ( !nxtPtr )
        {
            list->tailPtr = itemPtr;
        }
        else
        {
            nxtPtr->prevPtr = itemPtr;
        }

        return true;
    }

    return false;
}

void* Cpl_Container_DList_peekHead( const CplContainerDList_T* list )
{
    assert( list );
    return list->headPtr;
}

void* Cpl_Container_DList_peekTail( const CplContainerDList_T* list )
{
    assert( list );
    return list->tailPtr;
}

bool Cpl_Container_DList_remove( CplContainerDList_T* list, void* itemToRemove )
{
    assert( list );
    assert( itemToRemove );
    CplContainerItemDListLinkage_T* itemPtr = (CplContainerItemDListLinkage_T*) itemToRemove;

    if ( Cpl_Container_Item_isInContainer_( itemPtr, list ) )
    {
        CplContainerItemDListLinkage_T* prvPtr = (CplContainerItemDListLinkage_T*) itemPtr->prevPtr;
        CplContainerItemDListLinkage_T* nxtPtr = (CplContainerItemDListLinkage_T*)itemPtr->nextPtr;
        if ( prvPtr )
        {
            if ( !(prvPtr->nextPtr=nxtPtr) )
            {
                list->tailPtr = prvPtr;         // Case: Remove tail object
            }
            else
            {
                nxtPtr->prevPtr = prvPtr;       // Case: Remove intermediate object
            }
        }
        else
        {
            if ( !(list->headPtr=nxtPtr) )
            {
                list->tailPtr = 0;              // Case: Remove last object
            }
            else
            {
                nxtPtr->prevPtr = 0;    // Case: Remove Head object
            }
        }

        Cpl_Container_Item_markAsFree_( itemPtr );
        return true;
    }

    return false;
}

void* Cpl_Container_DList_next( const void* item )
{
    assert( item );
    CplContainerItemDListLinkage_T* itemPtr = (CplContainerItemDListLinkage_T*) item;
    return itemPtr->nextPtr;
}

void* Cpl_Container_DList_prev( const void* item )
{
    assert( item );
    CplContainerItemDListLinkage_T* itemPtr = (CplContainerItemDListLinkage_T*) item;
    return itemPtr->prevPtr;
}

bool Cpl_Container_DList_isInList( const CplContainerDList_T* list, const void* item )
{
    assert( list );
    assert( item );
    return Cpl_Container_Item_isInContainer_( item, list );
}


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

#include "cslist.h"
#include <assert.h>

void Cpl_Container_SList_initialize( CplContainerSList_T* listToInitialize )
{
    assert( listToInitialize );

    listToInitialize->headPtr = 0;
    listToInitialize->tailPtr = 0;
}


void Cpl_Container_SList_move( CplContainerSList_T* dstList, CplContainerSList_T* srcList )
{
    assert( dstList );
    assert( srcList );

    // clear the destination list
    Cpl_Container_SList_clear( dstList );

    // Copy each item (so the debug info is correct)
    CplContainerItemSListLinkage_T* nextPtr;
    while ( (nextPtr=(CplContainerItemSListLinkage_T*)Cpl_Container_SList_get( srcList )) )
    {
        Cpl_Container_SList_put( dstList, nextPtr );
    }
}

void Cpl_Container_SList_clear( CplContainerSList_T* listToClear )
{
    assert( listToClear );

    // Drain list so the debug traps work correctly
    while ( Cpl_Container_SList_get( listToClear ) )
        ;
}



void* Cpl_Container_SList_get( CplContainerSList_T* list )
{
    assert( list );
    CplContainerItemSListLinkage_T* nextPtr;

    if ( (nextPtr=(CplContainerItemSListLinkage_T*) list->headPtr) )
    {
        if ( !(list->headPtr=(CplContainerItemSListLinkage_T*) nextPtr->nextPtr) )
        {
            list->tailPtr = 0;
        }
    }
    Cpl_Container_Item_markAsFree_( nextPtr );
    return nextPtr;
}

void* Cpl_Container_SList_getLast( CplContainerSList_T* list )
{
    assert( list );
    CplContainerItemSListLinkage_T* lastPtr;

    lastPtr = list->tailPtr;
    if ( lastPtr )
    {
        Cpl_Container_SList_remove( list, list->tailPtr );
    }
    return lastPtr;
}



bool Cpl_Container_SList_put( CplContainerSList_T* list, void* item )
{
    assert( list );
    assert( item );
    CplContainerItemSListLinkage_T* itemPtr = (CplContainerItemSListLinkage_T*) item;

    if ( Cpl_Container_Item_markAsContained_( itemPtr, list ) )
    {
        if ( list->headPtr )
        {
            list->tailPtr->nextPtr = itemPtr;
        }
        else
        {
            list->headPtr = itemPtr;
        }
        itemPtr->nextPtr = 0;
        list->tailPtr    = itemPtr;

        return true;
    }
    return false;
}

bool Cpl_Container_SList_putFirst( CplContainerSList_T* list, void* item )
{
    assert( list );
    assert( item );
    CplContainerItemSListLinkage_T* itemPtr = (CplContainerItemSListLinkage_T*) item;

    if ( Cpl_Container_Item_markAsContained_( itemPtr, list ) )
    {
        if ( list->headPtr )
        {
            itemPtr->nextPtr = list->headPtr;
            list->headPtr    = itemPtr;
        }
        else
        {
            list->headPtr    = list->tailPtr = itemPtr;
            itemPtr->nextPtr = 0;
        }

        return true;
    }
    return false;
}

void* Cpl_Container_SList_peekHead( const CplContainerSList_T* list )
{
    assert( list );
    return list->headPtr;
}

void* Cpl_Container_SList_peekTail( const CplContainerSList_T* list )
{
    assert( list );
    return list->tailPtr;
}

bool Cpl_Container_SList_remove( CplContainerSList_T* list, void* itemToRemove )
{
    assert( list );
    assert( itemToRemove );
    CplContainerItemSListLinkage_T* itemPtr = (CplContainerItemSListLinkage_T*) itemToRemove;

    if ( Cpl_Container_Item_isInContainer_( itemPtr, list ) )
    {
        CplContainerItemSListLinkage_T* nxtPtr;
        CplContainerItemSListLinkage_T* prvPtr;
        for ( nxtPtr=list->headPtr, prvPtr=0; nxtPtr; prvPtr=nxtPtr, nxtPtr=(CplContainerItemSListLinkage_T*) nxtPtr->nextPtr )
        {
            if ( nxtPtr == itemPtr )
            {
                if ( prvPtr )
                {
                    if ( !(prvPtr->nextPtr=nxtPtr->nextPtr) )
                    {
                        list->tailPtr = prvPtr;
                    }
                }
                else
                {
                    if ( !(list->headPtr=(CplContainerItemSListLinkage_T*) nxtPtr->nextPtr) )
                    {
                        list->tailPtr = 0;
                    }
                }
                Cpl_Container_Item_markAsFree_( itemPtr );
                return true;
            }
        }
        // Note: I should never get here (i have already checked that I am in list at the top of the method)
    }
    return false;
}

void* Cpl_Container_SList_next( const void* item )
{
    assert( item );
    CplContainerItemSListLinkage_T* itemPtr = (CplContainerItemSListLinkage_T*) item;
    return itemPtr->nextPtr;
}

bool Cpl_Container_SList_isInList( const CplContainerSList_T* list, const void* item )
{
    return Cpl_Container_Item_isInContainer_( item, list );
}

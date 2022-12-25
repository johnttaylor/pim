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

#include "citem.h"
#include <assert.h>



void Cpl_Container_Item_initialize( void* uninitializeItem )
{
    assert( uninitializeItem );
    CplContainerItemSListLinkage_T* itemLinkage = (CplContainerItemSListLinkage_T*) uninitializeItem;
    
    // Make sure that the 'container' pointer is ZERO
    itemLinkage->containerPtr = 0; 
}

bool Cpl_Container_Item_markAsContained_( void* item, void* newContainerPtr )
{
    assert( item );
    assert( newContainerPtr );
    CplContainerItemSListLinkage_T* itemLinkage = (CplContainerItemSListLinkage_T*) item;

    // Check for error of being in two lists at the same time
    if ( itemLinkage->containerPtr != 0 )
    {
        return false;
    }

    // Mark as contained
    else
    {
        itemLinkage->containerPtr = newContainerPtr;
        return true;
    }
}

bool Cpl_Container_Item_isInContainer_( const void* item, const void* containerPtr )
{
    assert( item );
    assert( containerPtr );
    CplContainerItemSListLinkage_T* itemLinkage = (CplContainerItemSListLinkage_T*) item;

    return itemLinkage->containerPtr == containerPtr;
}

void Cpl_Container_Item_markAsFree_( void* item )
{
    CplContainerItemSListLinkage_T* itemLinkage = (CplContainerItemSListLinkage_T*) item;
    if ( itemLinkage )
    {
        itemLinkage->containerPtr = 0;
    }
}




#ifndef Cpl_Container_citem_h_
#define Cpl_Container_citem_h_
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
/** @file 

    This file provides the definitions for creating a data structures that
    can be contained in a linked lists, containers, trees, etc. using 'intrusive'
    linkage. The item to be contained is RESPONSIBLE for providing the memory for
    the link pointer(s).  CAUTION: Since the is only one set of link(s) per
    structure, the structure can only be in at most ONE list at any given time.
    
    USAGE:
    ------
    To make a data structure 'listable', the application/developers needs
    to modify the data structure as follows:
    \code
    
        Original structure:
            typedef struct
            {
                int   my_data;
                void* my_ptr;
                ....
            } MyStructure;
    
        Linkable structure:
            typedef struct
            {
                CPL_CONTAINTER_ITEM_SLIST;             // Link(s) for container implementation
                int   my_data;
                void* my_ptr;
                ....
            } MyStructure;

    \endcode

    NOTES: 
    1. The above macro MUST be the FIRST field in the data structure.
    2. All 'items' must be initialized before be placed in a list
    3. Items are NOT type safe!
    4. An item can be in at most ONE container at any given time.  If an 
       attempt is made to put an item is to multiple containers (at the same
       time) - the put/insert operation fails
*/

#include <stdbool.h>


// 
#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/** This type defines the fatal-error-callback function handler
 */
typedef void (*CplContainerItemFatalErrorCbFunc_T)(void* item, void* currentList, void* newList);

/// Type definition for a singly linked item
typedef struct 
{
    void* containerPtr;
    void* nextPtr;
} CplContainerItemSListLinkage_T;

/** Single-Link field.  This symbol must be placed as the FIRST entry in a
    the user's structure that will/can be stored in a container.
 */
#define CPL_CONTAINTER_ITEM_SLIST       CplContainerItemSListLinkage_T __cpl_container_item_slist__


/// Type definition for a Doubly linked item
typedef struct 
{
    void* containerPtr;
    void* nextPtr;
    void* prevPtr;
} CplContainerItemDListLinkage_T;

/** Double-Link field.  This symbol must be placed as the FIRST entry in a
     the user's structure that will/can be stored in a container.
     
     NOTE: The item can also be used/contained in a Singly Link list
  */
#define CPL_CONTAINTER_ITEM_DLIST       CplContainerItemDListLinkage_T __cpl_container_item_dlist__

/*---------------------------------------------------------------------------*/
/** This method is used to initialize an 'item' instance.  The initialization 
    only needs to occur ONCE after the instance has been allocated. 
    ALL ITEMS MUST be initialize prior to placing them in a container.
    
    EXCEPTION: If an item is statically allocated, aka it is in the BSS segment,
    it does not have be initialize using this method since the C/C++ standard
    guaranties all data in the BSS segment will be initialized to zero on
    start-up.  
*/
void Cpl_Container_Item_initialize( void* uninitializeItem );


/*---------------------------------------------------------------------------*/
/** This function is private and/or has 'PACKAGE_SCOPE'.  This means that the
    application should NEVER call/use this functions.  The functions are 
    only exposes to simply the implementation of the containers.

    This function is used to mark the item as in-a-container.  

    Returns true if successful; else false is returned (i.e. the item is
    already in a container)
 */
bool Cpl_Container_Item_markAsContained_( void* item, void* newContainerPtr );

/** This function is private and/or has 'PACKAGE_SCOPE'.  This means that the
    application should NEVER call/use this functions.  The functions are
    only exposes to simply the implementation of the containers.

    This function returns true if the item is in the specified container
    else false is returned
 */
bool Cpl_Container_Item_isInContainer_( const void* item, const void* containerPtr );

/** This function is private and/or has 'PACKAGE_SCOPE'.  This means that the
    application should NEVER call/use this functions.  The functions are
    only exposes to simply the implementation of the containers.

    This function is used to mark the item as 'available', i.e. NOT in a 
    container.
 */
void Cpl_Container_Item_markAsFree_( void* item );

/*---------------------------------------------------------------------------*/
// 
#ifdef __cplusplus
};
#endif
#endif  // end header latch
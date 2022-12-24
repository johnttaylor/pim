#ifndef Cpl_Container_cdlist_h_
#define Cpl_Container_cdlist_h_
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

    This file defines a C-based interfaces for Doubly linked lists that do not 
    use/require dynamic memory allocation.  See the 'README.txt' file for 
    details on how intrusive containers work. 

    NOTE: The list operations are NOT type safe - it the Application's 
          responsibility for managing the types/content of the lists
*/

#include "Cpl/Container/citem.h"

// 
#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/** This structure define the internal structure of a Doubly Linked List. The
    application should treat this struct as black box, i.e. never access the
    field members directly.
*/
typedef struct CplContainerDList_T
{
    CplContainerItemDListLinkage_T* headPtr;   //!< Pointer to the first item in the list
    CplContainerItemDListLinkage_T* tailPtr;   //!< Pointer to the last item in the list
} CplContainerDList_T;


/** Initializes the list. This method must be called before calling any other
    functions in this interface..

    EXCEPTION: If the CplContainerDList_T instance is statically allocated, aka
    it is in the BSS segment, it does not have be initialized using this function
    since the C/C++ standard guaranties all data in the BSS segment will be
    initialized to zero on start-up (which is the initialized state).
*/
void Cpl_Container_DList_initialize( CplContainerDList_T* listToInitialize );


/** Moves the content of 'srcList' to the 'dstList'.  The 'dstList' is cleared
    before the move operation occurs.
 */
void Cpl_Container_DList_move( CplContainerDList_T* dstList, CplContainerDList_T* srcList );

/** Empties the list.  All references to the item(s) in the
    list are lost.
 */
void Cpl_Container_DList_clear( CplContainerDList_T* listToClear );


/** Removes the FIRST item in the list.  Returns NULL if the list is empty.
 */
void* Cpl_Container_DList_get( CplContainerDList_T* list );

/** Adds the item as the LAST item in the list.  Returns true if the
    operation was successful; else false is returned. 
 */
bool Cpl_Container_DList_put( CplContainerDList_T* list, void* item );

/** Removes the LAST item in the list.  Returns NULL if the list is empty.
*/
void* Cpl_Container_DList_getLast( CplContainerDList_T* list );

/** Adds the item as the FIRST item in the list.  Returns true if the
    operation was successful; else false is returned. 
 */
bool Cpl_Container_DList_putFirst( CplContainerDList_T* list, void* item );

/** Insert the 'newItem' into the list ahead of the 'beforeItem' element. 
    Returns true if the operation was successful; else false is returned. 
 */
bool Cpl_Container_DList_insertBefore( CplContainerDList_T* list, void* beforeItem, void* newItem );

/** Insert the 'newItem' into the list behind the 'afterItem' element. 
    Returns true if the operation was successful; else false is returned. 
 */
bool Cpl_Container_DList_insertAfter( CplContainerDList_T* list, void* afterItem, void* newItem );

/** Return a pointer to the FIRST item in the list. The returned item remains
    in the list.  Returns 0 if the list is empty.
 */
void* Cpl_Container_DList_peekHead( const CplContainerDList_T* list );

/** Return a pointer to the LAST item in the list. The returned item remains
    in the list.  Returns 0 if the list is empty.
 */
void* Cpl_Container_DList_peekTail( const CplContainerDList_T* list );

/** This function searches the list and - if found - removes the specified
    item from the list.  Returns true of the item was found and removed form
    the list; else false is returned.
 */
bool Cpl_Container_DList_remove( CplContainerDList_T* list, void* itemToRemove );

/** This function returns the next item in the list AFTER the specified item.
    The returned item is NOT removed from the list.  This method is typically
    use to traverse the list without changing its list membership. If there is
    no next item the method returns 0.  The method will also return 0 if the
    specified item is NOT in the list.
 */
 void* Cpl_Container_DList_next( const void* item );

/** This function returns the previous item in the list BEFORE the specified item.
    The returned item is NOT removed from the list.  This method is typically
    use to traverse the list without changing its list membership. If there is
    no previous item the method returns 0.  The method will also return 0 if the
    specified item is NOT in the list.
 */
 void* Cpl_Container_DList_prev( const void* item );

 /** Returns 'true' if the instance is in the specified list.
  */
 bool Cpl_Container_DList_isInList( const CplContainerDList_T* list, const void* item );

 
/*---------------------------------------------------------------------------*/
// 
#ifdef __cplusplus
};
#endif
#endif  // end header latch
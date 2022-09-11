#ifndef Cpl_Container_Map_h_
#define Cpl_Container_Map_h_
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

#include "Cpl/Container/AvlTree_.h"


/// 
namespace  Cpl {
/// 
namespace Container {

/** This template class implements an Map using an AVL Binary tree
    (i.e. a sorted list with fast searches, inserts, and deletes).

    Template ARGS:
        ITEM    - Data type of the object stored in the Map.  'ITEM' must
                  be a sub-class of the Cpl::Container::MapItem base
                  class.
 */

template <class ITEM>
class Map
{
private:
    /// Delegate operations to the generic tree implementation
    AvlTree_  m_tree;

public:
    ///Constructor.
    Map() noexcept {}

    /** This is a special constructor for when the Map is
        statically declared (i.e. it is initialized as part of
        C++ startup BEFORE main() is executed.  C/C++ guarantees
        that all statically declared data will be initialized
        to zero by default (see r.8.4 in C++ Programming Language,
        Second Edition).  Since the head & tail pointers are
        initialized to zero - then the C/C++ default is OK.  Why
        do I care about all this?  Because if you attempt to build
        static list(s), then the order of when the Map is
        constructed vs. when the static elements are added to the
        list is a problem!  To avoid this problem, a alternate
        constructor that does NOT initialize the head & tail pointers
        is provided.  It assumes that the pointers are already set
        zero because the list is "static". Whew!
     */
    Map( const char* ignoreThisParameter_usedToCreateAUniqueConstructor );

public: ///@name Operations to manage items in the Map
    ///@{
    /** Inserts an item into the tree.  If the node is successfully inserted,
        then the method return true.  If the tree already contains a node
        with the same key, then the method returns false.
     */
    bool  insert( ITEM& node );

    /** Removes the node (if it exists) that has the matching key. Returns
        the node removed from the tree or 0 if no key match was found.
     */
    ITEM* remove( const Key& keyOfItemToDelete );

    /** Removes the specified item from the tree.  Returns true
        when the node was found and removed; else false is
        returned (i.e. node not exists in the tree).
     */
    bool removeItem( ITEM& node );

    /** Searches for a item with a matching key.  Returns the node that
        matches, else 0.
     */
    ITEM* find( const Key& keyToFind ) const;

    /** Returns true if the specified item is in the map; else false
        is returned.
     */
    bool isInMap( ITEM& node ) const;

    /// Returns the first item in the tree. Returns 0 if tree is empty
    ITEM* first() const;

    /// Returns the last item in the tree. Returns 0 if tree is empty
    ITEM* last() const;

    /// Returns the next item in the tree.  Returns 0 if at the end-of-tree
    ITEM* next( ITEM& current ) const;

    /// Returns the previous item in the tree.  Returns 0 if at the start-of-tree
    ITEM* previous( ITEM& current ) const;

    /** Removes the first item in the list.  Returns 0 if the list
        is empty.
     */
    ITEM* getFirst();

    /** Removes the last item in the list.  Returns 0 if the list
        is empty.
     */
    ITEM* getLast();
    ///@}


public: ///@name Operations on the Map itself
    ///@{
    /** Moves the content of the this Map to the specified Map.
     */
    void move( Map<ITEM>& dst );

    /** Empties the Map.  All references to the item(s) in the
        Map are lost.
     */
    void clearTheMap();
    ///@}


private: ///@name Prevent the container from being copied
    ///@{
    /// Prevent access to the copy constructor -->Containers can not be copied!
    Map( const Map& m );

    /// Prevent access to the assignment operator -->Containers can not be copied!
    const Map& operator=( const Map& m );
    ///@}
};


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////

template <class ITEM>
Map<ITEM>::Map( const char* ignoreThisParameter_usedToCreateAUniqueConstructor )
    :m_tree( ignoreThisParameter_usedToCreateAUniqueConstructor )
{
}


template <class ITEM>
void Map<ITEM>::move( Map<ITEM>& dst )
{
    // clear the destination map
    dst.clearTheMap();

    // Copy each item (so the debug info is correct)
    ITEM* nextPtr;
    while ( (nextPtr=getFirst()) )
    {
        dst.insert( *nextPtr );
    }
}

template <class ITEM>
void Map<ITEM>::clearTheMap()
{
    // Drain map so the debug traps work correctly
    while ( getFirst() )
        ;
}


template <class ITEM>
bool Map<ITEM>::insert( ITEM& node )
{
    return m_tree.insert( node );
}


template <class ITEM>
ITEM* Map<ITEM>::remove( const Key& keyOfItemToDelete )
{
    ITEM* nodePtr = find( keyOfItemToDelete );
    if ( nodePtr )
    {
        return (ITEM*) m_tree.removeItem( *nodePtr );
    }
    return 0;
}

template <class ITEM>
bool Map<ITEM>::removeItem( ITEM& node )
{
    return m_tree.removeItem( node ) != 0;
}

template <class ITEM>
ITEM* Map<ITEM>::getFirst()
{
    ITEM* nodePtr = first();
    if ( nodePtr )
    {
        return (ITEM*) m_tree.removeItem( *nodePtr );
    }
    return 0;
}

template <class ITEM>
ITEM* Map<ITEM>::getLast()
{
    ITEM* nodePtr = last();
    if ( nodePtr )
    {
        return (ITEM*) m_tree.removeItem( *nodePtr );
    }
    return 0;
}

template <class ITEM>
ITEM* Map<ITEM>::find( const Key& keyToFind ) const
{
    return (ITEM*) m_tree.find( keyToFind );
}

template <class ITEM>
bool Map<ITEM>::isInMap( ITEM& node ) const
{
    return node.isInContainer_( this );
}

template <class ITEM>
ITEM* Map<ITEM>::first() const
{
    return (ITEM*) m_tree.first();
}

template <class ITEM>
ITEM* Map<ITEM>::last() const
{
    return (ITEM*) m_tree.last();
}

template <class ITEM>
ITEM* Map<ITEM>::next( ITEM& current ) const
{
    return (ITEM*) m_tree.next( current );
}

template <class ITEM>
ITEM* Map<ITEM>::previous( ITEM& current ) const
{
    return (ITEM*) m_tree.previous( current );
}



};      // end namespaces
};
#endif  // end header latch

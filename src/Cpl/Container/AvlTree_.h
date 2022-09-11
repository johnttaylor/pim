#ifndef Cpl_Container_AvlTree_x_h_
#define Cpl_Container_AvlTree_x_h_
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

#include "colony_config.h"
#include "Cpl/Container/MapItem.h"


/** This option sets the maximum depth of the AVL Tree.  Do NOT override the
    default value unless you REALLY know what you are doing.
 */
#ifndef OPTION_CPL_CONTAINER_AVLTREE_MAX_TREE_DEPTH
#define OPTION_CPL_CONTAINER_AVLTREE_MAX_TREE_DEPTH      32
#endif


/// 
namespace Cpl {
/// 
namespace Container {

/** This concrete class implements the core functionality of for
    AVL Binary tree (i.e. a sorted list with fast searches, inserts,
    and deletes).  Tree does not allow duplicate keys.  Clients should
    NOT USE THIS CLASS DIRECTLY, but via a Map.  The Map
    class is a wrapper that makes the AVL tree type safe.

    This class was adapted from Jonathan Bertoni C code snippet:
        'non-recursive non-allocating avl tree code'

    The original code is in the public domain and is available at:
        https://sourceforge.net/snippet/detail.php?type=snippet&id=100933
 */

class AvlTree_
{
private:
    /// Helper class so that the root node can behave like all the other nodes
    class AvlRoot : public MapItem,
        public Key
    {
    public:
        /// 
        const Key& getKey() const noexcept;
        /// 
        int compareKey( const Key& key ) const;
        /// 
        const void* getRawKey( unsigned* returnRawKeyLenPtr = 0 ) const;

    public:
        /// Constructor
        AvlRoot();

        /// Special constructor
        AvlRoot( const char* ignoreThisParameter_usedToCreateAUniqueConstructor );

    };

    /// Root Node
    AvlRoot  m_root;


public:
    /// Constructor
    AvlTree_();

    /** This is a special constructor for when the list is
        statically declared (i.e. it is initialized as part of
        C++ startup BEFORE main() is executed.  C/C++ guarantees
        that all statically declared data will be initialized
        to zero by default (see r.8.4 in C++ Programming Language,
        Second Edition).  Since the head & tail pointers are
        initialized to zero - then the C/C++ default is OK.  Why
        do I care about all this?  Because if you attempt to build
        static list(s), then the order of when the list is
        constructed vs. when the static elements are added to the
        list is a problem!  To avoid this problem, a alternate
        constructor that does NOT initialize the head & tail pointers
        is provided.  It assumes that the pointers are already set
        zero because the list is "static". Whew!
     */
    AvlTree_( const char* ignoreThisParameter_usedToCreateAUniqueConstructor );

public:
    /** Inserts an item into the tree.  If the node is successfully inserted,
        then the method return true.  If the tree already contains a node
        with the same key, then the method returns false.
     */
    bool insert( MapItem& node );

    /** Deletes the specified item from the tree.  Returns the
        node deleted.  If the delete fails (i.e. node does
        not exist in the tree), then 0 is returned.
     */
    MapItem* removeItem( MapItem& node );

    /// Searches for a item with a matching key.  Returns 0 if not found
    MapItem* find( const Key& keyToFind ) const;

    /// Returns the first item in the tree. Returns 0 if tree is empty
    MapItem* first() const;

    /// Returns the last item in the tree. Returns 0 if tree is empty
    MapItem* last() const;

    /// Returns the next item in the tree.  Returns 0 if at the end-of-tree
    static MapItem* next( MapItem& current );

    /// Returns the previous item in the tree.  Returns 0 if at the start-of-tree
    static MapItem* previous( MapItem& current );


private: // Helpers
    ///
    static bool grewLeft( MapItem* nodePtr );
    ///
    static bool grewRight( MapItem* nodePtr );
    ///
    static bool rightShorter( MapItem* nodePtr );
    ///
    static bool leftShorter( MapItem* nodePtr );
    ///
    static MapItem* rotateLeft( MapItem* nodePtr );
    ///
    static MapItem* rotateRight( MapItem* nodePtr );
    ///
    static void exchangeWithPrev( MapItem* nodePtr );

};

};      // end namespaces
};
#endif  // end header latch

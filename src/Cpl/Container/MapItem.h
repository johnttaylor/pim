#ifndef Cpl_Container_MapItem_h_
#define Cpl_Container_MapItem_h_
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

#include "Cpl/Container/DictItem.h"


/// 
namespace  Cpl {
/// 
namespace Container {


/** This abstract class represents a item that can be contained in
    an Map (aka a sorted list implemented using an AVL tree).  The client
    sub-class is required to implement the 'getKey()' method (from the
    parent class DictItem).

    NOTE: A MapItem can also be contained in a SList and/or a DList
          (but obviously not at the same time).
 */
class MapItem : public DictItem
{

protected:
    /// Magic values for balance status
    enum Balance_T { eLEFT_=-1, eEVEN_=0, eRIGHT_=1 };

protected:
    /// Link field to the parent node.
    void*   m_parentPtr_;

    /// I am the root node
    bool    m_is_NOT_root_;

    /// Balance status
    Balance_T m_balance_;

protected:
    /// Constructor
    MapItem() { initialize( 0 ); }

    /** Constructor -->special constructor to allow a Map to be
        statically allocated.  Only the Sorted List itself should ever
        use this constructor -->not intended for Items in the list
     */
    MapItem( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ):DictItem( ignoreThisParameter_usedToCreateAUniqueConstructor ) {}


    /// Initialize the node when being inserted in the tree
    inline void initialize( MapItem* parent ) { m_parentPtr_=parent; m_is_NOT_root_=true; m_balance_=eEVEN_; m_nextPtr_=0; m_prevPtr_=0; }


protected: // Helper methods 

    /// Get tree connection/pointer
    inline MapItem*    getParent() const { return (MapItem*) m_parentPtr_; }

    /// Set tree connection/pointer
    inline void        setParent( MapItem* n ) { m_parentPtr_ = n; }

    /// Get tree connection/pointer
    inline MapItem*    getLeft() const { return (MapItem*) m_prevPtr_; };

    /// Set tree connection/pointer
    inline void        setLeft( MapItem* n ) { m_prevPtr_ = n; }

    /// Get tree connection/pointer
    inline MapItem*    getRight() const { return (MapItem*) m_nextPtr_; };

    /// Set tree connection/pointer
    inline void        setRight( MapItem* n ) { m_nextPtr_ = n; }


    // Allow the AVL Tree class access to my data members
    friend class AvlTree_;

};

};      // end namespaces
};
#endif  // end header latch


#ifndef Cpl_Container_DictItem_h_
#define Cpl_Container_DictItem_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Container/Item.h"
#include "Cpl/Container/Key.h"


/// 
namespace  Cpl {
/// 
namespace Container {


/** This abstract class represents a item that can be contained in
    Dictionary.  The Dictionary is an ordered map implemented as a hash table
    that can only contain item(s) with same key type. The client sub-class is
    required to implement the 'getKey() method.

    NOTE: A DictItem can also be contained in a SList and/or a DList
          (but obviously not at the same time).
 */
class DictItem : public ExtendedItem
{
protected:
    /** This method returns a reference to the Item's Key instance.  The
        application is responsible for implementing this method.
     */
    virtual const Key& getKey() const noexcept = 0;


protected:
    /// Remember which hash bucket I am stored in
    unsigned int m_hashCode_;


protected:
    /// Constructor
    DictItem():m_hashCode_( 0 ) {}

    /** Constructor used ONLY with the child class MapItem: -->special
        constructor to allow a Map to be statically allocated.  Only the Map
        itself should ever use this constructor -->not intended for Items in a
        Map
     */
    DictItem( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ):ExtendedItem( ignoreThisParameter_usedToCreateAUniqueConstructor ) {}


    // Allow the Hash table access to me
    friend class HashTable_;

};

};      // end namespaces
};
#endif  // end header latch

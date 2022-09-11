#ifndef Cpl_Container_HashTable_x_h_
#define Cpl_Container_HashTable_x_h_
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
#include "Cpl/Container/HashFunction.h"
#include "Cpl/Container/DList.h"


/// 
namespace Cpl {
/// 
namespace Container {


/** This concrete class implements the core functionality for a Dictionary
    and/or Hash Table. Key collisions are handled by a simple link list for
    each 'hash bucket'. A default hashing algorithm is used, but the application
    can supply a different/better algorithm. Clients should NOT USE THIS CLASS
    DIRECTLY, but via the Dictionary class.  The Map class is a wrapper
    that makes the Hash Table type safe.

    NOTE: This is no checking for duplicate keys. You can insert multiple
          items with duplicates keys, but there is no guaranty on how
          those items are found in searches or removed from the table.
 */
class HashTable_ : public HashFunction
{
public:
    /// Used to return Usage/Stats information about the table
    struct Stats
    {
        ///
        unsigned long m_numItems;
        ///
        unsigned int  m_numBuckets;
        ///
        unsigned int  m_numEmptyBuckets;
        ///
        double        m_average_itemsPerBucket;
        ///
        unsigned long m_max_itemsPerBucket;
    };


private:
    /// Array of hash buckets
    DList<DictItem>*    m_buckets;

    /// Number of hash buckets
    unsigned int        m_numBuckets;

    /// Reference to the hashing function
    HashFunction&       m_hashFunc;

    /// Number of items in the table
    unsigned long       m_numItems;

public:
    /// Constructor
    HashTable_( DList<DictItem> buckets[], unsigned int numBuckets );

    /// Constructor
    HashTable_( DList<DictItem> buckets[], unsigned int numBuckets, HashFunction& userSuppliedHashFunction );


public:
    /** Inserts an item into the table.
     */
    void insert( DictItem& node );

    /** Removes the specified item from the table.  Returns the
        node removed.  If the remove fails (i.e. node does
        not exist in the table), then 0 is returned.
     */
    DictItem* removeItem( DictItem& nodeToDelete );

    /** Searches for a item with a matching key.  Returns the node that
        matches, else 0.
     */
    DictItem* find( const Key& keyToFind ) const;

    /// Returns the first item in the table. Returns 0 if table is empty
    DictItem* first() const;

    /// Returns the next item in the table.  Returns 0 if at the end-of-table
    DictItem* next( DictItem& current ) const;


public:
    /** Returns table stats.  Caller provides the memory for the stats structure.

        Note: The stats are not calculate/gathered until this method is called.
              The duration of this call is directly related to the number of
              items in the hash table.
     */
    void tableStats( Stats& tableInfo ) const;


private: // HashFunction 
    /** Default hash function.
        This one comes from 'Compiler Design in C', by Allen I. Holub,
        PRENTICE HALL, ISBN 0-13-155045-4
     */
    unsigned int hashKey( const void* keystart, unsigned keylen, unsigned int maxBuckets ) const noexcept;

};


};      // end namespaces
};
#endif  // end header latch

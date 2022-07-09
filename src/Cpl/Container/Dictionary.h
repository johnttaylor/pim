#ifndef Cpl_Container_Dictionary_h_
#define Cpl_Container_Dictionary_h_
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

#include "Cpl/Container/HashTable_.h"


/// 
namespace  Cpl {
/// 
namespace Container {


/** This template class implements a Dictionary and/or Hash Table. Hash
    Collisions are handled by a simple link list for each 'hash bucket'.

        o The application is responsible for providing memory for the
          Dictionary, i.e. must provide an array of DList<ITEM> where the
          size of the array is the 'number of buckets' in the hash table.

        o This is no checking for duplicate keys. You can insert multiple
          items with duplicates keys, but there is no guaranty on how
          those items are found in searches and/or removed from the table.

        o There is no limit to the number of items that can be
          stored in the table.  However, there is a performance
          penalty as the ratio of items vs. table buckets increases.
          Recommended size: For the default hash function use a prime
          number.  Here is a list of some useful primes:
            67, 131, 257, 521, 1031, 2053, 4099, 8209, 16411

    Template ARGS:
        ITEM  - Data type of the object stored in the dictionary.  'ITEM'
                must be a sub-class of the Cpl::Container::DictItem base
                class.
 */

template <class ITEM>
class Dictionary
{
private:
    /// Delegate operations to the generic table implementation
    HashTable_  m_table;


public:
    /// Constructor.  Note: Client provides the memory for the table
    Dictionary( DList<DictItem> buckets[], unsigned int numBuckets ):m_table( buckets, numBuckets ) {}

    /// Constructor. Note: Client provides the memory for the table 
    Dictionary( DList<DictItem> buckets[], unsigned int numBuckets, HashFunction& userSuppliedHashFunc ):m_table( buckets, numBuckets, userSuppliedHashFunc ) {}


public: ///@name Operations to manage items in the Dictionary
    ///@{
    /** Inserts an item into the table.
     */
    void insert( ITEM& node );

    /** Removes the node (if it exists) that has the matching key. Returns
        the node removed from the table or 0 if no key match was found.
     */
    ITEM* remove( const Key& keyOfObjectToDelete );

    /** Removes the specified item from the table.  Returns true
        when the node was found and removed; else false is
        returned (i.e. node not exists in the table).
     */
    bool removeItem( ITEM& node );

    /** Searches for a item with a matching key.  Returns the node that
        matches, else 0.
     */
    ITEM* find( const Key& keyToFind ) const;

    /** Returns true if the specified item is in the Dictionary; else false
        is returned.
     */
    bool isInDictionary( ITEM& node ) const;

    /// Returns the first item in the table. Returns 0 if table is empty
    ITEM* first() const;

    /** Returns the next item in the table.  Returns 0 if at the end-of-table
        Note: There is NO ORDER of the items stored in the Dictionary!
     */
    ITEM* next( ITEM& current ) const;

    /** Removes the first item in the list.  Returns 0 if the list
        is empty.
     */
    ITEM* getFirst();
    ///@}


public: ///@name Operations on the Dictionary itself
    ///@{
    /** Moves the content of the this Dictionary to the specified Dictionary.
     */
    void move( Dictionary<ITEM>& dst );

    /** Empties the Dictionary.  All references to the item(s) in the
        dictionary are lost.
     */
    void clearTheDictionary();

    /** Returns table stats.  Caller provides the memory for the stats structure.

        Note: The stats are not calculate/gathered until this method is called.
              The duration of this call is directly related to the number of
              items in the dictionary.
     */
    void stats( HashTable_::Stats& tableInfo ) const;
    ///@}


private: ///@name Prevent the container from being copied
    ///@{
    /// Prevent access to the copy constructor -->Containers can not be copied!
    Dictionary( const Dictionary& m );

    /// Prevent access to the assignment operator -->Containers can not be copied!
    const Dictionary& operator=( const Dictionary& m );
    ///@}
};


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////


template <class ITEM>
void Dictionary<ITEM>::move( Dictionary<ITEM>& dst )
{
    // clear the destination dictionary
    dst.clearTheDictionary();

    // Copy each item (so the debug info is correct)
    ITEM* nextPtr;
    while ( (nextPtr=getFirst()) )
    {
        dst.insert( *nextPtr );
    }
}

template <class ITEM>
void Dictionary<ITEM>::clearTheDictionary()
{
    // Drain dictionary so the debug traps work correctly
    while ( getFirst() )
        ;
}

template <class ITEM>
void Dictionary<ITEM>::insert( ITEM& node )
{
    m_table.insert( node );
}

template <class ITEM>
ITEM* Dictionary<ITEM>::remove( const Key& keyOfObjectToDelete )
{
    ITEM* nodePtr = find( keyOfObjectToDelete );
    if ( nodePtr )
    {
        return (ITEM*) m_table.removeItem( *nodePtr );
    }
    return 0;
}

template <class ITEM>
bool Dictionary<ITEM>::removeItem( ITEM& node )
{
    return m_table.removeItem( node ) != 0;
}

template <class ITEM>
ITEM* Dictionary<ITEM>::getFirst()
{
    ITEM* nodePtr = first();
    if ( nodePtr )
    {
        return (ITEM*) m_table.removeItem( *nodePtr );
    }
    return 0;
}


template <class ITEM>
ITEM* Dictionary<ITEM>::find( const Key& keyToFind ) const
{
    return (ITEM*) m_table.find( keyToFind );
}

template <class ITEM>
bool Dictionary<ITEM>::isInDictionary( ITEM& node ) const
{
    return node.isInContainer_( this );
}

template <class ITEM>
ITEM* Dictionary<ITEM>::first() const
{
    return (ITEM*) m_table.first();
}

template <class ITEM>
ITEM* Dictionary<ITEM>::next( ITEM& current ) const
{
    return (ITEM*) m_table.next( current );
}

template <class ITEM>
void Dictionary<ITEM>::stats( HashTable_::Stats& tableInfo ) const
{
    m_table.tableStats( tableInfo );
}


};      // end namespaces
};
#endif  // end header latch

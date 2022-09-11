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

#include "HashTable_.h"


///
using namespace Cpl::Container;


//////////////////////////////////////////////////////////////////////////////
HashTable_::HashTable_( DList<DictItem> buckets[],
                        unsigned int    numBuckets )
    :m_buckets( buckets ),
    m_numBuckets( numBuckets ),
    m_hashFunc( *this ),
    m_numItems( 0 )
{
}

HashTable_::HashTable_( DList<DictItem> buckets[],
                        unsigned int    numBuckets,
                        HashFunction&   userSuppliedHashFunction
)
    :m_buckets( buckets ),
    m_numBuckets( numBuckets ),
    m_hashFunc( userSuppliedHashFunction ),
    m_numItems( 0 )
{
}


///////////////////////////////
void HashTable_::insert( DictItem& node )
{
    // Plant the node in the indexed bucket.  Note: There is NO difference in 
    // the handling of an empty-bucket vs. collision scenario since each 
    // bucket IS a list.
    unsigned     keylength = 0;
    const void*  keyptr    = node.getKey().getRawKey( &keylength );
    unsigned int index     = m_hashFunc.hashKey( keyptr, keylength, m_numBuckets );
    node.m_hashCode_       = index;
    m_buckets[index].put( node );
    m_numItems++;
}

DictItem* HashTable_::find( const Key& keyToFind ) const
{
    // Hash the key
    unsigned         keylength = 0;
    const void*      keyptr    = keyToFind.getRawKey( &keylength );
    unsigned int     index     = m_hashFunc.hashKey( keyptr, keylength, m_numBuckets );
    DList<DictItem>& list      = m_buckets[index];
    DictItem*        itemPtr   = list.first();

    // Traverse the hash bucket's list looking for a match
    while ( itemPtr )
    {
        if ( itemPtr->getKey().compareKey( keyToFind ) == 0 )
        {
            break;
        }
        itemPtr = list.next( *itemPtr );
    }

    // Return the matching item (or 0 if not one found)
    return itemPtr;
}

DictItem* HashTable_::removeItem( DictItem& nodeToDelete )
{
    // Remove the node
    unsigned int index = nodeToDelete.m_hashCode_;
    if ( m_buckets[index].remove( nodeToDelete ) )
    {
        m_numItems--;
        return &nodeToDelete;
    }

    return 0;
}

DictItem* HashTable_::first() const
{
    // Trap empty table
    if ( m_numItems )
    {
        // Find the first non-empty bucket in the table
        unsigned int i;
        for ( i=0; i < m_numBuckets; i++ )
        {
            DictItem* itemPtr = m_buckets[i].first();
            if ( itemPtr )
            {
                return itemPtr;
            }
        }
    }

    // Empty table -->return null
    return 0;
}

DictItem* HashTable_::next( DictItem& current ) const
{
    // Case: There are more item(s) in the current bucket
    unsigned int index   = current.m_hashCode_;
    DictItem*    nextPtr = m_buckets[index].next( current );
    if ( nextPtr )
    {
        return nextPtr;
    }

    // Case: Move to the next non-empty bucket
    while ( ++index < m_numBuckets )
    {
        if ( (nextPtr=m_buckets[index].first()) )
        {
            return nextPtr;
        }
    }

    // If I get here, then there are no more items in the table
    return 0;
}

//////////////////////////////
void HashTable_::tableStats( HashTable_::Stats& info ) const
{
    // Initialize stats structure
    info.m_numItems               = m_numItems;
    info.m_numBuckets             = m_numBuckets;
    info.m_numEmptyBuckets        = m_numBuckets;
    info.m_average_itemsPerBucket = 0.0;
    info.m_max_itemsPerBucket     = 0;
    if ( !m_numItems )
    {
        return;
    }

    // Walk all buckets
    unsigned int  i;
    unsigned int  filledBuckets = 0;
    unsigned long sumItemCount  = 0;
    for ( i=0; i < m_numBuckets; i++ )
    {
        DictItem* itemPtr = m_buckets[i].first();
        if ( itemPtr )
        {
            info.m_numEmptyBuckets--;
            filledBuckets++;
            unsigned long itemCount = 0;
            while ( itemPtr )
            {
                itemCount++;
                itemPtr = m_buckets[i].next( *itemPtr );
            }
            sumItemCount += itemCount;
            if ( itemCount > info.m_max_itemsPerBucket )
            {
                info.m_max_itemsPerBucket = itemCount;
            }
        }
    }

    // Calc average 
    info.m_average_itemsPerBucket = ((double) sumItemCount) / ((double) filledBuckets);
}


//////////////////////////////
unsigned int HashTable_::hashKey( const void* keystart, unsigned keylen, unsigned int maxBuckets ) const noexcept
{
    static const unsigned int bits_in_unsigned     = sizeof( unsigned int ) * 8;
    static const unsigned int seventy_five_percent = (unsigned int) (bits_in_unsigned * .75);
    static const unsigned int twelve_percent       = (unsigned int) (bits_in_unsigned * .125);
    static const unsigned int high_bits            = ~((unsigned int) (~0) >> twelve_percent);

    unsigned char* ptr = (unsigned char*) keystart;
    unsigned       i   = 0;
    unsigned int   key = 0;
    unsigned int   g;
    while ( i++ < keylen )
    {
        key = (key << twelve_percent) + (unsigned int) (*ptr++);
        if ( (g=key & high_bits) )
        {
            key = (key ^ (g >> seventy_five_percent)) & ~high_bits;
        }
    }
    return key % maxBuckets;
}

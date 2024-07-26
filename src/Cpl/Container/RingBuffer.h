#ifndef Cpl_Container_RingBuffer_h_
#define Cpl_Container_RingBuffer_h_
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

#include <stdint.h>


/// 
namespace  Cpl {
/// 
namespace Container {


/** This template class implements a Ring Buffer.  The size of the
    ring buffer is limited by number of bits in platform's 'unsigned'
    data type.

    Thread/ISR Safety Notes:
        - The only mutable (once the Ring buffer is created) data members of the
          class are the head/tail pointers and the buffer contents.
        - The add() operation only modifies the tail pointer (but does read the
          head pointer)
        - The remove() operation only modifies the head pointer (but does read
          the tail pointer).
        - The clearTheBuffer() operation modifies both the head and tail pointers
        - All other methods only read data members
        - The implementation ASSUMES there is a single producer and a single
          consumer of the RingBuffer.  And that ONLY the producer/consumer
          are the invoking operations on the RingBuffer.

    Template Args:
        ITEM:=      Type of the data stored in the Ring Buffer
 */
template <class ITEM>
class RingBuffer
{
private:
    /// Points to the first item in the buffer.
    ITEM*           m_headPtr;

    /// Points to the last item in the buffer.
    ITEM*           m_tailPtr;

    /// Number of element in the allocate memory
    const unsigned  m_memoryNumElements;

    /// End of the Memory element storage
    ITEM* const     m_endOfMemPtr;

    /// Memory for the Elements
    ITEM* const     m_elements;


public:
    /** Constructor.  The application is responsible for providing the memory
        for the Ring Buffer.  The argument ''numElements' is the number of
        items that will fit in the memory allocated by 'memoryForElements' - it
        is NOT the number of bytes of 'memoryForElements'.

        Note: The maximum number of element that can actually be stored is
              numElements - 1 (one element/index/slot is consumed/used to
              represents the empty buffer state).
     */
    RingBuffer( unsigned numElements, ITEM memoryForElements[] ) noexcept;



public:
    /** Removes the first item in the Buffer. The contents of the
        removed item will be copied into the 'dst' argument. The method
        return true if the operation was successful; else false is
        returned, i.e. the Ring buffer is/was empty.
     */
    bool remove( ITEM& dst ) noexcept;


    /** The contents of 'item' will be copied into the Ring Buffer as the
        'last' item in the  buffer. Return true if the operation was
        successful; else false is returned, i.e. the Buffer was full prior to
        the attempted add().
     */
    bool add( const ITEM& item ) noexcept;


    /** Returns a pointer to the first item in the Buffer.
        The returned item remains in the buffer.  Returns 0
        if the Buffer is empty.
     */
    ITEM* peekHead( void ) const noexcept;


    /** Returns a pointer to the last item in the Buffer.
        The returned item remains in the Buffer.  Returns 0
        if the Buffer is empty.
     */
    ITEM* peekTail( void ) const noexcept;


public:
    /** This method returns true if the Ring Buffer is empty
     */
    bool isEmpty( void ) const noexcept;

    /** This method returns true if the Ring Buffer is full
     */
    bool isFull( void ) const noexcept;


    /** This method returns the current number of items in
        the Ring Buffer
     */
    unsigned getNumItems( void ) const noexcept;


    /** This method returns the maximum number of items that
        can be stored in the Ring buffer.
     */
    unsigned getMaxItems( void ) const noexcept;



public:
    /** Empties the Ring Buffer.  All references to the item(s) in the
        buffer are lost.
     */
    void clearTheBuffer() noexcept;

public:
    /** This method returns a pointer to the next item to be removed. In addition
        it returns the number of elements that can be removed as linear/flat
        buffer (i.e. without wrapping around raw buffer memory)

        If the Ring buffer is empty, a null pointer is returned
     */
    ITEM* peekNextRemoveItems( unsigned& dstNumFlatElements ) noexcept;

    /** This method 'removes' N elements - that were removed using the
        pointer returned from peekNextRemoveItems - from the ring buffer.  
        Basically it updates the head pointer to reflect items removed using 
        direct memory access.

        'numElementsToRemove' be less than or equal to the 'dstNumFlatElements'
        returned from peekNextRemoveItems().

        CAUTION: IF YOU DON'T UNDERSTAND THE USE CASE FOR THIS METHOD - THEN 
                 DON'T USE IT.  If this method is used improperly, it WILL
                 CORRUPT the Ring Buffer!
     */
    void removeElements( unsigned numElementsToRemove ) noexcept;

public:
    /** This method returns a pointer to the next item to be added. In addition
        it returns the number of elements that can be added as linear/flat
        buffer (i.e. without wrapping around raw buffer memory)

        If the Ring buffer is full, a null pointer is returned
     */
    ITEM* peekNextAddItems( unsigned& dstNumFlatElements ) noexcept;

    /** This method 'adds' N elements - that were populated using the
        pointer returned from peekNextAddItems - to the ring buffer.  Basically
        its updates the tail pointer to reflect items added using direct
        memory access.

        'numElementsAdded' be less than or equal to the 'dstNumFlatElements'
        returned from peekNextAddItems().

        CAUTION: IF YOU DON'T UNDERSTAND THE USE CASE FOR THIS METHOD - THEN 
                 DON'T USE IT. If this method is used improperly, it WILL
                 CORRUPT the Ring Buffer!
     */
    void addElements( unsigned numElementsAdded ) noexcept;

private:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    RingBuffer( const RingBuffer& m );

    /// Prevent access to the assignment operator -->Containers can not be copied!
    const RingBuffer& operator=( const RingBuffer& m );

};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////


template <class ITEM>
RingBuffer<ITEM>::RingBuffer( unsigned maxElements, ITEM memoryForElements[] ) noexcept
    : m_headPtr( memoryForElements )
    , m_tailPtr( memoryForElements )
    , m_memoryNumElements( maxElements )
    , m_endOfMemPtr( memoryForElements + maxElements )
    , m_elements( memoryForElements )
{
}


template <class ITEM>
inline void RingBuffer<ITEM>::clearTheBuffer() noexcept
{
    m_headPtr = m_tailPtr = m_elements;
}


template <class ITEM>
inline bool RingBuffer<ITEM>::add( const ITEM& item ) noexcept
{
    if ( isFull() )
    {
        return false;
    }

    *m_tailPtr = item;
    if ( ++m_tailPtr >= m_endOfMemPtr )
    {
        m_tailPtr = m_elements;
    }

    return true;
}

template <class ITEM>
inline bool RingBuffer<ITEM>::remove( ITEM& dst ) noexcept
{
    if ( isEmpty() )
    {
        return false;
    }

    dst = *m_headPtr;
    if ( ++m_headPtr >= m_endOfMemPtr )
    {
        m_headPtr = m_elements;
    }
    return true;
}


template <class ITEM>
inline ITEM* RingBuffer<ITEM>::peekNextRemoveItems( unsigned& dstNumFlatElements ) noexcept
{
    if ( isEmpty() )
    {
        return nullptr;
    }

    unsigned totalNumElements = getNumItems();
    dstNumFlatElements        = m_endOfMemPtr - m_headPtr;
    if ( dstNumFlatElements > totalNumElements )
    {
        dstNumFlatElements = totalNumElements;
    }

    return m_headPtr;
}

template <class ITEM>
inline void RingBuffer<ITEM>::removeElements( unsigned numElementsToRemove ) noexcept
{
    // By the defined semantics - I simply update the head pointer
    m_headPtr += numElementsToRemove;
    if ( m_headPtr >= m_endOfMemPtr )
    {
        m_headPtr = m_elements;
    }
}

template <class ITEM>
inline ITEM* RingBuffer<ITEM>::peekNextAddItems( unsigned& dstNumFlatElements ) noexcept
{
    if ( isFull() )
    {
        dstNumFlatElements = 0;
        return nullptr;
    }

    unsigned totalAvailElements = getMaxItems() - getNumItems();
    dstNumFlatElements          = m_endOfMemPtr - m_tailPtr;
    if ( dstNumFlatElements > totalAvailElements )
    {
        dstNumFlatElements = totalAvailElements;
    }

    return m_tailPtr;
}

template <class ITEM>
inline void RingBuffer<ITEM>::addElements( unsigned numElementsAdded ) noexcept
{
    // By the defined semantics - I simply update the tail pointer
    m_tailPtr += numElementsAdded;
    if ( m_tailPtr >= m_endOfMemPtr )
    {
        m_tailPtr = m_elements;
    }
}

template <class ITEM>
inline ITEM* RingBuffer<ITEM>::peekHead( void ) const noexcept
{
    if ( isEmpty() )
    {
        return 0;
    }

    return m_headPtr;
}

template <class ITEM>
inline ITEM* RingBuffer<ITEM>::peekTail( void ) const noexcept
{
    if ( isEmpty() )
    {
        return 0;
    }

    ITEM* prevElem = m_tailPtr - 1;
    if ( prevElem < m_elements )
    {
        prevElem = m_endOfMemPtr - 1;
    }

    return prevElem;
}


template <class ITEM>
inline bool RingBuffer<ITEM>::isEmpty( void ) const noexcept
{
    return m_headPtr == m_tailPtr;
}

template <class ITEM>
inline bool RingBuffer<ITEM>::isFull( void ) const noexcept
{
    ITEM* nextElem = m_tailPtr + 1;
    if ( nextElem >= m_endOfMemPtr )
    {
        nextElem = m_elements;
    }
    return nextElem == m_headPtr;
}

template <class ITEM>
inline unsigned RingBuffer<ITEM>::getNumItems( void ) const noexcept
{
    unsigned headIdx = (unsigned) (m_headPtr - m_elements);
    unsigned tailIdx = (unsigned) (m_tailPtr - m_elements);
    if ( tailIdx < headIdx )
    {
        tailIdx += m_memoryNumElements;
    }
    return tailIdx - headIdx;
}

template <class ITEM>
inline unsigned RingBuffer<ITEM>::getMaxItems( void ) const noexcept
{
    return m_memoryNumElements - 1;   // One elem/slot is reserved for the empty-list condition
}



};      // end namespaces
};
#endif  // end header latch

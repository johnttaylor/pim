#ifndef Cpl_Container_RingBuffer_h_
#define Cpl_Container_RingBuffer_h_
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

#include <stdint.h>


/// 
namespace  Cpl {
/// 
namespace Container {


/** This template class implements a Ring Buffer.  The size of the
    ring buffer is limited by number of bits in platform's 'unsigned'
    data type.

    Template Args:
        ITEM:=      Type of the data stored in the Ring Buffer
 */
template <class ITEM>
class RingBuffer
{
private:
    /// Points to the first item in the buffer.
    ITEM * m_headPtr;

    /// Points to the last item in the buffer.
    ITEM*    m_tailPtr;

    /// Current number of items in the buffer
    unsigned m_count;

    /// Max number of elements
    unsigned m_max;

    /// End of the Memory element storage
    ITEM*    m_endOfMemPtr;

    /// Memory for the Elements
    ITEM*    m_elements;


public:
    /** Constructor.  The application is responsible for providing the memory
        for the Ring Buffer.  The argument ''maxElements' is the number of
        items that will fit in the memory allocated by 'memoryForElements' - it
        is NOT the number of bytes of 'memoryForElements'.
     */
    RingBuffer( unsigned maxElements, ITEM memoryForElements[] ) noexcept;



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
    :m_headPtr( 0 ), m_tailPtr( 0 ), m_count( 0 ), m_max( maxElements ), m_endOfMemPtr( memoryForElements + maxElements ), m_elements( memoryForElements )
{
}


template <class ITEM>
inline void RingBuffer<ITEM>::clearTheBuffer() noexcept
{
    m_count = 0;
}


template <class ITEM>
inline bool RingBuffer<ITEM>::add( const ITEM& item ) noexcept
{
    if ( isFull() )
    {
        return false;
    }

    if ( isEmpty() )
    {
        m_headPtr = m_tailPtr = m_elements;
    }
    else
    {
        if ( ++m_tailPtr >= m_endOfMemPtr )
        {
            m_tailPtr = m_elements;
        }
    }

    *m_tailPtr = item;
    m_count++;
    return true;
}

template <class ITEM>
inline bool RingBuffer<ITEM>::remove( ITEM& dst ) noexcept
{
    if ( isEmpty() )
    {
        return false;
    }

    m_count--;
    dst = *m_headPtr;
    if ( ++m_headPtr >= m_endOfMemPtr )
    {
        m_headPtr = m_elements;
    }
    return true;
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

    return m_tailPtr;
}


template <class ITEM>
inline bool RingBuffer<ITEM>::isEmpty( void ) const noexcept
{
    return m_count == 0;
}

template <class ITEM>
inline bool RingBuffer<ITEM>::isFull( void ) const noexcept
{
    return m_count == m_max;
}

template <class ITEM>
inline unsigned RingBuffer<ITEM>::getNumItems( void ) const noexcept
{
    return m_count;
}

template <class ITEM>
inline unsigned RingBuffer<ITEM>::getMaxItems( void ) const noexcept
{
    return m_max;
}



};      // end namespaces
};
#endif  // end header latch

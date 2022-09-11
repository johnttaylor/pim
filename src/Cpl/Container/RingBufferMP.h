#ifndef Cpl_Container_RingBuffer_MP_h_
#define Cpl_Container_RingBuffer_MP_h_
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

#include "Cpl/Container/RingBufferMT.h"
#include "Cpl/Dm/Mp/Uint32.h"


/// 
namespace  Cpl {
/// 
namespace Container {


/** This template class implements a THREAD SAFE Ring Buffer AND it maintains/reports
    the number of elements stored in the ring buffer via a Model Point.  The
    size of the ring buffer is limited by number of bits in platform's 'unsigned'
    data type.

    Template Args:
        ITEM:=      Type of the data stored in the Ring Buffer
 */
template <class ITEM>
class RingBufferMP : public RingBufferMT<ITEM>
{
public:
    /** Constructor.  The application is responsible for providing the memory
        for the Ring Buffer.  The argument ''maxElements' is the number of
        items that will fit in the memory allocated by 'memoryForElements' - it
        is NOT the number of bytes of 'memoryForElements'.
     */
    RingBufferMP( unsigned maxElements, ITEM memoryForElements[], Cpl::Dm::Mp::Uint32& mpElementCount ) noexcept
        : RingBufferMT<ITEM>( maxElements, memoryForElements )
        , m_mpElementCount( mpElementCount )
    {
        // NOTE: I would really like to initialize the model point here in the constructor - BUT
        //       if this class is statically created/allocated - that is problem because the CPL
        //       Services may or may not be initialized/working.  So the work-around is to
        //       attempt a lazy initialization of the elementCount model point.
    }



public:
    /// See Cpl::Container::RingBuffer.
    bool remove( ITEM& dst ) noexcept
    {
        bool result = RingBufferMT<ITEM>::remove( dst );
        if ( result )
        {
            decrementMp();
        }
        return result;
    }

    /// Extends remove() to expose/return the MP's sequence number on the update
    bool remove( ITEM& dst, uint16_t& seqNum ) noexcept
    {
        bool result = RingBufferMT<ITEM>::remove( dst );
        if ( result )
        {
            seqNum = decrementMp();
        }
        return result;
    }

    /// See Cpl::Container::RingBuffer.
    bool add( const ITEM& item ) noexcept
    {
        bool result = RingBufferMT<ITEM>::add( item );
        if ( result )
        {
            incrementMp();
        }
        return result;
    }

    /// Extends add() to expose/return the MP's sequence number on the update
    bool add( const ITEM& item, uint16_t& seqNum ) noexcept
    {
        bool result = RingBufferMT<ITEM>::add( item );
        if ( result )
        {
            seqNum =  incrementMp();
        }
        return result;
    }

public:
    /// See Cpl::Container::RingBuffer.
    void clearTheBuffer() noexcept
    {
        RingBufferMT<ITEM>::clearTheBuffer();
        m_mpElementCount.write( 0 );
    }

    /// Extends add() to expose/return the MP's sequence number on the update
    void clearTheBuffer( uint16_t& seqNum ) noexcept
    {
        RingBufferMT<ITEM>::clearTheBuffer();
        seqNum = m_mpElementCount.write( 0 );
    }

protected:
    /// helper method to increment the MP
    uint16_t incrementMp() noexcept
    {
        // Increment the count -->but do a 'lazy initialization' to the MP if was not previously initialized
        if ( m_mpElementCount.isNotValid() )
        {
            return m_mpElementCount.write( 1 );
        }
        return m_mpElementCount.increment();
    }

    /// helper method to decrement the MP
    uint16_t decrementMp() noexcept
    {
        // Decrement the count -->but do a 'lazy initialization' to the MP if was not previously initialized
        if ( m_mpElementCount.isNotValid() )
        {
            return m_mpElementCount.write( 0 );
        }
        return m_mpElementCount.decrement();
    }

private:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    RingBufferMP( const RingBufferMP& m );

    /// Prevent access to the assignment operator -->Containers can not be copied!
    const RingBufferMP& operator=( const RingBufferMP& m );

public:
    /// Model point to report my element count.  NOTE: Public access is allowed to simply the application subscribing/accessing the MP
    Cpl::Dm::Mp::Uint32& m_mpElementCount;
};



};      // end namespaces
};
#endif  // end header latch

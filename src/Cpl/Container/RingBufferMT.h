#ifndef Cpl_Container_RingBuffer_MT_h_
#define Cpl_Container_RingBuffer_MT_h_
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

#include "Cpl/Container/RingBuffer.h"
#include "Cpl/System/Mutex.h"

/// 
namespace  Cpl {
/// 
namespace Container {


/** This template class implements a THREAD SAFE Ring Buffer.  The size of the
    ring buffer is limited by number of bits in platform's 'unsigned'
    data type.

    Template Args:
        ITEM:=      Type of the data stored in the Ring Buffer
 */
template <class ITEM>
class RingBufferMT: public RingBuffer<ITEM>
{
public:
    /** Constructor.  The application is responsible for providing the memory
        for the Ring Buffer.  The argument ''maxElements' is the number of
        items that will fit in the memory allocated by 'memoryForElements' - it
        is NOT the number of bytes of 'memoryForElements'.
     */
    RingBufferMT( unsigned maxElements, ITEM memoryForElements[] ) noexcept :RingBuffer<ITEM>( maxElements, memoryForElements ) {}



public:
    /// See Cpl::Container::RingBuffer.
    bool remove( ITEM& dst ) noexcept { Cpl::System::Mutex::ScopeBlock criticalSection( m_lock ); return RingBuffer<ITEM>::remove( dst ); }

    /// See Cpl::Container::RingBuffer.
    bool add( const ITEM& item ) noexcept { Cpl::System::Mutex::ScopeBlock criticalSection( m_lock ); return RingBuffer<ITEM>::add( item ); }

    /// See Cpl::Container::RingBuffer.
    ITEM* peekHead( void ) const noexcept { Cpl::System::Mutex::ScopeBlock criticalSection( *((Cpl::System::Mutex*)&m_lock) ); return RingBuffer<ITEM>::peekHead(); }

    /// See Cpl::Container::RingBuffer.
    ITEM* peekTail( void ) const noexcept { Cpl::System::Mutex::ScopeBlock criticalSection( *((Cpl::System::Mutex*)&m_lock) ); return RingBuffer<ITEM>::peekTail(); }

public:
    /** This method returns a pointer to the next item to be added. In addition
        it returns the number of elements that can be added as linear/flat
        buffer (i.e. without wrapping around raw buffer memory)

        If the Ring buffer is full, a null pointer is returned (and 'dstNumFlatElements'
        is set to zero). 

        This method with addElements() method provide an ATOMIC update of 
        the ring buffer.  This means that addElements() MUST ALWAYS be called
        after calling peekNextAddItems().
     */
    ITEM* peekNextAddItems( unsigned& dstNumFlatElements ) noexcept { m_lock.lock(); return RingBuffer<ITEM>::peekNextAddItems( dstNumFlatElements );}

    /** This method 'adds' N elements - that were populated using the
        pointer returned from peekNextAddItems - to the ring buffer.  Basically
        its updates the tail pointer to reflect items added using direct
        memory access.

        'numElementsAdded' be less than or equal to the 'dstNumFlatElements'
        returned from peekNextAddItems(). If the call to peekNextAddTiems, THEN
        'numElementsAdded' MUST BE SET to zero.

        CAUTION: 
            1. IF YOU DON'T UNDERSTAND THE USE CASE FOR THIS METHOD - THEN
               DON'T USE IT. If this method is used improperly, it WILL
               CORRUPT the Ring Buffer!
            2. MUST ALWAYS BE CALLED following a call to peekNextAddItems().
     */
    void addElements( unsigned numElementsAdded ) noexcept { RingBuffer<ITEM>::addElements( numElementsAdded ); m_lock.unlock(); }

public:
    /** This method returns a pointer to the next item to be removed. In addition
        it returns the number of elements that can be removed as linear/flat
        buffer (i.e. without wrapping around raw buffer memory)

        If the Ring buffer is empty, a null pointer is returned (and 'dstNumFlatElements'
        is set to zero). 

        This method with removeElements() method provide an ATOMIC update of
        the ring buffer.  This means that removeElements() MUST ALWAYS be called
        after calling peekNextRemoveItems().
     */
    ITEM* peekNextRemoveItems( unsigned& dstNumFlatElements ) noexcept { m_lock.lock(); return RingBuffer<ITEM>::peekNextRemoveItems( dstNumFlatElements ); }

    /** This method 'removes' N elements - that were removed using the
        pointer returned from peekNextRemoveItems - from the ring buffer.
        Basically it updates the head pointer to reflect items removed using
        direct memory access.

        'numElementsToRemove' be less than or equal to the 'dstNumFlatElements'
        returned from peekNextRemoveItems().

        CAUTION: 
            1. IF YOU DON'T UNDERSTAND THE USE CASE FOR THIS METHOD - THEN
               DON'T USE IT.  If this method is used improperly, it WILL
               CORRUPT the Ring Buffer!
            2. MUST ALWAYS BE CALLED following a call to peekNextRemoveItems().
     */
    void removeElements( unsigned numElementsToRemove ) noexcept { RingBuffer<ITEM>::removeElements( numElementsToRemove ); m_lock.unlock(); }


public:
    /// See Cpl::Container::RingBuffer.
    bool isEmpty( void ) const noexcept { Cpl::System::Mutex::ScopeBlock criticalSection( *((Cpl::System::Mutex*)&m_lock) ); return RingBuffer<ITEM>::isEmpty(); }

    /// See Cpl::Container::RingBuffer.
    bool isFull( void ) const noexcept { Cpl::System::Mutex::ScopeBlock criticalSection( *((Cpl::System::Mutex*)&m_lock) ); return RingBuffer<ITEM>::isFull(); }

    /// See Cpl::Container::RingBuffer.
    unsigned getNumItems( void ) const noexcept { Cpl::System::Mutex::ScopeBlock criticalSection( *((Cpl::System::Mutex*)&m_lock) ); return RingBuffer<ITEM>::getNumItems(); }

    /// See Cpl::Container::RingBuffer.
    unsigned getMaxItems( void ) const noexcept { Cpl::System::Mutex::ScopeBlock criticalSection( *((Cpl::System::Mutex*)&m_lock) ); return RingBuffer<ITEM>::getMaxItems(); }

public:
    /// See Cpl::Container::RingBuffer.
    void clearTheBuffer() noexcept { Cpl::System::Mutex::ScopeBlock criticalSection( m_lock ); RingBuffer<ITEM>::clearTheBuffer(); }



private:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    RingBufferMT( const RingBufferMT& m );

    /// Prevent access to the assignment operator -->Containers can not be copied!
    const RingBufferMT& operator=( const RingBufferMT& m );

protected:
    /// Mutex for critical sections
    Cpl::System::Mutex m_lock;
};


};      // end namespaces
};
#endif  // end header latch

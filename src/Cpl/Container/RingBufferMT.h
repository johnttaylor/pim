#ifndef Cpl_Container_RingBuffer_MT_h_
#define Cpl_Container_RingBuffer_MT_h_
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

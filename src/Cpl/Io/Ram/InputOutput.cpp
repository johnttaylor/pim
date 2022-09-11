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

#include "InputOutput.h"

//
using namespace Cpl::Io::Ram;



//////////////////////
InputOutput::InputOutput( void* rawMemory, unsigned memorySizeInBytes )
    : m_ringBuffer( memorySizeInBytes, (uint8_t*) rawMemory )
    , m_writersWaiting( 0 )
    , m_readersWaiting( 0 )
    , m_opened( true )
{
}

InputOutput::~InputOutput()
{
}

void InputOutput::reset()
{
    Cpl::System::Mutex::ScopeBlock lock( m_lock );
    if ( m_opened )
    {
        m_ringBuffer.clearTheBuffer();
    }
}

//////////////////////
bool InputOutput::read( void* buffer, int numBytes, int& bytesRead )
{
    // Fail if closed/eos
    m_lock.lock();
    bool closed = !m_opened;
    m_lock.unlock();
    if ( closed )
    {
        return false;
    }

    // Do nothing if requested to read zero bytes
    bytesRead = 0;
    if ( numBytes == 0 )
    {
        return true;
    }

    // Attempt to read at least one byte
    uint8_t* dstPtr = (uint8_t*) buffer;
    m_lock.lock();
    while ( numBytes && m_opened )
    {
        // Remove the next byte to the buffer
        if ( !m_ringBuffer.remove( *dstPtr ) )
        {
            // Buffer is empty -->have we read at least on byte?
            if ( bytesRead > 0 )
            {
                // YES -->return
                m_lock.unlock();
                return true;
            }

            // Block until there is data in the Buffer
            else
            {
                // Wait to be signal, then try again
                m_readersWaiting++;
                m_lock.unlock();
                m_semaReaders.wait();
                m_lock.lock();
                continue;
            }
        }

        // Removed data from the buffer -->check for waiters
        else
        {
            for ( ; m_writersWaiting; m_writersWaiting-- )
            {
                m_semaWriters.signal();
            }
        }

        // try the next byte
        bytesRead++;
        numBytes--;
        dstPtr++;
    }
    m_lock.unlock();

    return true;
}

bool InputOutput::available()
{
    Cpl::System::Mutex::ScopeBlock lock( m_lock );
    return m_opened && !m_ringBuffer.isEmpty();
}


//////////////////////
bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    // Fail if closed/eos
    m_lock.lock();
    bool closed = !m_opened;
    m_lock.unlock();
    if ( closed )
    {
        return false;
    }

    // Do nothing if requested to write zero bytes
    bytesWritten = 0;
    if ( maxBytes == 0 )
    {
        return true;
    }

    // Attempt to write at least one byte
    const uint8_t* srcPtr = (const uint8_t*) buffer;
    m_lock.lock();
    while ( maxBytes && m_opened )
    {
        // Add the next byte to the buffer
        if ( !m_ringBuffer.add( *srcPtr ) )
        {
            // Buffer is full -->have we written at least on byte?
            if ( bytesWritten > 0 )
            {
                // YES -->return
                m_lock.unlock();
                return true;
            }

            // Block until there is room in the Buffer
            else
            {
                // Wait to be signal, then try again
                m_writersWaiting++;
                m_lock.unlock();
                m_semaWriters.wait();
                m_lock.lock();
                continue;
            }
        }

        // Added data from the buffer -->check for waiters
        else
        {
            for ( ; m_readersWaiting; m_readersWaiting-- )
            {
                m_semaReaders.signal();
            }
        }

        // try the next byte
        bytesWritten++;
        maxBytes--;
        srcPtr++;
    }

    bool result = m_opened;
    m_lock.unlock();
    return result;
}


void InputOutput::flush()
{
    // Has no meaning
}

bool InputOutput::isEos()
{
    Cpl::System::Mutex::ScopeBlock lock( m_lock );
    return !m_opened;
}

void InputOutput::close()
{
    Cpl::System::Mutex::ScopeBlock lock( m_lock );
    if ( m_opened )
    {
        m_opened = false;

        // Free-up any/all write waiters
        for ( ; m_writersWaiting; m_writersWaiting-- )
        {
            m_semaWriters.signal();
        }
        // Free-up any/all read waiters
        for ( ; m_readersWaiting; m_readersWaiting-- )
        {
            m_semaReaders.signal();
        }
    }
}

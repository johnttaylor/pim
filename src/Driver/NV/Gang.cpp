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

#include "Gang.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Api.h"

using namespace Driver::NV;

//////////////////////////////////////////////////////////////////////////////
Gang::Gang( Api* drivers[] )
    : m_drivers( drivers )
    , m_numPages( 0 )
    , m_pageSize( 0 )
    , m_totalSize( 0 )
    , m_started( false )
{
    CPL_SYSTEM_ASSERT( drivers );

    // Calculate total size
    m_pageSize = size_t( -1 );
    while ( *drivers != nullptr )
    {
        m_totalSize += (*drivers)->getTotalSize();

        // Trap the smallest page size
        if ( (*drivers)->getPageSize() < m_pageSize )
        {
            m_pageSize = (*drivers)->getPageSize();
        }
        drivers++;
    }

    // Derive number pages
    if ( m_pageSize != 0 && m_pageSize != size_t( -1 ) )
    {
        m_numPages = m_totalSize / m_pageSize;
    }
    
    // This will only happen if the supplied driver list is EMPTY
    else
    {
        m_pageSize = 0;
    }
}

bool Gang::start() noexcept
{
    // Skip processing if already started
    if ( !m_started )
    {
        // Start the individual drivers
        Api** drivers = m_drivers;
        while ( *drivers != nullptr )
        {
            if ( !(*drivers)->start() )
            {
                return false;
            }
            drivers++;
        }

        m_started = true;
        return true;
    }

    return false;
}

void Gang::stop() noexcept
{
    // Skip processing if already stopped
    if ( m_started )
    {
        m_started = false;

        // Start the individual drivers
        Api** drivers = m_drivers;
        while ( *drivers != nullptr )
        {
            (*drivers)->stop();
            drivers++;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
bool Gang::write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept
{
    // Fail if not started or write is out of range
    if ( !m_started ||
         (dstOffset + numBytesToWrite) > m_totalSize )
    {
        return false;
    }

    // Find the starting driver
    size_t endOffset   = 0;
    size_t beginOffset = 0;
    Api**  drivers     = m_drivers;
    while ( *drivers != nullptr )
    {
        // Find the 'starting' page/driver
        size_t driverTotalSize = (*drivers)->getTotalSize();
        endOffset             += (*drivers)->getTotalSize();;
        if ( dstOffset < endOffset )
        {
            // Normalize the dstOffset and write the data
            dstOffset   -= beginOffset;

            // Loop through as many drivers as needed to complete the write
            const uint8_t* srcPtr         = (const uint8_t*) srcData;
            while ( numBytesToWrite && *drivers != nullptr )
            {
                bool result = driverWrite( drivers, dstOffset, srcPtr, numBytesToWrite );
                if ( !result )
                {
                    return false;
                }

                // Advance to the next driver
                drivers++;
                dstOffset = 0;
            }
            
            // Note: numBytesToWrite should always be zero.  But if some how we
            // ran out of drivers before the write completed ->fail the 
            // transaction
            return numBytesToWrite == 0;
        }

        // Next driver
        beginOffset += driverTotalSize;
        drivers++;
    }

    // If get here - no byte were written
    return false;
}

bool Gang::driverWrite( Api**           drivers,
                        size_t          normalizedDstOffset,
                        const uint8_t*& srcData,
                        size_t&         numBytesToWrite ) noexcept
{
    // Start writing bytes using the current driver
    size_t bytesThisDriver = numBytesToWrite;
    if ( normalizedDstOffset + numBytesToWrite > (*drivers)->getTotalSize() )
    {
        bytesThisDriver = (*drivers)->getTotalSize() - normalizedDstOffset;
        numBytesToWrite = numBytesToWrite - bytesThisDriver;
    }
    else
    {
        numBytesToWrite = 0;
    }
    bool result = (*drivers)->write( normalizedDstOffset, srcData, bytesThisDriver );
    srcData    += bytesThisDriver;
    return result;
}

//////////////////////////////////////////////////////////////////////////////
bool Gang::read( size_t srcOffset, void* dstData, size_t numBytesToRead ) noexcept
{
    // Fail if not started or read is out of range
    if ( !m_started ||
         (srcOffset + numBytesToRead) > m_totalSize )
    {
        return false;
    }

    // Find the starting driver
    size_t endOffset   = 0;
    size_t beginOffset = 0;
    Api**  drivers     = m_drivers;
    while ( *drivers != nullptr )
    {
        // Find the 'starting' page/driver
        size_t driverTotalSize = (*drivers)->getTotalSize();
        endOffset             += (*drivers)->getTotalSize();;
        if ( srcOffset < endOffset )
        {
            // Normalize the srcOffset and read the data
            srcOffset -= beginOffset;

            // Loop through as many drivers as needed to complete the read
            uint8_t* dstPtr  = (uint8_t*) dstData;
            while ( numBytesToRead && *drivers != nullptr )
            {
                bool result = driverRead( drivers, srcOffset, dstPtr, numBytesToRead );
                if ( !result )
                {
                    return false;
                }

                // Advance to the next driver
                drivers++;
                srcOffset = 0;
            }

            // Note: numBytesToRead should always be zero.  But if some how we
            // ran out of drivers before the read completed ->fail the 
            // transaction
            return numBytesToRead == 0;
        }

        // Next driver
        beginOffset += driverTotalSize;
        drivers++;
    }

    // If get here - no bytes were read
    return false;
}

bool Gang::driverRead( Api**     drivers,
                       size_t    normalizedSrcOffset,
                       uint8_t*& dstData,
                       size_t&   numBytesToRead ) noexcept
{
    // This should never happen -->but abort the write if there are no more drivers in the list
    if ( *drivers == nullptr )
    {
        return false;
    }

    // Starting reading the data from the current driver
    size_t bytesThisDriver = numBytesToRead;
    if ( normalizedSrcOffset + numBytesToRead > (*drivers)->getTotalSize() )
    {
        bytesThisDriver = (*drivers)->getTotalSize() - normalizedSrcOffset;
        numBytesToRead  = numBytesToRead - bytesThisDriver;
    }
    else
    {
        numBytesToRead = 0;
    }
    bool result = (*drivers)->read( normalizedSrcOffset, dstData, bytesThisDriver );
    dstData    += bytesThisDriver;
    return result;
}


//////////////////////////////////////////////////////////////////////////////
size_t Gang::getNumPages() const noexcept
{
    return m_numPages;
}

size_t Gang::getPageSize() const noexcept
{
    return m_pageSize;
}

size_t Gang::getTotalSize() const noexcept
{
    return m_totalSize;
}

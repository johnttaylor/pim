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

#include "Api.h"
#include "Cpl/Io/File/InputOutput.h"
#include "Cpl/Io/File/Api.h"
#include <new>

using namespace Driver::NV::File::Cpl;
using namespace ::Cpl;

//////////////////////////////////////////////////////////////////////////////
Api::Api( size_t      numPages,
          size_t      bytesPerPage,
          const char* filename )
    : m_fname( filename )
    , m_numPages( numPages )
    , m_pageSize( bytesPerPage )
    , m_totalSize( numPages * bytesPerPage )
    , m_started( false )
{
}

bool Api::start() noexcept
{
    // Skip processing if already started
    if ( !m_started )
    {
        // Check there is an existing directory with the same file name
        if ( Io::File::Api::isDirectory( m_fname ) )
        {
            return false;
        }

        // If the file does not exist - create it and fill it with the 'erased value'
        if ( !Io::File::Api::exists( m_fname ) )
        {
            // Create the file
            Io::File::InputOutput fd( m_fname, true, true, false );
            if ( !fd.isOpened() )
            {
                return false;
            }

            // Set the file/storage to all 'Erased Value'
            uint8_t pageBuffer[OPTION_DRIVER_NV_FILE_CPL_BYTES_PER_PAGE];
            memset( pageBuffer, OPTION_DRIVER_NV_FILE_CPL_ERASED_VALUE, sizeof( pageBuffer ) );
            size_t bytesRemaining = m_totalSize;
            while( bytesRemaining )
            {
                size_t bytesToWrite = bytesRemaining > sizeof( pageBuffer ) ? sizeof( pageBuffer ) : bytesRemaining;
                if ( !fd.write( pageBuffer, bytesToWrite ) )
                {
                    return false;
                }
                bytesRemaining -= bytesToWrite;
            }
        }

        m_started = true;
        return true;
    }

    return false;
}

void Api::stop() noexcept
{
    m_started = false;
}

//////////////////////////////////////////////////////////////////////////////
bool Api::write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept
{
    // Fail if not started
    if ( !m_started )
    {
        return false;
    }

    // Fail immediately if out-of-range
    if ( dstOffset + numBytesToWrite > m_totalSize )
    {
        return false;
    }


    // Open the file
    Io::File::InputOutput fd( m_fname, false, false, false );
    bool                  result = false;

    // Set the file position/offset
    if ( fd.setAbsolutePos( dstOffset ) )
    {
        // Read the data
        result = fd.write( srcData, numBytesToWrite );
    }

    return result;
}

bool Api::read( size_t srcOffset, void* dstData, size_t numBytesToRead ) noexcept
{
    // Fail if not started
    if ( !m_started )
    {
        return false;
    }

    // Fail immediately if out-of-range
    if ( srcOffset + numBytesToRead > m_totalSize )
    {
        return false;
    }

    // Open the file
    Io::File::InputOutput fd( m_fname, false, false, true );
    bool                  result = false;

    // Set the file position/offset
    if ( fd.setAbsolutePos( srcOffset ) )
    {
        // Read the data
        uint8_t* dstPtr = (uint8_t*) dstData;
        while ( numBytesToRead )
        {
            int bytesRead;
            if ( !fd.read( dstPtr, numBytesToRead, bytesRead ) )
            {
                return false;
            }

            numBytesToRead -= bytesRead;
            dstPtr         += bytesRead;
        }

        // If I get here I have read all of the requested data
        result = true;
    }

    return result;
}



size_t Api::getNumPages() const noexcept
{
    return m_numPages;
}

size_t Api::getPageSize() const noexcept
{
    return m_pageSize;
}

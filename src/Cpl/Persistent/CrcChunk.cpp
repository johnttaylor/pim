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

#include "CrcChunk.h"
#include "Private_.h"
#include "Cpl/Checksum/Crc32EthernetFast.h"
#include "Cpl/System/Assert.h"
#include <memory.h>

#define SECT_ "Cpl::Persistent"

#define FRAME_OFFSET_DATA_LEN   0
#define FRAME_OFFSET_DATA       (sizeof(size_t))

#define CRC_SIZE                (sizeof(uint32_t))

#define FRAME_OVERHEAD          (FRAME_OFFSET_DATA+CRC_SIZE)

///
using namespace Cpl::Persistent;

/////////////////////
CrcChunk::CrcChunk( RegionMedia& m_region )
    : m_region( m_region )
{
}

void CrcChunk::start( Cpl::Dm::MailboxServer& myMbox ) noexcept
{
    m_region.start( myMbox );
    m_dataLen = m_region.getRegionLength() - FRAME_OVERHEAD;
}

void CrcChunk::stop() noexcept
{
    m_region.stop();
}

bool CrcChunk::pushToRecord( Payload& dstHandler )
{
    return dstHandler.putData( g_workBuffer_, m_dataLen );
}
size_t CrcChunk::pullFromRecord( Payload& srcHandler )
{
    return srcHandler.getData( g_workBuffer_, m_dataLen );
}

void CrcChunk::reset()
{
    // Nothing required at this time (i.e. hook for child classes)
}

size_t CrcChunk::getMetadataLength() const noexcept
{
    return FRAME_OVERHEAD;
}

/////////////////////
bool CrcChunk::loadData( Payload& dstHandler, size_t index ) noexcept
{
    Cpl::Checksum::Crc32EthernetFast crc;
    crc.reset();
    size_t offset  = index;

    // Read the data length
    size_t datalen;
    if ( m_region.read( offset, &datalen, sizeof( datalen ) ) == sizeof( datalen ) )
    {
        // Make sure we have enough buffer space (Note: Check for rolling over the size_t bit space)
        size_t dataRemaining = datalen + CRC_SIZE;
        if ( dataRemaining <= sizeof( g_workBuffer_ ) && datalen < dataRemaining )
        {
            crc.accumulate( &datalen, sizeof( datalen ) );
            offset += sizeof( datalen );

            // Read the data AND CRC bytes
            uint8_t* dstPtr      = g_workBuffer_;
            while ( dataRemaining )
            {
                size_t bytesRead = m_region.read( offset, dstPtr, dataRemaining );
                if ( bytesRead == 0 )
                {
                    break;
                }
                crc.accumulate( g_workBuffer_, bytesRead );
                offset        += bytesRead;
                dataRemaining -= bytesRead;
            }

            // Check the CRC
            if ( crc.isOkay() )
            {
                // Remember/set the datalen
                m_dataLen = datalen;

                // Pass the data to the client
                return pushToRecord( dstHandler );
            }
        }
    }

    // If I get here that was an error OR a BAD CRC
    return false;
}

bool CrcChunk::updateData( Payload& srcHandler, size_t index, bool invalidate ) noexcept
{
    // Get the Payload data
    memset( g_workBuffer_, 0, sizeof( g_workBuffer_ ) );     // zero out all of the data - to ensure known values for the 'extra-space' (if there is any)
    size_t len = pullFromRecord( srcHandler );
    if ( len == 0 )
    {
        return false;
    }

    // Zero the data when erasing the record
    if ( invalidate )
    {
        memset( g_workBuffer_, 0, sizeof( g_workBuffer_ ) );     
    }

    // Set my record length based on the size of the application data
    m_dataLen = len;

    // Housekeeping
    Cpl::Checksum::Crc32EthernetFast crc;
    size_t                           offset  = index;
    bool                             result = true;
    crc.reset();

    // Data Length
    result &= m_region.write( offset, &m_dataLen, sizeof( m_dataLen ) );
    crc.accumulate( &m_dataLen, sizeof( m_dataLen ) );
    offset += sizeof( m_dataLen );

    // Payload
    result &= m_region.write( offset, g_workBuffer_, m_dataLen );
    crc.accumulate( g_workBuffer_, m_dataLen );
    offset += m_dataLen;

    // CRC
    uint8_t crcBuffer[CRC_SIZE];
    crc.finalize( crcBuffer );
    if ( invalidate )
    {
        // Corrupt the CRC when erasing the data
        crcBuffer[0] ^= 0xA5;
    }
    result &= m_region.write( offset, crcBuffer, CRC_SIZE );

    return result;
}

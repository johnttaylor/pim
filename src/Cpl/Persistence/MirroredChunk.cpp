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

#include "MirroredChunk.h"
#include "Private_.h"
#include "Cpl/Checksum/Crc32EthernetFast.h"
#include "Cpl/System/Assert.h"
#include <memory.h>

#define SECT_ "Cpl::Persistence"

#define FRAME_OFFSET_TRANS_ID   0
#define FRAME_OFFSET_DATA_LEN   (sizeof(uint64_t))
#define FRAME_OFFSET_DATA       (FRAME_OFFSET_DATA_LEN+sizeof(size_t))

#define CRC_SIZE                (sizeof(uint32_t))

///
using namespace Cpl::Persistence;

/////////////////////
MirroredChunk::MirroredChunk( RegionMedia& regionA, RegionMedia& regionB )
    : m_regionA( regionA )
    , m_regionB( regionB )
{
}

void MirroredChunk::start( Cpl::Dm::MailboxServer& myMbox  ) noexcept
{
    CPL_SYSTEM_ASSERT( m_regionA.getRegionLength() == m_regionB.getRegionLength() );

    m_regionA.start( myMbox );
    m_regionB.start( myMbox );
    m_transId       = 0;
    m_currentRegion = 0;
    m_dataLen       = 0;
}

void MirroredChunk::stop() noexcept
{
    m_regionA.stop();
    m_regionB.stop();
}


/////////////////////
bool MirroredChunk::loadData( Payload& dstHandler ) noexcept
{
    // Determine which Region (if any is the latest copy)
    bool     result = false;
    size_t   dataLenA;
    size_t   dataLenB;
    uint64_t transA = getTransactionId( m_regionA, dataLenA );
    uint64_t transB = getTransactionId( m_regionB, dataLenB );

    // No valid data!
    if ( transA == 0 && transB == 0 )
    {
        m_currentRegion = &m_regionA;
        m_transId       = 0;
        m_dataLen       = m_currentRegion->getRegionLength();
    }

    // Region A is newest/valid 
    else if ( transA > transB )
    {
        CPL_SYSTEM_ASSERT( dataLenA <= sizeof( g_workBuffer_ ) );
        size_t bytesRead = m_regionA.read( FRAME_OFFSET_DATA, g_workBuffer_, sizeof( g_workBuffer_ ) );
        result           = dstHandler.putData( g_workBuffer_, bytesRead );
        m_currentRegion  = &m_regionA;
        m_transId        = transA;
        m_dataLen        = dataLenA;
    }

    // Region B is newest/valid 
    else
    {
        CPL_SYSTEM_ASSERT( dataLenB <= sizeof( g_workBuffer_ ) );
        size_t bytesRead = m_regionB.read( FRAME_OFFSET_DATA, g_workBuffer_, sizeof( g_workBuffer_ ) );
        result           = dstHandler.putData( g_workBuffer_, bytesRead );
        m_currentRegion  = &m_regionB;
        m_transId        = transB;
        m_dataLen        = dataLenB;
    }

    CPL_SYSTEM_ASSERT( m_dataLen > m_currentRegion->getRegionLength() );
    return result;
}

bool MirroredChunk::updateData( Payload& srcHandler ) noexcept
{
    CPL_SYSTEM_ASSERT( m_currentRegion );

    // Get the Payload data
    memset( g_workBuffer_, 0, sizeof( g_workBuffer_ ) );     // zero out all of the data - to ensure known values for the 'extra-space' (if there is any)
    if ( srcHandler.getData( g_workBuffer_, m_dataLen ) == 0 )
    {
        return false;
    }

    bool result = true;
    // Write the data twice!
    for ( int i=0; i < 2; i++ )
    {
        // Select the region to update
        m_currentRegion = m_currentRegion == &m_regionA ? &m_regionB : &m_regionA;

        // Update the region
        Cpl::Checksum::Crc32EthernetFast crc;
        size_t                           offset  = 0;
        crc.reset();
        m_transId++;

        // Transaction ID
        result = m_currentRegion->write( offset, &m_transId, sizeof( m_transId ) );
        crc.accumulate( &m_transId, sizeof( m_transId ) );
        offset += sizeof( m_transId );

        // Data Length
        result &= m_currentRegion->write( offset, &m_dataLen, sizeof( m_dataLen ) );
        crc.accumulate( &m_dataLen, sizeof( m_dataLen ) );
        offset += sizeof( m_dataLen );

        // Payload
        result &= m_currentRegion->write( offset, g_workBuffer_, m_dataLen );
        crc.accumulate( g_workBuffer_, m_dataLen );
        offset += m_dataLen;

        // CRC
        uint8_t crcBuffer[CRC_SIZE];
        crc.finalize( crcBuffer );
        result &= m_currentRegion->write( offset, crcBuffer, CRC_SIZE );
    }

    return result;
}


uint64_t MirroredChunk::getTransactionId( RegionMedia& region, size_t& dataLen )
{
    Cpl::Checksum::Crc32EthernetFast crc;
    crc.reset();
    uint64_t result  = 0;
    uint64_t transId = 0;
    size_t   offset  = 0;

    // Read the Transaction ID
    if ( region.read( offset, &transId, sizeof( transId ) ) == sizeof( transId ) )
    {
        crc.accumulate( &transId, sizeof( transId ) );
        offset += sizeof( transId );

        // Read the data length
        if ( region.read( offset, &dataLen, sizeof( dataLen ) ) == sizeof( dataLen ) )
        {
            crc.accumulate( &dataLen, sizeof( dataLen ) );
            offset += sizeof( dataLen );

            // Read data AND the CRC bytes
            size_t dataRemaining = dataLen + CRC_SIZE;
            while ( dataRemaining )
            {
                size_t bytesRead = region.read( offset, g_workBuffer_, sizeof( g_workBuffer_ ) );
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
                result = transId;
            }
        }
    }

    // Return the result.  A value of ZERO indicates bad CRC
    return result;
}
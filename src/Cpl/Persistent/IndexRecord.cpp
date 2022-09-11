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

#include "IndexRecord.h"
#include "Cpl/System/Assert.h"
#include <memory.h>

#define SECT_ "Cpl::Persistent"


///
using namespace Cpl::Persistent;

//////////////////////////////////////////////////////
IndexRecord::IndexRecord( Cpl::Persistent::Chunk& chunkHandler ) noexcept
    : DataRecord( chunkHandler )
{
}


void IndexRecord::start( Cpl::Dm::MailboxServer& myMbox ) noexcept
{
    // Call my parent
    DataRecord::start( myMbox );
    m_validLatest = false;
}

void IndexRecord::stop() noexcept
{
    // Call my parent
    DataRecord::stop();
}

bool IndexRecord::getLatestOffset( size_t& offset, uint64_t& indexValue ) const noexcept
{
    if ( !m_validLatest )
    {
        return false;
    }
    offset     = m_latest;
    indexValue = m_latestIndex;
    return true;
}

void IndexRecord::setLatestOffset( size_t offset, uint64_t indexValue ) noexcept
{
    m_validLatest = true;
    m_latest      = offset;
    m_latestIndex = indexValue;
}

bool IndexRecord::processNoValidData() noexcept
{
    m_latest      = 0;
    m_latestIndex = 0;
    m_validLatest = true;
    return true;
}

size_t IndexRecord::getPayloadSize() const noexcept
{
    return sizeof( m_latest ) + sizeof( m_latestIndex );
}

size_t IndexRecord::getData( void* dst, size_t maxDstLen ) noexcept
{
    // Do NOT return 'invalid' data OR if the dst length is too small
    if ( !m_validLatest || maxDstLen < getPayloadSize() )
    {
        return 0;
    }

    uint8_t* ptr = (uint8_t*) dst;
    memcpy( ptr, &m_latest, sizeof( m_latest ) );
    ptr += sizeof( m_latest );
    memcpy( ptr, &m_latestIndex, sizeof( m_latestIndex ) );
    
    return getPayloadSize();
}

bool IndexRecord::putData( const void* src, size_t srcLen ) noexcept
{
    if ( srcLen >= getPayloadSize() )
    {
        const uint8_t* ptr = (const uint8_t*) src;
        memcpy( &m_latest, ptr, sizeof( m_latest ) );
        ptr += sizeof( m_latest );
        memcpy( &m_latestIndex, ptr, sizeof( m_latestIndex ) );

        m_validLatest = true;
        return true;
    }

    return false;
}
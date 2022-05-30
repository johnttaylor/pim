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

#include "DataRecord.h"
#include "Cpl/System/Assert.h"

#define SECT_ "Cpl::Persistent"


///
using namespace Cpl::Persistent;

//////////////////////////////////////////////////////
DataRecord::DataRecord( Cpl::Persistent::Chunk& chunkHandler ) noexcept
    : m_chunkHandler( chunkHandler )
    , m_started( false )
{
}

DataRecord::~DataRecord()
{
    // Make sure the Chunk handler gets/is stopped 
    stop();
}

void DataRecord::start( Cpl::Dm::MailboxServer& myMbox ) noexcept
{
    if ( !m_started )
    {
        m_started = true;

        // Start the chunk handler
        m_chunkHandler.start( myMbox );
    }
}

void DataRecord::stop() noexcept
{
    if ( m_started )
    {
        m_started = false;

        // Stop the Chunk Handler
        m_chunkHandler.stop();
    }
}

bool DataRecord::writeToMedia( size_t index) noexcept
{
    return m_chunkHandler.updateData( *this, index );
}

bool DataRecord::readFromMedia(size_t index) noexcept
{
    if ( !m_chunkHandler.loadData( *this, index ) )
    {
        // No valid data -->let the application decide what to do
        return processNoValidData();
    }

    // If I get here, everything worked!
    return true;
}



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

#include "NVAdapter.h"
#include "Cpl/System/Assert.h"


#define SECT_ "Cpl::Persistent"


///
using namespace Cpl::Persistent;

/////////////////////
NVAdapter::NVAdapter( Driver::NV::Api& lowLevelDriver, size_t regionStartAddress, size_t regionLen ) noexcept
    : RegionMedia( regionStartAddress, regionLen )
    , m_driver( lowLevelDriver )
{
}

void NVAdapter::start( Cpl::Dm::MailboxServer& myMbox ) noexcept
{
    m_driver.start();
}

void NVAdapter::stop() noexcept
{
    m_driver.stop();
}


/////////////////////
bool NVAdapter::write( size_t offset, const void* srcData, size_t srcLen ) noexcept
{
    CPL_SYSTEM_ASSERT( srcData );
    CPL_SYSTEM_ASSERT( srcLen > 0 );

    // Check to make sure we don't overflow the region
    if ( (offset + srcLen) > m_regionLength )
    {
        return false;
    }
    return m_driver.write( offset + m_startAddress, srcData, srcLen );
}

size_t NVAdapter::read( size_t offset, void* dstBuffer, size_t bytesToRead ) noexcept
{
    CPL_SYSTEM_ASSERT( dstBuffer );
    CPL_SYSTEM_ASSERT( bytesToRead > 0 );

    // Check to make sure we don't overflow the region
    if ( (offset + bytesToRead) > m_regionLength )
    {
        return 0;
    }
    return m_driver.read( offset + m_startAddress, dstBuffer, bytesToRead ) ? bytesToRead : 0;
}

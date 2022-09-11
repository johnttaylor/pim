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

#include "FileAdapter.h"
#include "Cpl/Io/File/Input.h"
#include "Cpl/Io/File/Output.h"
#include "Cpl/System/Assert.h"


#define SECT_ "Cpl::Persistent"


///
using namespace Cpl::Persistent;

/////////////////////
FileAdapter::FileAdapter( const char* fileName, size_t regionStartAddress, size_t regionLen ) noexcept
    : RegionMedia( regionStartAddress, regionLen )
    , m_fileName( fileName )
{
}

void FileAdapter::start( Cpl::Dm::MailboxServer& myMbox ) noexcept
{
    // Nothing needed
}

void FileAdapter::stop() noexcept
{
    // Nothing needed
}


/////////////////////
bool FileAdapter::write( size_t offset, const void* srcData, size_t srcLen ) noexcept
{
    CPL_SYSTEM_ASSERT( srcData );
    CPL_SYSTEM_ASSERT( srcLen > 0 );

    Cpl::Io::File::Output fd( m_fileName );
    if ( !fd.isOpened() )
    {
        return false;
    }

    bool result = fd.setAbsolutePos( offset );
    if ( result )
    {
        result = fd.write( srcData, srcLen );
    }
    fd.close();
    return result;
}

size_t FileAdapter::read( size_t offset, void* dstBuffer, size_t bytesToRead ) noexcept
{
    CPL_SYSTEM_ASSERT( dstBuffer );
    CPL_SYSTEM_ASSERT( bytesToRead > 0 );
    int actualBytesRead = 0;

    Cpl::Io::File::Input fd( m_fileName );
    if ( !fd.isOpened() )
    {
        return 0;
    }

    bool result = fd.setAbsolutePos( offset );
    if ( result )
    {
        result = fd.read( dstBuffer, bytesToRead, actualBytesRead );
    }
    fd.close();

    return result? actualBytesRead : 0;
}

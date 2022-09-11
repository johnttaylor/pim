#ifndef Cpl_Persistent_File_Adapter_h_
#define Cpl_Persistent_File_Adapter_h_
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

#include "Cpl/Persistent/RegionMedia.h"


///
namespace Cpl {
///
namespace Persistent {

/** This concrete class implements the RegionMedia interface using the 
    Cpl::Io::File interfaces. Each instance of this class uses a single file 
    as the storage media.  It is the responsibility of the application to ensure
    that each instance has a unique file name
 */
class FileAdapter : public RegionMedia
{
public:
    /** Constructor.  
     */
    FileAdapter( const char* fileName, size_t regionStartAddress, size_t regionLen ) noexcept;

public:
    /// See Cpl::Persistent::RegionMedia
    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept;

    /// See Cpl::Persistent::RegionMedia
    void stop() noexcept;

    /// See Cpl::Persistent::RegionMedia
    bool write( size_t offset, const void* srcData, size_t srcLen ) noexcept;

    /// See Cpl::Persistent::RegionMedia
    size_t read( size_t offset, void* dstBuffer, size_t bytesToRead ) noexcept;


protected:
    /// Remember my file name
    const char* m_fileName;
};

};      // end namespaces
};
#endif  // end header latch
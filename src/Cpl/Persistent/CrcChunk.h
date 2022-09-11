#ifndef Cpl_Persistent_Crc_Chunk_h_
#define Cpl_Persistent_Crc_Chunk_h_
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


#include "Cpl/Persistent/Chunk.h"
#include "Cpl/Persistent/RegionMedia.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Persistent {


/** This concrete class implements the Chunk interface by using a 32Bit CRC
    for ensuring data integrity.
 */
class CrcChunk: public Chunk
{
public:
    /// Constructor
    CrcChunk( RegionMedia& region );

public:
    /// See Cpl::Persistent::Chunk
    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept;

    /// See Cpl::Persistent::Chunk
    void stop() noexcept;

    /// See Cpl::Persistent::Chunk
    bool loadData( Payload& dstHandler, size_t index=0  ) noexcept;

    /// See Cpl::Persistent::Chunk
    bool updateData( Payload& srcHandler, size_t index=0, bool invalidate=false  ) noexcept;

    
    /// See Cpl::Persistent::Chunk
    size_t getMetadataLength() const noexcept;

protected:
    /// Helper method. Encapsulates pushing data to the record
    virtual bool pushToRecord( Payload& dstHandler );

    /// Helper method. Encapsulates retrieving data from the record.  Returns the length of the data
    virtual size_t pullFromRecord( Payload& srcHandler );

    /// Helper method. Encapsulates actions that occur when there is NO VALID data
    virtual void reset();


protected:
    /// Region/Media 
    RegionMedia& m_region;

    /// Data Length for the record
    size_t       m_dataLen;
};


};      // end namespaces
};
#endif  // end header latch

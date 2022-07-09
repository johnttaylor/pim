#ifndef Cpl_Persistent_Region_Media_h_
#define Cpl_Persistent_Region_Media_h_
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
/** @file */

#include "Cpl/Dm/MailboxServer.h"
#include <stdlib.h>


///
namespace Cpl {
///
namespace Persistent {


/** This mostly abstract class defines the operations that can be performed on
    a persistent media.  In addition there is concept of a persistent media
    being dividing into non-overlapping regions.  Each Region has a starting
    offset, and length (in bytes) of the region.  All read/write operations
    are relative the start of the region, i.e. offset zero is first byte of 
    the region; offset 9 is the tenth byte in the region.
 */
class RegionMedia
{
public:
    /** This method is to start/initialize the Region.  It is typically only
        called once at the startup of the application.  However, start() can be
        called after a previous call to the stop() method.
        
        This method is called when the corresponding Chunk instance is 'started'

        The 'myMbox' is a reference to the RecordServer's ITC mailbox, i.e. the
        mailbox for the thread that the Record executes in. The 'myEventLoop'
        is a reference to the RecordServer's Event Loop.
     */
    virtual void start( Cpl::Dm::MailboxServer& myMbox ) noexcept = 0;

    /** This method is to stop/shutdown the Region.  It is typically only
        called once during an orderly shutdown of the application. However,
        start() can be after a previous call to the stop() method.
     */
    virtual void stop() noexcept = 0;


public:
    /** This method writes 'srcLen' bytes to the media at the specified offset.  
        The offset is relative to the start of the Region's start address.

        The method return true of the write operation was successful, else false
        is returned.
     */
    virtual bool write( size_t offset, const void* srcData, size_t srcLen ) noexcept = 0;

    /** This method reads 'bytesToRead' bytes from the media at the specified 
        offset.  The offset is relative to the start of the Region's start 
        address.

        The method returns the number of bytes read.  A return value of ZERO 
        indicates that an error occurred.
     */
    virtual size_t read( size_t offset, void* dstBuffer, size_t bytesToRead ) noexcept = 0;


public:
    /// Returns the Region's starting address
    virtual size_t getStartAddress() const { return m_startAddress; };

    /// Returns the Region's length in bytes
    virtual size_t getRegionLength() const { return m_regionLength; };


public:
    /// Virtual destructor
    virtual ~RegionMedia() {}


protected:
    /// Constructor.  Note: A child class is required
    RegionMedia( size_t startAddress, size_t regionLength ) :m_startAddress( startAddress ), m_regionLength( regionLength ) {}

protected:
    /// The Regions' starting address
    size_t m_startAddress;

    /// The length, in bytes, of the region
    size_t m_regionLength;
};


};      // end namespaces
};
#endif  // end header latch

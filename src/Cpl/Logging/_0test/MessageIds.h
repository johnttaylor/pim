#ifndef Cpl_Persistent_Chunk_h_
#define Cpl_Persistent_Chunk_h_
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


#include "Cpl/Persistent/Payload.h"
#include "Cpl/Dm/MailboxServer.h"

///
namespace Cpl {
///
namespace Persistent {


/** This abstract class defines the interface for a Chunk. A Chunk is responsible
    for managing the meta-data (e.g. CRC) associated with a Record's data.  There
    is one-to-one relationship between a Chunk and Record instances.
 */
class Chunk
{
public:
    /** This method is to start/initialize the chunk.  It is typically only
        called once at the startup of the application.  However, start() can be
        called after a previous call to the stop() method.
 
        This method is called when the corresponding Record instance is 'started'

        The 'myMbox' is reference to the RecordServer's DM/ITC mailbox, i.e. the
        mailbox for the thread that the Chunk executes in.
     */
    virtual void start( Cpl::Dm::MailboxServer& myMbox ) noexcept = 0;

    /** This method is to stop/shutdown the chunk.  It is typically only
        called once during an orderly shutdown of the application. However,
        start() can be after a previous call to the stop() method.
     */
    virtual void stop() noexcept = 0;


public:
    /** This method is used to initiate the sequence to retrieve a Record's
        data from persistent storage.  This method should ONLY be called once
        (after start() has been called) to load its data into RAM.

        The 'index' offset can be used by the Chunk client to 'index into' the
        Chunk's region instead of starting at offset zero.  This argument should
        only be used when multiple instances of a Record is being stored in
        a single region.

        The method returns true if the read operation was successful and that
        the CRC for the data is valid; else false is returned
     */
    virtual bool loadData( Payload& dstHandler, size_t index=0 ) noexcept = 0;

    /** This method is used to update persistent storage with new data for the
        Record.  The entire record is written/updated when this call is made.
        The method does not return until the write operation has completed.
    
        If the 'invalidate' argument is true, the instead of writing the
        new data to persistent storage, all binary zero's are written AND in
        incorrect CRC is written.  This effectively erases the Record, i.e. the 
        next time the record is loaded, it will fail because the chunk data is 
        NOT valid.

        The 'index' offset can be used by the Chunk client to 'index into' the
        Chunk's region instead of starting at offset zero.  This argument should
        only be used when multiple instances of a Record is being stored in
        a single region.

        The method returns true if successful; else false is returned.  It is
        the responsibility of the Record/Application to decided what to do when 
        there is error (e.g. ignored, a log entry generated, etc.)
    */
    virtual bool updateData( Payload& srcHandler, size_t index=0, bool invalidate=false  ) noexcept = 0;


public:
    /** This method returns the size, in bytes, of any/all metadata that is
        included with the record.
     */
    virtual size_t getMetadataLength() const noexcept = 0;

public:
    /// Virtual destructor
    virtual ~Chunk() {}
};


};      // end namespaces
};
#endif  // end header latch

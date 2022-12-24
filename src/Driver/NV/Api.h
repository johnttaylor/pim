#ifndef Driver_NV_Api_h_
#define Driver_NV_Api_h_
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


#include <stdlib.h>

///
namespace Driver {
///
namespace NV {


/** This class defines the interface for a platform independent Non-volatile
    storage driver. Only basic read/write operations are defined.  How the NV
    storage is used is left the application and/or upper layers 
    
    The Interface assumes a paradigm of the NV storage is broken up into N
    pages where each page contains M bytes.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class Api 
{
public:
    /** This method is used initialize/start the driver.  To 'restart' the driver,
        the application must call stop(), then start().  

        The method returns true if successful; else false is returned when an
        error occurred.  If false is returned, future read/write calls will always
        return false.
     */
    virtual bool start() noexcept = 0;

    /// This method is used to stop/shutdown the driver.
    virtual void stop() noexcept = 0;

public:
    /** This method writes N bytes from 'srcData' starting at the storage offset
        of 'dstOffset'.  The application is responsible for ensuring that the 
        size 'srcData' is at least 'numBytesToWrite' in length.
        
        This write() method itself is synchronous, but the physically writing of 
        the data to the NV storage is not guaranteed to synchronous, i.e. the 
        physical write operation can completed AFTER the write() method returns. 
        If a second write() call is made and the physical storage is still being
        updated from the first write() call, the second call will wait until 
        the first physical write operation completes.  There is no defined 
        semantics for the waiting (i.e. it could be a blocking wait or a busy 
        wait)

        The method returns true if operation was successful; else false is 
        returned.
     */
    virtual bool write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept = 0;

    /** This method reads N bytes starting at the storage offset of 'srcOffset' 
        and stores the bytes into 'dstData'. The application is responsible for 
        ensuring that the size 'dstData' is at least 'numBytesToRead' in length.
        
        The method returns true if operation was successful; else false is 
        returned.
     */
    virtual bool read( size_t srcOffset, void* dstData, size_t numBytesToRead ) noexcept = 0;


public:
    /** This method returns the total number of NV storage pages.  A Page is 
        defined as the boundary/maximum amount of data that can be written in 
        a single physical update of the NV storage

        NOTE: ALWAYS use the getTotalSize() method for querying/determining the 
              total available storage size.
     */
    virtual size_t getNumPages() const noexcept = 0;

    /** This method returns the NV storage page size.  See getNumPages() for 
        more details about the definition of a page.  

        NOTE: ALWAYS use the getTotalSize() method for querying/determining the
              total available storage size.
     */
    virtual size_t getPageSize() const noexcept = 0;
    
    /** The method that returns the total storage size in bytes.

        Note: This is canonical source of truth for the total available storage,
              i.e, if the driver instance is a 'Gang' driver instance then
              the method CAN return a different result from: getNumPages() * getPageSize()
     */        
    virtual size_t getTotalSize() const noexcept { return getNumPages() * getPageSize(); }

public:
    /// Virtual destructor
    virtual ~Api() {}
};




};      // end namespaces
};
#endif  // end header latch

#ifndef Driver_NV_Gang_h_
#define Driver_NV_Gang_h_
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


#include "Driver/NV/Api.h"
#include <stdint.h>




///
namespace Driver {
///
namespace NV {


/** This class implements the Non-volatile storage driver using a collection
    of NV drivers to provide a 'flat address' space that spans multiple 
    discrete storage chips/media.  
    
    The individual drivers/nv-storage-instance can be different sizes and
    different physical media.
    
    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class Gang : public Driver::NV::Api
{
public:
    
public: 
    /** Constructor. The 'drivers' argument is a variable length array of
        pointers to a concrete NV drivers.  The last entry in the array must
        be a nullptr (i.e. end-of-list marker)
     */
    Gang( Api* drivers[] );

public:
    /// See Driver::NV::Api
    bool start() noexcept;

    /// See Driver::NV::Api
    void stop() noexcept;

    /// See Driver::NV::Api
    bool write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept;
    
    /// See Driver::NV::Api
    bool read( size_t srcOffset, void* dstData, size_t numBytesToRead ) noexcept;

    /** See Driver::NV::Api.  The number of pages is based on the smallest
        page size from the various ganged drivers that fits within the total
        available space. When the ganged drivers have pages size that are not
        multiples of each other, the returned value can be just approximate 
        number of pages.
     */
    size_t getNumPages() const noexcept;

    /** See Driver::NV::Api. The reported page size is the smallest page size
        from the various ganged drivers.
     */
    size_t getPageSize() const noexcept;
    
    /** See Driver::NV::Api.  Reports the total available storage size. This 
        method accurately returns the total amount of storage.
     */
    size_t getTotalSize() const noexcept;

protected:
    /// Helper method to read data 
    bool driverRead( Api**     drivers, 
                     size_t    normalizedSrcOffset, 
                     uint8_t*& dstData,
                     size_t&   numBytesToRead ) noexcept;

    /// Helper method to write data 
    bool driverWrite( Api**           drivers, 
                      size_t          normalizedDstOffset, 
                      const uint8_t*& srcData, 
                      size_t&         numBytesToWrite ) noexcept;


protected:
    /// List of drivers
    Api**       m_drivers;
    
    /// Number of pages
    size_t      m_numPages;

    /// Page size
    size_t      m_pageSize;

    /// Total size
    size_t      m_totalSize;

    /// Started state
    bool        m_started;
};




};      // end namespaces
};
#endif  // end header latch

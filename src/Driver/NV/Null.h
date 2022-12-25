#ifndef Driver_NV_Null_h_
#define Driver_NV_Null_h_
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


#include "colony_config.h"
#include "Driver/NV/Api.h"
#include <memory.h>


/** The page size, in bytes, of the EEPROM chips.
 */
#ifndef OPTION_DRIVER_NV_NULL_PAGE_SIZE    
#define OPTION_DRIVER_NV_NULL_PAGE_SIZE             128
#endif

 /** Total page count
  */
#ifndef OPTION_DRIVER_NV_NULL_TOTAL_PAGES
#define OPTION_DRIVER_NV_NULL_TOTAL_PAGES           (512)
#endif

  ///
namespace Driver {
///
namespace NV {


/** This concrete class provides a 'null' implementation of the NV Storage
    interface.
 */
class Null : public Api
{
public:
    /// Constructor
    Null( size_t numPages=OPTION_DRIVER_NV_NULL_PAGE_SIZE, size_t bytesPerPage=OPTION_DRIVER_NV_NULL_TOTAL_PAGES )
        : m_numPages( numPages )
        , m_bytesPerPage( bytesPerPage )
    {
    }

public:
    /// See Driver::NV::Api
    bool start() noexcept { return true; }

    /// See Driver::NV::Api
    void stop() noexcept {}


    /// See Driver::NV::Api. Always successful 
    bool write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept { return true; }

    /// See Driver::NV::Api. Always successful.  Sets the returned data to all zeros
    bool read( size_t srcOffset, void* dstData, size_t numBytesToRead ) noexcept { memset( dstData, 0, numBytesToRead );  return true; }

    /// See Driver::NV::Api. 
    size_t getNumPages() const noexcept { return m_numPages; }

    /// See Driver::NV::Api. 
    size_t getPageSize() const noexcept { return m_bytesPerPage; }

protected:
    /// Number of pages
    size_t m_numPages;

    /// Bytes per page
    size_t m_bytesPerPage;
};




};      // end namespaces
};
#endif  // end header latch

#ifndef Driver_NV_File_Cpl_Api_h_
#define Driver_NV_File_Cpl_Api_h_
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


/// Number of Pages
#ifndef OPTION_DRIVER_NV_FILE_CPL_NUM_PAGES
#define OPTION_DRIVER_NV_FILE_CPL_NUM_PAGES         512
#endif

/// Number of bytes per page
#ifndef OPTION_DRIVER_NV_FILE_CPL_BYTES_PER_PAGE
#define OPTION_DRIVER_NV_FILE_CPL_BYTES_PER_PAGE    128
#endif

/** The filename to use as the physical storage
 */
#ifndef OPTION_DRIVER_NV_FILE_CPL_FILE_NAME
#define OPTION_DRIVER_NV_FILE_CPL_FILE_NAME         "eeprom.bin"
#endif

/** The value used to erase the physical storage
 */
#ifndef OPTION_DRIVER_NV_FILE_CPL_ERASED_VALUE
#define OPTION_DRIVER_NV_FILE_CPL_ERASED_VALUE      0xFF
#endif

///
namespace Driver {
///
namespace NV {
///
namespace File {
///
namespace Cpl {


/** This class implements the Non-volatile storage driver using the Cpl::Io::File
    interfaces.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class Api : public Driver::NV::Api
{
public: 
    /// Constructor 
    Api( size_t      numPages     = OPTION_DRIVER_NV_FILE_CPL_NUM_PAGES,
         size_t      bytesPerPage = OPTION_DRIVER_NV_FILE_CPL_BYTES_PER_PAGE,
         const char* filename     = OPTION_DRIVER_NV_FILE_CPL_FILE_NAME );

public:
    /// See Driver::NV::Api
    bool start() noexcept;

    /// See Driver::NV::Api
    void stop() noexcept;

    /// See Driver::NV::Api
    bool write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept;
    
    /// See Driver::NV::Api
    bool read( size_t srcOffset, void* dstData, size_t numBytesToRead ) noexcept;

    /// See Driver::NV::Api
    size_t getNumPages() const noexcept;

    /// See Driver::NV::Api
    size_t getPageSize() const noexcept;
    
   
protected:
    /// File name
    const char* m_fname;
    
    /// number of pages
    size_t      m_numPages;

    /// bytes per page
    size_t      m_pageSize;

    /// Actual storage size
    size_t      m_totalSize;

    /// Started state
    bool        m_started;
};




};      // end namespaces
};
};
};
#endif  // end header latch

#ifndef Cpl_Io_File_Md5Checksum_h_
#define Cpl_Io_File_Md5Checksum_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2023  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "colony_config.h"
#include "Cpl/Checksum/ApiMd5.h"
#include "Cpl/Text/String.h"

/// Size, in bytes, of the work buffer used when reading the file contents
#ifndef OPTION_CPL_IO_FILE_MD5_WORK_BUFFER_SIZE
#define OPTION_CPL_IO_FILE_MD5_WORK_BUFFER_SIZE     1024
#endif

///
namespace Cpl {
///
namespace Io {
///
namespace File {



/** This method calculates the MD5 Checksum of the specified file.  The
    method returns true when successful.  The caller is required to provide
    memory for 'dstDigest'

    When 'dstString' is non null, the checksum as a ASCII HEX string is returned.

    When 'dstFileLen' is non null, the length (in bytes) of the file is returned.
 */
bool calcMD5Checksum( const char*                       fullPathName,
                      Cpl::Checksum::ApiMd5::Digest_T   dstDigest,
                      Cpl::Text::String*                dstString  = nullptr,
                      size_t*                           dstFileLen = nullptr );


};      // end namespaces
};
};
#endif  // end header latch

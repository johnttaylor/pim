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

#include "Md5Checksum.h"
#include "Cpl/Checksum/Md5Aladdin.h"
#include "Input.h"
#include <string.h>

//
using namespace Cpl::Io::File;


//////////////////////////
bool Cpl::Io::File::calcMD5Checksum( const char*                       fullPathName,
                                     Cpl::Checksum::ApiMd5::Digest_T   dstDigest,
                                     Cpl::Text::String*                dstString,
                                     size_t*                           dstFileLen )
{
    Input infd( fullPathName );
    if ( !infd.isOpened() )
    {
        return false;
    }

    // Read the entire file contents
    Cpl::Checksum::Md5Aladdin md5;
    uint8_t                   workBuffer[OPTION_CPL_IO_FILE_MD5_WORK_BUFFER_SIZE];
    size_t                    fileLen = 0;
    int                       bytesRead;
    for(;;)
    {
        // Read one 'chunk' at time
        if ( !infd.read( workBuffer, sizeof(workBuffer), bytesRead ) )
        {
            // Reached End-of-File
            if ( infd.isEof() )
            {
                // Success
                memcpy( dstDigest, &(md5.finalize( dstString, false )), sizeof( Cpl::Checksum::ApiMd5::Digest_T ) );
                if ( dstFileLen )
                {
                    *dstFileLen = fileLen;
                }
                return true;
            }

            // Error OTHER then EOF
            else
            {
                break;
            }
        }
        
        // Calculate the checksum
        else
        {
            md5.accumulate( workBuffer, bytesRead );
            fileLen += bytesRead;
        }
    }

    // If I get here, there was an error
    return false;
}

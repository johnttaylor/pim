#ifndef Cpl_Io_File_Common_x_h_
#define Cpl_Io_File_Common_x_h_
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

#include "Cpl/Io/Descriptor.h"

///
namespace Cpl {
///
namespace Io {
///
namespace File {

/** This private concrete class is a helper class that implements
    the common functions for the handling files.
 */
class Common_
{
public:
    /** Opens a file.  The default arguments are set for:

            o Read/Write access
            o Do NOT create the file if the file does not exist
            o Do NOT truncate the file contents
     */
    static Cpl::Io::Descriptor open( const char* fileEntryName, bool readOnly=true, bool forceCreate=false, bool forceEmptyFile=false );


public:
    /// See Cpl::Io::File::ObjectApi
    static bool length( Cpl::Io::Descriptor fd, unsigned long& length );


public:
    /// See Cpl::Io::File::ObjectApi
    static bool currentPos( Cpl::Io::Descriptor fd, unsigned long& currentPos  );

    /// See Cpl::Io::File::ObjectApi
    static bool setRelativePos( Cpl::Io::Descriptor fd, long deltaOffset );

    /// See Cpl::Io::File::ObjectApi
    static bool setAbsolutePos( Cpl::Io::Descriptor fd, unsigned long newoffset );

    /// See Cpl::Io::File::ObjectApi
    static bool setToEof( Cpl::Io::Descriptor fd );
};


};      // end namespaces
};
};
#endif  // end header latch

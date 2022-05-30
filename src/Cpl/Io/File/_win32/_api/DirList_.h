#ifndef Cpl_Io_File_DirList_x_h_
#define Cpl_Io_File_DirList_x_h_
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

#include "Cpl/Io/File/Api.h"
#include <windows.h>


///
namespace Cpl {
///
namespace Io {
///
namespace File {



/** This private concrete does the work for Api::walkDirectory() method
 */
class DirList_
{
protected:
    /// internal variable
    int                 m_depth;
    
    /// internal variable
    int                 m_curDepth;
    
    /// internal variable
    bool                m_filesOnly;
    
    /// internal variable
    bool                m_dirsOnly;
    
    /// internal variable
    Api::Info           m_info;
    
    /// internal variable
    WIN32_FIND_DATA     m_fdata;
    
    /// internal variable
    NameString          m_name;
    
    /// internal variable
    NameString          m_file;


public:
    /// Constructor
    DirList_( const char* rootDir, int depth=1, bool filesOnly=false, bool dirsOnly=false );


    /// Destructor
    ~DirList_();


public:
    /// See Api::walkDirectory
    bool traverse( Api::DirectoryWalker& callback );
};


};      // end namespaces
};
};
#endif  // end header latch

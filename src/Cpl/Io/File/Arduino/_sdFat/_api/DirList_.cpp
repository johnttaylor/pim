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

#include "DirList_.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Io/File/Arduino/_sdFat/Private_.h"
#include "FatLib/FatStructs.h"
#include <time.h>

#define MAX_ENTRY_NAME  (8+1+3+1)   // Short name support ONLY!

//
using namespace Cpl::Io::File;




///////////////
DirList_::DirList_( const char* rootDir, int depth, bool filesOnly, bool dirsOnly )
    :m_depth( depth ),
    m_curDepth( 0 ),
    m_filesOnly( filesOnly ),
    m_dirsOnly( dirsOnly ),
    m_name(  rootDir  )
{
}

DirList_::~DirList_()
{
}


///////////////
bool DirList_::traverse( Api::DirectoryWalker& callback )
{
    bool completed = true;
    int  added     = 0;

    // Ensure my current root ends with a trailing dir separator
    m_curDepth++;
    if ( m_name[m_name.length() - 1] != '/' )
    {
        m_name += '/';
        added++;
    }

    // Get the first item in current root
    FatFile currentDir( m_name(), O_RDONLY );
    if ( currentDir.isDir() )
    {
        currentDir.rewind();
        FatFile file;

        while ( file.openNext( &currentDir, O_RDONLY ) )
        {
            // Get info for the current item in the directory
            dir_t src;
            if ( file.dirEntry( &src ) )
            {
                Api::Info info;
                info.m_isDir     = DIR_IS_SUBDIR( &src );
                info.m_isFile    = DIR_IS_FILE( &src );
                info.m_size      = src.fileSize;
                info.m_readable  = true;
                info.m_writeable = ( src.attributes & DIR_ATT_READ_ONLY ) == 0;
                struct tm ftime  ={ 0, };
                ftime.tm_year    = FAT_YEAR( src.lastWriteDate ) + 10;   // The FAT Epoch starts at 1980
                ftime.tm_mon     = FAT_MONTH( src.lastWriteDate ) - 1;   // The FAT Month is 1-12
                ftime.tm_mday    = FAT_DAY( src.lastWriteDate );
                ftime.tm_hour    = FAT_HOUR( src.lastWriteTime );
                ftime.tm_min     = FAT_MINUTE( src.lastWriteTime );
                ftime.tm_sec     = FAT_SECOND( src.lastWriteTime );
                info.m_mtime     = mktime( &ftime );

                // Construct full path for the current item
                char entryName[MAX_ENTRY_NAME];
                if ( file.getName( entryName, MAX_ENTRY_NAME ) )
                {
                    // Callback the client
                    if ( ( !m_filesOnly && !m_dirsOnly ) || ( m_filesOnly && info.m_isFile ) || ( m_dirsOnly && info.m_isDir ) )
                    {
                        if ( callback.item( m_name, entryName, info ) == Cpl::Type::Traverser::eABORT )
                        {
                            completed = false;
                            file.close();
                            break;
                        }
                    }

                    // Do I need to dive into a sub directory?
                    if ( info.m_isDir && m_curDepth < m_depth )
                    {
                        int len = strlen( entryName );
                        m_name += entryName;

                        // Walk the sub directory
                        if ( !traverse( callback ) )
                        {
                            completed = false;
                            file.close();
                            break;
                        }
                        m_name.trimRight( len );
                    }
                }
            }
            file.close();
        }
    }

    // Housekeeping
    m_curDepth--;
    if ( completed )
    {
        m_name.trimRight( added );
    }

    return completed;
}

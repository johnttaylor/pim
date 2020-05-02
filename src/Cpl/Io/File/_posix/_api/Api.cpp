/*-----------------------------------------------------------------------------
* This file is part of the Shift-Right Open Repository.  The Repository is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the root directory or on the Internet at
* http://www.shift-right.com/openrepo/license.htm
*
* Copyright (c) 2001-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Cpl/Io/File/Api.h"
#include "DirList_.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


///
using namespace Cpl::Io::File;


/// Helper method that does all of the work for populating the Info struct
static void populate_( Api::Info& infoOut, struct stat& filestats )
{
    infoOut.m_isDir     = (filestats.st_mode&S_IFDIR) == S_IFDIR;
    infoOut.m_isFile    = (filestats.st_mode&S_IFREG) == S_IFREG;
    infoOut.m_size      = filestats.st_size;
    infoOut.m_mtime     = filestats.st_mtime;
    infoOut.m_readable  = (filestats.st_mode&S_IROTH) == S_IROTH;
    infoOut.m_writeable = (filestats.st_mode&S_IWOTH) == S_IWOTH;

    if ( getuid() == filestats.st_uid )
    {
        infoOut.m_readable  |= (filestats.st_mode&S_IRUSR) == S_IRUSR;
        infoOut.m_writeable |= (filestats.st_mode&S_IWUSR) == S_IWUSR;
    }
    if ( getgid() == filestats.st_gid )
    {
        infoOut.m_readable  |= (filestats.st_mode&S_IRGRP) == S_IRGRP;
        infoOut.m_writeable |= (filestats.st_mode&S_IWGRP) == S_IWGRP;
    }

    // Make 'size' of directory behave per the Cpl::Io::File::Api semantics
    if ( infoOut.m_isDir )
    {
        infoOut.m_size = 0;
    }
}



/////////////////////////////////////////////////////
bool Api::getInfo( const char* fsEntryName, Info& infoOut )
{
    struct stat filestats;
    if ( stat( getNative( fsEntryName ), &filestats ) == 0 )
    {
        populate_( infoOut, filestats );
        return true;
    }

    return false;
}



/////////////////////////////////////////////////////
bool Api::canonicalPath( const char* relPath, Cpl::Text::String& absPath )
{
    char  buffer[CPL_IO_FILE_MAX_NAME + 1];
    char* ptr = realpath( relPath, buffer );
    if ( ptr )
    {
        absPath = buffer;
        return !absPath.truncated();
    }

    return false;
}

bool Api::getCwd( Cpl::Text::String& cwd )
{
    int len;
    char* ptr    = cwd.getBuffer( len );
    char* result = getcwd( ptr, len );
    cwd          = getStandard( cwd );
    return result != NULL;
}


/////////////////////////////////////////////////////
bool Api::exists( const char* fsEntryName )
{
    return access( getNative( fsEntryName ), F_OK ) == 0;
}


bool Api::createFile( const char* fileName )
{
    bool result  = false;
    int  fd = ::open( getNative( fileName ), O_RDWR | O_CREAT | O_EXCL, 0666 );
    if ( fd != -1 )
    {
        result = true;
        ::close( fd );
    }

    return result;
}

bool Api::createDirectory( const char* dirName )
{
    return mkdir( getNative( dirName ), 0755 ) == 0;
}


bool Api::renameInPlace( const char* oldName, const char* newName )
{
    NameString nativeNewName = getNative( newName );
    return ::rename( getNative( oldName ), nativeNewName ) == 0;
}


bool Api::moveFile( const char* oldFileName, const char* newFileName )
{
    // Note: Under posix, the rename() command can/will move files
    return renameInPlace( oldFileName, newFileName );
}


bool Api::remove( const char* fsEntryName )
{
    return ::remove( getNative( fsEntryName ) ) == 0;
}


/////////////////////////////////////////////////////
bool Api::walkDirectory( const char*      dirToList,
                         DirectoryWalker& callback,
                         int              depth,
                         bool             filesOnly,
                         bool             dirsOnly
)
{
    DirList_ lister( dirToList, depth, filesOnly, dirsOnly );
    return lister.traverse( callback );
}

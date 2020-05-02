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
#include <io.h>
#include <stdio.h>
#include <direct.h>


///
using namespace Cpl::Io::File;


/// Helper method that does all of the work for populating the Info struct
static void populate_( Api::Info& infoOut, struct _stat& filestats )
{
    infoOut.m_isDir     = (filestats.st_mode&_S_IFDIR) == _S_IFDIR;
    infoOut.m_isFile    = (filestats.st_mode&_S_IFREG) == _S_IFREG;
    infoOut.m_size      = filestats.st_size;
    infoOut.m_mtime     = filestats.st_mtime;
    infoOut.m_readable  = (filestats.st_mode&_S_IREAD) == _S_IREAD;
    infoOut.m_writeable = (filestats.st_mode&_S_IWRITE) == _S_IWRITE;
}



/////////////////////////////////////////////////////
bool Api::getInfo( const char* fsEntryName, Info& infoOut )
{
    struct _stat filestats;
    if ( _stat( getNative( fsEntryName ), &filestats ) == 0 )
    {
        populate_( infoOut, filestats );
        return true;
    }

    return false;
}



/////////////////////////////////////////////////////
bool Api::canonicalPath( const char* relPath, Cpl::Text::String& absPath )
{
    int len;
    char* ptr    = absPath.getBuffer( len );
    char* result = _fullpath( ptr, getNative( relPath ), (size_t) len );
    absPath      = getStandard( absPath );
    return result != NULL;
}

bool Api::getCwd( Cpl::Text::String& cwd )
{
    int len;
    char* ptr    = cwd.getBuffer( len );
    char* result =  _getcwd( ptr, len );
    cwd          = getStandard( cwd );
    return result != NULL;
}


/////////////////////////////////////////////////////
bool Api::exists( const char* fsEntryName )
{
    return _access( getNative( fsEntryName ), 0 ) == 0;
}


bool Api::createFile( const char* fileName )
{
    bool result  = false;
    int  fd = _open( getNative( fileName ), _O_RDWR | _O_CREAT | _O_EXCL, _S_IREAD | _S_IWRITE );
    if ( fd != -1 )
    {
        result = true;
        _close( fd );
    }

    return result;
}

bool Api::createDirectory( const char* dirName )
{
    return _mkdir( getNative( dirName ) ) == 0;
}


bool Api::renameInPlace( const char* oldName, const char* newName )
{
    NameString nativeNewName = getNative( newName );
    return ::rename( getNative( oldName ), nativeNewName ) == 0;
}


bool Api::moveFile( const char* oldFileName, const char* newFileName )
{
    // Note: On Windoze, the rename() command does NOT move directories -->but it will move files
    //       The rename() command will fail with 'EACCES' when attempting to move a directory.
    return renameInPlace( oldFileName, newFileName );
}


bool Api::remove( const char* fsEntryName )
{
    if ( ::remove( getNative( fsEntryName ) ) != 0 )   // NOTE: remove() on Windoze does NOT delete directories!
    {
        return _rmdir( getNative( fsEntryName ) ) == 0;
    }

    return true;
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
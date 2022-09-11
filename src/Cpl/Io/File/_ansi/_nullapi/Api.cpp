/*-----------------------------------------------------------------------------
* This file is part of the Shift-Right Open Repository.  The Repository is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the root directory or on the Internet at
* http://www.shift-right.com/openrepo/license.htm
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Cpl/Io/File/Api.h"
#include <stdio.h>


///
using namespace Cpl::Io::File;

/////////////////////////////////////////////////////
bool Api::getInfo( const char* fsEntryName, Info& infoOut )
{
    // not supported by the Standard C Library
    return false;
}



/////////////////////////////////////////////////////
bool Api::canonicalPath( const char* relPath, Cpl::Text::String& absPath )
{
    return false;
}

bool Api::getCwd( Cpl::Text::String& cwd )
{
    return false;
}


/////////////////////////////////////////////////////
bool Api::exists( const char* fsEntryName )
{
    return false;
}


bool Api::createFile( const char* fileName )
{
    return false;
}

bool Api::createDirectory( const char* dirName )
{
    return false;
}


bool Api::renameInPlace( const char* oldName, const char* newName )
{
    return false;
}


bool Api::moveFile( const char* oldFileName, const char* newFileName )
{
    return false;
}


bool Api::remove( const char* fsEntryName )
{
    return false;
}


/////////////////////////////////////////////////////
bool Api::walkDirectory( const char*      dirToList,
                         DirectoryWalker& callback,
                         int              depth,
                         bool             filesOnly,
                         bool             dirsOnly
)
{
    return false;
}

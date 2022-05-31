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
#include "Cpl/Io/File/Common_.h"
#include "Private_.h"
#include <new>

///
using namespace Cpl::Io::File;

/////////////////////////////////////////////////////
Cpl::Io::Descriptor Common_::open( const char* fileEntryName, bool readOnly, bool forceCreate, bool forceEmptyFile )
{
    // Set open flags as requested
    int flags = readOnly ? O_RDONLY : O_RDWR;
    if ( forceCreate )
    {
        flags |= O_CREAT;

    }
    if ( forceEmptyFile )
    {
        flags |= O_TRUNC;
    }

    // Allocate a file instance
    FatFile* fileHandle = new( std::nothrow ) FatFile;
    if ( fileHandle != nullptr )
    {
        // Attempt to Open the file
        if ( !fileHandle->open( g_arduino_sdfat_fs.vwd(), fileEntryName, flags ) )
        {
            delete fileHandle;
            fileHandle = 0;
        }
    }

    // Return the file handle
    Cpl::Io::Descriptor fd( (void*)fileHandle );
    return fd;
}

/////////////////////////////////////////////////////
bool Common_::currentPos( Cpl::Io::Descriptor fd, unsigned long& curpos )
{
    if ( fd.m_handlePtr == 0 )
    {
        return 0;
    }

    FatFile* fileHandle = (FatFile*)fd.m_handlePtr;
    curpos = fileHandle->curPosition();
    return true;
}


bool Common_::setRelativePos( Cpl::Io::Descriptor fd, long deltaOffset )
{
    if ( fd.m_handlePtr == 0 )
    {
        return false;
    }

    FatFile* fileHandle = (FatFile*)fd.m_handlePtr;
    return fileHandle->seekCur( (int32_t)deltaOffset );
}


bool Common_::setAbsolutePos( Cpl::Io::Descriptor fd, unsigned long newoffset )
{
    if ( fd.m_handlePtr == 0 )
    {
        return false;
    }

    FatFile* fileHandle = (FatFile*)fd.m_handlePtr;
    return fileHandle->seekSet( (uint32_t)newoffset );
}


bool Common_::setToEof( Cpl::Io::Descriptor fd )
{
    if ( fd.m_handlePtr == 0 )
    {
        return false;
    }

    FatFile* fileHandle = (FatFile*)fd.m_handlePtr;
    return fileHandle->seekEnd( 0 );
}


bool Common_::length( Cpl::Io::Descriptor fd, unsigned long& length )
{
    if ( fd.m_handlePtr == 0 )
    {
        return 0; // Error -->return zero
    }

    FatFile* fileHandle = (FatFile*)fd.m_handlePtr;
    length              = fileHandle->fileSize();
    return true;
}


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
#include <windows.h>


///
using namespace Cpl::Io::File;

/////////////////////////////////////////////////////
Cpl::Io::Descriptor Common_::open( const char* fileEntryName, bool readOnly, bool forceCreate, bool forceEmptyFile )
{
    // Set open flags as requested
    DWORD createOpt = OPEN_EXISTING;
    DWORD accessOpt = readOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;
    if ( forceCreate )
    {
        createOpt = OPEN_ALWAYS;
    }
    if ( forceEmptyFile )
    {
        createOpt = CREATE_ALWAYS;
    }

    // Open the file
    Cpl::Io::Descriptor fd( CreateFile( Cpl::Io::File::Api::getNative( fileEntryName ),
                            accessOpt,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            0,
                            createOpt,
                            FILE_ATTRIBUTE_NORMAL,
                            0
    )
    );
    return fd;
}

/////////////////////////////////////////////////////
bool Common_::currentPos( Cpl::Io::Descriptor fd, unsigned long& curPos )
{
    if ( (HANDLE) (fd.m_handlePtr) == INVALID_HANDLE_VALUE )
    {
        return 0;
    }

    DWORD pos = SetFilePointer( (HANDLE) (fd.m_handlePtr), 0, 0, FILE_CURRENT );
    curPos    = (unsigned long) pos;
    return pos != INVALID_SET_FILE_POINTER;
}


bool Common_::setRelativePos( Cpl::Io::Descriptor fd, long deltaOffset )
{
    if ( (HANDLE) (fd.m_handlePtr) == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    return SetFilePointer( (HANDLE) (fd.m_handlePtr), deltaOffset, 0, FILE_CURRENT ) != INVALID_SET_FILE_POINTER;
}


bool Common_::setAbsolutePos( Cpl::Io::Descriptor fd, unsigned long newoffset )
{
    if ( (HANDLE) (fd.m_handlePtr) == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    return SetFilePointer( (HANDLE) (fd.m_handlePtr), newoffset, 0, FILE_BEGIN ) != INVALID_SET_FILE_POINTER;
}


bool Common_::setToEof( Cpl::Io::Descriptor fd )
{
    if ( (HANDLE) (fd.m_handlePtr) == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    return SetFilePointer( (HANDLE) (fd.m_handlePtr), 0, 0, FILE_END ) != INVALID_SET_FILE_POINTER;
}



bool Common_::length( Cpl::Io::Descriptor fd, unsigned long& length )
{
    if ( (HANDLE) (fd.m_handlePtr) == INVALID_HANDLE_VALUE )
    {
        return 0; // Error -->return zero
    }

    DWORD len = GetFileSize( (HANDLE) (fd.m_handlePtr), 0 );
    length = (unsigned long) len;
    return len != INVALID_FILE_SIZE;

}


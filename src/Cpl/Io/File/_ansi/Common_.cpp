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
#include "Cpl/Io/File/Common_.h"
#include <stdio.h>


///
using namespace Cpl::Io::File;

/////////////////////////////////////////////////////
Cpl::Io::Descriptor Common_::open( const char* fileEntryName, bool readOnly, bool forceCreate, bool forceEmptyFile )
{
    const char* mode = "r+"; // default to read+write, no create, no truncate

    // Read only (no create, no truncate)
    if ( readOnly )
    {
        mode = "r";
    }

    // Read/Write (create, truncate). NOTE: using FILE* handle, I only have the option to create AND truncate (can't separate the two options)
    else if ( forceCreate || forceEmptyFile )
    {
        mode = "w+";
    }

    // Open the file
    Cpl::Io::Descriptor fd( fopen( Cpl::Io::File::Api::getNative( fileEntryName ), mode ) );
    return fd;
}


/////////////////////////////////////////////////////
bool Common_::currentPos( Cpl::Io::Descriptor fd, unsigned long& currentPos )
{
    if ( fd.m_handlePtr == 0 )
    {
        return false;
    }

    long pos   = ftell( (FILE*) (fd.m_handlePtr) );
    currentPos = (unsigned long) pos;
    return pos != -1L;
}

bool Common_::setRelativePos( Cpl::Io::Descriptor fd, long deltaOffset )
{
    if ( fd.m_handlePtr == 0 )
    {
        return false;
    }

    return fseek( (FILE*) (fd.m_handlePtr), deltaOffset, SEEK_CUR ) == 0;
}

bool Common_::setAbsolutePos( Cpl::Io::Descriptor fd, unsigned long newoffset )
{
    if ( fd.m_handlePtr == 0 )
    {
        return false;
    }

    return fseek( (FILE*) (fd.m_handlePtr), (long) newoffset, SEEK_SET ) == 0;
}

bool Common_::setToEof( Cpl::Io::Descriptor fd )
{
    if ( fd.m_handlePtr == 0 )
    {
        return false;
    }
    return fseek( (FILE*) (fd.m_handlePtr), 0, SEEK_END ) == 0;
}



/////////////////////////////////////////////////////
// Brute force approach to get the length of the file -->set the file 
// position indicator to EOF and use the EOF byte offset as the length.
bool Common_::length( Cpl::Io::Descriptor fd, unsigned long& length )
{
    if ( fd.m_handlePtr == 0 )
    {
        return false; // Error 
    }

    // Cache the current file position indicator (note: using fgetpos() because it more portable than using fseek for caching/restoring a position)
    fpos_t cur;
    if ( fgetpos( (FILE*) (fd.m_handlePtr), &cur ) != 0 )
    {
        return false; // Error 
    }

    // Goto EOF
    if ( fseek( (FILE*) (fd.m_handlePtr), 0, SEEK_END ) != 0 )
    {
        return false; // Error 
    }

    // Get the byte offset of EOF      
    long len = ftell( (FILE*) (fd.m_handlePtr) );

    // Restore the previous file position indicator
    if ( len == -1 || fsetpos( (FILE*) (fd.m_handlePtr), &cur ) != 0 )
    {
        return false; // Error 
    }

    // Return the length
    length = (unsigned long) len;
    return true;
}

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

#include "Cpl/Io/Stdio/Input_.h"
#include "Cpl/System/FatalError.h"
#include "Private_.h"

//
using namespace Cpl::Io::Stdio;



///////////////////
Input_::Input_( int fd )
    : m_inFd( (void*)0 )
    , m_inEos( false )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_().  Constructed with an 'int' instead of a 'FatFile*'." );
}

Input_::Input_( void* handle )
    : m_inFd( handle )
    , m_inEos( false )
{
}

Input_::Input_( Cpl::Io::Descriptor streamfd )
    : m_inFd( streamfd )
    , m_inEos( false )
{
}

Input_::Input_( void )
    : m_inFd( (void*)0 )
    , m_inEos( true )
{
}

Input_::~Input_( void )
{
    close();
}

bool Input_::isOpened( void )
{
    return m_inFd.m_handlePtr != 0;
}


//////////////////////
void Input_::activate( int fd )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_::activate(handle).  Activated with an 'INT' instead of a 'FatFile*'." );
}

void Input_::activate( void* handle )
{
    // Only activate if already closed 
    if ( m_inFd.m_handlePtr == 0 )
    {
        m_inFd.m_handlePtr = handle;
        m_inEos            = false;
    }
    else
    {
        Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_::activate(FatFile*).  Attempting to Activate an already opened stream." );
    }
}

void Input_::activate( Cpl::Io::Descriptor streamfd )
{
    // Only activate if already closed 
    if ( m_inFd.m_handlePtr == 0 )
    {
        m_inFd  = streamfd;
        m_inEos = false;
    }
    else
    {
        Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_::activate(descriptor).  Attempting to Activate an already opened stream." );
    }
}

//////////////////////
bool Input_::read( void* buffer, int numBytes, int& bytesRead )
{
    // Trap that the stream has been CLOSED!
    if ( m_inFd.m_handlePtr == 0 )
    {
        return false;
    }

    // Ignore read requests of ZERO bytes
    if ( numBytes == 0 )
    {
        bytesRead = 0;
        return true;
    }

    // perform the read
    FatFile* fileHandle = (FatFile*) m_inFd.m_handlePtr;
    bytesRead           = fileHandle->read( buffer, numBytes );
    m_inEos             = bytesRead == 0 ? true : false;
    return bytesRead > 0;
}


bool Input_::available()
{
    // Trap that the stream has been CLOSED!
    if ( m_inFd.m_handlePtr == 0 )
    {
        return false;
    }

    FatFile* fileHandle = (FatFile*) m_inFd.m_handlePtr;
    int   nbytes        = fileHandle->available();
    return nbytes > 0;
}

bool Input_::isEos( void )
{
    return m_inEos;
}

void Input_::close()
{
    if ( m_inFd.m_handlePtr != 0 )
    {
        FatFile* fileHandle = (FatFile*) m_inFd.m_handlePtr;
        fileHandle->close();
        m_inFd.m_handlePtr = 0;
    }
}

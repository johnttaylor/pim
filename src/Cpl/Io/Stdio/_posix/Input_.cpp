/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Cpl/Io/Stdio/Input_.h"
#include "Cpl/System/FatalError.h"
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

//
using namespace Cpl::Io::Stdio;



///////////////////
Input_::Input_( int fd )
    : m_inFd( fd )
    , m_inEos( false )
{
}

Input_::Input_( void* handle )
    : m_inFd( -1 )
    , m_inEos( false )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_().  Constructed with an 'HANDLE' instead of a 'int'." );
}

Input_::Input_( Cpl::Io::Descriptor streamfd )
    : m_inFd( streamfd )
    , m_inEos( false )
{
}

Input_::Input_( void )
    : m_inFd( -1 )
    , m_inEos( true )
{
}

Input_::~Input_( void )
{
}

bool Input_::isOpened( void )
{
    return m_inFd.m_fd != -1;
}


//////////////////////
void Input_::activate( int fd )
{
    // Only activate if already closed 
    if ( m_inFd.m_fd == -1 )
    {
        m_inFd.m_fd = fd;
        m_inEos     = false;
    }
    else
    {
        Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_::activate(int).  Attempting to Activate an already opened stream." );
    }
}

void Input_::activate( void* handle )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_::activate(handle).  Activated with an 'HANDLE' instead of a 'int'." );
}

void Input_::activate( Cpl::Io::Descriptor streamfd )
{
    // Only activate if already closed 
    if ( m_inFd.m_fd == -1 )
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
    if ( m_inFd.m_fd == -1 )
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
    bytesRead = (int) ::read( m_inFd.m_fd, buffer, numBytes );
	//int lastError = errno;
	m_inEos = bytesRead == 0? true : false;
	//printf( "m_inEos=%d, bytesRead=%d, errno=%d\n", m_inEos, bytesRead, lastError );
	return bytesRead > 0;
}


bool Input_::available()
{
    // Trap that the stream has been CLOSED!
    if (m_inFd.m_fd == -1)
    {
        return false;
    }

    int nbytes;
    ioctl( m_inFd.m_fd, FIONREAD, &nbytes );
    return nbytes > 0 ;
}

bool Input_::isEos( void )
{
	return m_inEos;
}

void Input_::close()
{
    if ( m_inFd.m_fd != -1 )
    {
        ::close( m_inFd.m_fd );
        m_inFd.m_fd = -1;
    }
}

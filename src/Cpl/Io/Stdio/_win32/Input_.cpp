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
#include <windows.h>

//
using namespace Cpl::Io::Stdio;



///////////////////
Input_::Input_( int fd )
    : m_inFd( (void*) INVALID_HANDLE_VALUE )
    , m_inEos( false )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_().  Constructed with an 'int' instead of a 'HANDLE'." );
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
    : m_inFd( (void*) INVALID_HANDLE_VALUE )
    , m_inEos( true )
{
}

Input_::~Input_( void )
{
}


bool Input_::isOpened()
{
    return m_inFd.m_handlePtr != (void*) INVALID_HANDLE_VALUE;
}


//////////////////////
void Input_::activate( int fd )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_::activate(fd).  Activated with an 'int' instead of a 'HANDLE'." );
}

void Input_::activate( void* handle )
{
    // Only activate if already closed 
    if ( m_inFd.m_handlePtr == (void*) INVALID_HANDLE_VALUE )
    {
        m_inFd.m_handlePtr = handle;
        m_inEos           = false;
    }
    else
    {
        Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_::activate(handle).  Attempting to Activate an already opened stream." );
    }
}

void Input_::activate( Cpl::Io::Descriptor streamfd )
{
    // Only activate if already closed 
    if ( m_inFd.m_handlePtr == (void*) INVALID_HANDLE_VALUE )
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
    if ( m_inFd.m_handlePtr == (void*) INVALID_HANDLE_VALUE )
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
    unsigned long work;
    BOOL result = ReadFile( (HANDLE) (m_inFd.m_handlePtr), buffer, numBytes, &work, 0 );
    bytesRead   = (int) work;
	DWORD lastError = GetLastError();
	m_inEos = (result != 0 && bytesRead > 0)? false: lastError == ERROR_HANDLE_EOF || lastError == ERROR_BROKEN_PIPE || bytesRead == 0? true : false;
	//printf( "m_inEos=%d, result=%ld, bytesRead=%d, LastError=%ld\n", m_inEos, result, bytesRead, lastError );
	return result != 0 && bytesRead > 0;
}


bool Input_::available()
{
    // Trap that the stream has been CLOSED!
    if (m_inFd.m_handlePtr == (void*)INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD signaled = WaitForSingleObject( m_inFd.m_handlePtr, 0 );
    return signaled == WAIT_OBJECT_0;
}

bool Input_::isEos()
{
	return m_inEos;
}

void Input_::close()
{
    if ( m_inFd.m_handlePtr != (void*) INVALID_HANDLE_VALUE )
    {
        CloseHandle( (HANDLE) (m_inFd.m_handlePtr) );
        m_inFd.m_handlePtr = (void*) INVALID_HANDLE_VALUE;
    }
}

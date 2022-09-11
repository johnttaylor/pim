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

#include "Cpl/Io/Stdio/Output_.h"
#include "Cpl/System/FatalError.h"
#include <windows.h>


//
using namespace Cpl::Io::Stdio;


//////////////////////
Output_::Output_( int fd )
    : m_outFd( (void*) INVALID_HANDLE_VALUE )
    , m_outEos( false )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Output_().  Constructed with an 'int' instead of a 'HANDLE'." );
}

Output_::Output_( void* handle )
    : m_outFd( handle )
    , m_outEos( false )
{
}

Output_::Output_( Cpl::Io::Descriptor streamfd )
    : m_outFd( streamfd )
    , m_outEos( false )
{
}

Output_::Output_( void )
    : m_outFd( (void*) INVALID_HANDLE_VALUE )
    , m_outEos( true )
{
}

Output_::~Output_( void )
{
}

bool Output_::isOpened()
{
    return m_outFd.m_handlePtr != (void*) INVALID_HANDLE_VALUE;
}


//////////////////////
void Output_::activate( int fd )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Output_::activate(fd).  Activated with an 'int' instead of a 'HANDLE'." );
}

void Output_::activate( void* handle )
{
    // Only activate if already closed 
    if ( m_outFd.m_handlePtr == (void*) INVALID_HANDLE_VALUE )
    {
        m_outFd.m_handlePtr = handle;
        m_outEos            = false;
    }
    else
    {
        Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Output_::activate(handle).  Attempting to Activate an already opened stream." );
    }
}

void Output_::activate( Cpl::Io::Descriptor streamfd )
{
    // Only activate if already closed 
    if ( m_outFd.m_handlePtr == (void*) INVALID_HANDLE_VALUE )
    {
        m_outFd  = streamfd;
        m_outEos = false;
    }
    else
    {
        Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Output_::activate(descriptor).  Attempting to Activate an already opened stream." );
    }
}


//////////////////////
bool Output_::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    // Trap that the stream has been CLOSED!
    if ( m_outFd.m_handlePtr == (void*) INVALID_HANDLE_VALUE )
    {
        return false;
    }

    // Ignore write requests of ZERO bytes
    if ( maxBytes == 0 )
    {
        bytesWritten = 0;
        return true;
    }

    // perform the write
    unsigned long work;
    BOOL result  = WriteFile( (HANDLE) (m_outFd.m_handlePtr), buffer, maxBytes, &work, 0 );
    bytesWritten = (int) work;
	DWORD lastError = GetLastError();
	m_outEos = (result != 0 || bytesWritten > 0)? false : lastError == ERROR_HANDLE_EOF || lastError == ERROR_BROKEN_PIPE || bytesWritten == 0? true : false;
	//printf( "m_outEos=%d, result=%ld, LastError=%ld\n", m_outEos, result, lastError );
	return result != 0 && bytesWritten > 0;
}


void Output_::flush()
{
    // Ignore if the stream has been CLOSED!
    if ( m_outFd.m_handlePtr != (void*) INVALID_HANDLE_VALUE )
    {
        FlushFileBuffers( (HANDLE) (m_outFd.m_handlePtr) );
    }
}

bool Output_::isEos()
{
	return m_outEos;
}

void Output_::close()
{
    if ( m_outFd.m_handlePtr != (void*) INVALID_HANDLE_VALUE )
    {
        CloseHandle( (HANDLE) (m_outFd.m_handlePtr) );
        m_outFd.m_handlePtr = (void*) INVALID_HANDLE_VALUE;
    }
}

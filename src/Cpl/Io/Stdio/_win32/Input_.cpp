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

#include "colony_config.h"
#include "Cpl/Io/Stdio/Input_.h"
#include "Cpl/System/FatalError.h"
#include <windows.h>

//
using namespace Cpl::Io::Stdio;

/* THIS IS HACK, but I discovered the issue/partial-solution well after the design
   and implementation of the Stdio/File-IO stuffs - so here is a hack that
   is mostly 'transparent' to the clients

   THE PROBLEM with STDIN:
   Windows consider mouse events, change in focus, etc. as events that get
   routed to the StdIn Handle.  The ReadFile() method ignores these 'non-character'
   events, but WaitForSingleObject() does NOT. This means that WaitForSingleObject()
   can/will return false positive (that data is available) when in fact the
   ReadFile() call will block.

   AND to make matters worse - the hack ONLY WORKS where is a 'live' terminal
   window, i.e. when the terminal's stdin is piped from a file - the hack DOES
   NOT WORK! Not a very good solution :( - but it solved my immediate needs
*/

#ifdef USE_CPL_IO_STDIO_WIN32_STDIN_CONSOLE_HACK
#define DATA_AVAILABLE \
    if ( m_inFd.m_handlePtr == (void*) GetStdHandle( STD_INPUT_HANDLE ) )   \
    { \
        return _kbhit(); \
    } \
    else \
    { \
        DWORD signaled = WaitForSingleObject( m_inFd.m_handlePtr, 0 ); \
        return signaled == WAIT_OBJECT_0; \
    }

// NO work-around
#else
#define DATA_AVAILABLE \
    DWORD signaled = WaitForSingleObject( m_inFd.m_handlePtr, 0 ); \
    return signaled == WAIT_OBJECT_0;

#endif

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

#include <conio.h>

bool Input_::available()
{
    // Trap that the stream has been CLOSED!
    if (m_inFd.m_handlePtr == (void*)INVALID_HANDLE_VALUE)
    {
        return false;
    }
    
    // See comments at the top of the file
    DATA_AVAILABLE
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

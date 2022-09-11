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

#include <stdio.h>
#include "Cpl/Io/Stdio/Input_.h"
#include "Cpl/System/FatalError.h"

//
using namespace Cpl::Io::Stdio;

///////////////////
Input_::Input_( int fd )
    :m_inFd( 0 )
    , m_inEos( false )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_().  Constructed with an 'int' instead of a 'FILE*'." );
}

Input_::Input_( void* handle )
    :m_inFd( handle )
    , m_inEos( false )
{
}

Input_::Input_( Cpl::Io::Descriptor streamfd )
    : m_inFd( streamfd )
    , m_inEos( false )
{
}

Input_::Input_( void )
    : m_inFd( 0 )
    , m_inEos( true )
{
}

Input_::~Input_( void )
{
}

bool Input_::isOpened()
{
    return m_inFd.m_handlePtr != 0;
}


//////////////////////
void Input_::activate( int fd )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_::activate(fd).  Activated with an 'int' instead of a 'FILE*'." );
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
        Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Input_::activate(handle).  Attempting to Activate an already opened stream." );
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
    if ( !m_inFd.m_handlePtr )
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
    bytesRead   = (int) fread( buffer, sizeof( char ), numBytes, (FILE*) (m_inFd.m_handlePtr) );
	bool result = true;
	m_inEos     = false;
	if ( bytesRead <= 0 )
	{
		result  = false;
		m_inEos = feof( (FILE*) ( m_inFd.m_handlePtr ) ) ? true: false;
	}
	//printf( "eos=%d, ferr=%d, bytesRead=%d, result=%d\n", m_inEos, ferror( (FILE*) ( m_inFd.m_handlePtr ) ), bytesRead, result );
	clearerr( (FILE*) ( m_inFd.m_handlePtr ) );
    return result;
}

bool Input_::available()
{
    printf( "ANSI AVAIALBLE(), return:%d\n", m_inFd.m_handlePtr != 0 );
    // CURRENTLY NOT SUPPORTED -->RETURN TRUE (as per documentation/contract) WHEN OPENED
    return m_inFd.m_handlePtr != 0;
}

bool Input_::isEos()
{
	return m_inEos;
}

void Input_::close()
{
    if ( m_inFd.m_handlePtr )
    {
        fclose( (FILE*) (m_inFd.m_handlePtr) );
        m_inFd.m_handlePtr = 0;
    }
}

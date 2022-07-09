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

#include <stdio.h>
#include "Cpl/Io/Stdio/Output_.h"
#include "Cpl/System/FatalError.h"


//
using namespace Cpl::Io::Stdio;


//////////////////////
Output_::Output_( int fd )
    : m_outFd( 0 )
    , m_outEos( false )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Output_().  Constructed with an 'int' instead of a 'FILE*'." );
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
    : m_outFd( 0 )
    , m_outEos( false )
{
}

Output_::~Output_( void )
{
}

bool Output_::isOpened()
{
    return m_outFd.m_handlePtr != 0;
}


//////////////////////
void Output_::activate( int fd )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Output_::activate(fd).  Activated with an 'int' instead of a 'FILE*'." );
}

void Output_::activate( void* handle )
{
    // Only activate if already closed 
    if ( m_outFd.m_handlePtr == 0 )
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
    if ( m_outFd.m_handlePtr == 0 )
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
    if ( !m_outFd.m_handlePtr )
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
    bytesWritten = (int) fwrite( buffer, sizeof( char ), maxBytes, (FILE*) (m_outFd.m_handlePtr) );
	bool result  = true;
	m_outEos     = false;
	if ( bytesWritten <= 0 )
	{
		m_outEos = feof( (FILE*) ( m_outFd.m_handlePtr ) )? true: false;
		result   = false;
	}
	//printf( "eos=%d, ferr=%d, bytesRead=%d, result=%d\n", m_outEos, ferror( (FILE*) ( m_inFd.m_handlePtr ) ), bytesWritten, result );
	clearerr( (FILE*) ( m_outFd.m_handlePtr ) );
	return result;
}

void Output_::flush()
{
    // Ignore if the stream has been CLOSED!
    if ( m_outFd.m_handlePtr )
    {
        fflush( (FILE*) (m_outFd.m_handlePtr) );
    }
}

bool Output_::isEos()
{
	return m_outEos;
}

void Output_::close()
{
    if ( m_outFd.m_handlePtr )
    {
        fclose( (FILE*) (m_outFd.m_handlePtr) );
        m_outFd.m_handlePtr = 0;
    }
}

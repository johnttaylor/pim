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

#include "Cpl/Io/Stdio/Output_.h"
#include "Cpl/System/FatalError.h"
#include "Private_.h"


//
using namespace Cpl::Io::Stdio;


//////////////////////
Output_::Output_( int fd )
    : m_outFd( (void*)0 )
    , m_outEos( false )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Output_().  Constructed with an 'int' instead of a 'FatFile*'." );
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
    : m_outFd( (void*) 0 )
    , m_outEos( true )
{
}

Output_::~Output_( void )
{
}

bool Output_::isOpened( void )
{
    return m_outFd.m_handlePtr != 0;
}

//////////////////////
void Output_::activate( int fd )
{
    Cpl::System::FatalError::logf( "Cpl:Io::Stdio::Output_::activate(int).  Activated with an 'INT' instead of a 'FatFile*'." );
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
    if ( m_outFd.m_handlePtr == 0 )
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
    FatFile* fileHandle = (FatFile*) m_outFd.m_handlePtr;
    bytesWritten        = fileHandle->write( buffer, maxBytes );
	m_outEos            = bytesWritten == 0? true : false;
	return bytesWritten > 0;
}


void Output_::flush()
{
    // Ignore if the stream has been CLOSED!
    if ( m_outFd.m_handlePtr != 0 )
    {
        FatFile* fileHandle = (FatFile*) m_outFd.m_handlePtr;
        fileHandle->sync();
    }
}

bool Output_::isEos( void )
{
	return m_outEos;
}

void Output_::close()
{
    if ( m_outFd.m_fd != 0 )
    {
        FatFile* fileHandle = (FatFile* ) m_outFd.m_handlePtr;
        fileHandle->sync();
        fileHandle->close();
        m_outFd.m_handlePtr = 0;
    }
}

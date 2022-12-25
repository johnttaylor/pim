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

#include <winsock2.h>
#include "Cpl/Io/Tcp/InputOutput.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Private_.h"
#include "Cpl/System/Shutdown.h"

//
// NOTE: The implementation ASSUMES that when the socket was created that
//       is set to non-blocking mode. For example:
//
//		 u_long iMode = 1;	       
//		 ioctlsocket( ( (SOCKET) ( m_fd.m_handlePtr ) ), FIONBIO, &iMode ); 
//

/////////////////////////////////////////////////////////////////////////////
// Use the CPL startup and shutdown hooks to initialize the Winsock library

// Anonymous namespace
namespace {

////
class ExitHandler : public Cpl::System::Shutdown::Handler
{
protected:
    ///
    int notify( int exit_code )
    {
        WSACleanup();
        return exit_code;
    }
};


////
class RegisterInitHandler : public Cpl::System::StartupHook_
{
protected:
    ///
    ExitHandler m_shutdown;

public:
    ///
    RegisterInitHandler() :StartupHook_( eMIDDLE_WARE ) {}


protected:
    /// 
    void notify( InitLevel_T init_level )
    {
        WSADATA wsaData;

        if ( WSAStartup( 0x202, &wsaData ) == SOCKET_ERROR )
        {
            int err = WSAGetLastError();
            WSACleanup();
            Cpl::System::FatalError::logf( "WSAStartup failed with error", err );
        }

        // Register my shutdown handler
        Cpl::System::Shutdown::registerHandler( m_shutdown );
    }

};



}; // end namespace

///
static RegisterInitHandler autoRegister_systemInit_hook;
/////////////////////////////////////////////////////////////////////////////


///
using namespace Cpl::Io::Tcp;


/////////////////////
InputOutput::InputOutput()
    : m_fd( ((void*) INVALID_SOCKET) )
    , m_eos( false )
{
}

InputOutput::InputOutput( Cpl::Io::Descriptor fd )
    : m_fd( fd )
    , m_eos( false )
{
}

InputOutput::~InputOutput( void )
{
    close();
}


///////////////////
void InputOutput::activate( Cpl::Io::Descriptor fd )
{
    // Only activate if already closed 
    if ( ((SOCKET) (m_fd.m_handlePtr)) == INVALID_SOCKET )
    {
        m_fd  = fd;
        m_eos = false;
    }
    else
    {
        Cpl::System::FatalError::logf( "Cpl:Io::Tcp::InputOutput::activate().  Attempting to Activate an already opened stream." );
    }
}


///////////////////
bool InputOutput::read( void* buffer, int numBytes, int& bytesRead )
{
    // Throw an error if the socket had already been closed
    if ( ((SOCKET) (m_fd.m_handlePtr)) == INVALID_SOCKET )
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
    bytesRead = recv( ((SOCKET) (m_fd.m_handlePtr)), (char*) buffer, numBytes, 0 );
    if ( bytesRead > 0 )
    {
        m_eos = false;
    }
    else if ( WSAGetLastError() == WSAEWOULDBLOCK )
    {
        bytesRead = 0;
        m_eos     = false;
    }
    else
    {
        m_eos = true;
        close();
    }
    return !m_eos;
}

bool InputOutput::available()
{
    unsigned long nbytes=1;            // NOTE: If there is error -->then I will return true
    ioctlsocket( ((SOCKET) (m_fd.m_handlePtr)), FIONREAD, &nbytes );
    return nbytes > 0 ? true : false;
}


//////////////////////
bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    // Throw an error if the socket had already been closed
    if ( ((SOCKET) (m_fd.m_handlePtr)) == INVALID_SOCKET )
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
    bytesWritten = send( ((SOCKET) (m_fd.m_handlePtr)), (char*) buffer, maxBytes, 0 );
    if ( bytesWritten > 0 )
    {
        m_eos = false;
    }
    else if ( bytesWritten <= 0 && WSAGetLastError() == WSAEWOULDBLOCK )
    {
        bytesWritten = 0;
        m_eos        = false;
    }
    else
    {
        m_eos = true;
        close();
    }
    return !m_eos;
}

void InputOutput::flush()
{
    // I could use WSAIoctl() here with SIO_FLUSH - but according
    // to the Microsoft documentation - WSAIoctrl w/SIO_FLUSH could
    // block (unless using overlapped IO) - which is not the designed
    // behavior for this call -->so we will skip it for now (jtt 2-14-2015)
}

bool InputOutput::isEos()
{
    return m_eos;
}

void InputOutput::close()
{
    m_eos = true;
    if ( ((SOCKET) (m_fd.m_handlePtr)) != INVALID_SOCKET )
    {
        closesocket( ((SOCKET) (m_fd.m_handlePtr)) );
        m_fd.m_handlePtr = (void*) INVALID_SOCKET;
    }
}

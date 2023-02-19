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

#include "Private_.h"
#include "Cpl/Io/Tcp/InputOutput.h"
#include "Cpl/System/FatalError.h"
#include "pico/cyw43_arch.h"

using namespace Cpl::Io::Tcp;
using namespace Cpl::Io::Tcp::lwIP::Picow;

#ifdef PICO_CYW43_ARCH_THREADSAFE_BACKGROUND
#define PICO_LOCK   cyw43_arch_lwip_begin      
#define PICO_UNLOCK cyw43_arch_lwip_end

#elif PICO_CYW43_ARCH_POLL
#define PICO_LOCK   g_internalLock.lock      
#define PICO_UNLOCK g_internalLock.unlock

#else
#error MUST be compiled with: PICO_CYW43_ARCH_POLL=1 or PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1
#endif




/////////////////////
InputOutput::InputOutput()
    : m_fd( nullptr )
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
    Socket_T* newfd = (Socket_T *) fd.m_handlePtr;

    PICO_LOCK();

    // Only activate if already closed 
    if ( m_fd.m_handlePtr == nullptr )
    {
        m_fd  = (void*) newfd;
        m_eos = false;
    }
    else
    {
        PICO_UNLOCK();
        Cpl::System::FatalError::logf( "Cpl:Io::Tcp::InputOutput::activate().  Attempting to Activate an already opened stream." );
        return;
    }
    PICO_UNLOCK();
}


///////////////////
bool InputOutput::read( void* buffer, int numBytes, int& bytesRead )
{
    Socket_T* fd     = (Socket_T *) m_fd.m_handlePtr;
    bool      result = false;
    PICO_LOCK();

    // Throw an error if the socket had already been closed
    if ( fd != nullptr )
    {
        // Fail if there was socket error
        if ( fd->lwipPcb == nullptr )
        {
            close();
        }

        // Ignore read requests of ZERO bytes 
        else if ( numBytes == 0 )
        {
            bytesRead = 0;
            result    = true;
        }

        // Check if there is any received data
        else if ( fd->recvPbuf == nullptr )
        {
            // No data received -->return zero bytes read
            bytesRead = 0;
            result    = true;
        }

        // Read some data...
        else
        {
            result = true;

            // Determine how much is left in the pbuf
            uint16_t remainingBytes = fd->recvPbuf->tot_len - fd->rxOffset;
            if ( numBytes > remainingBytes )
            {
                numBytes = remainingBytes;
            }

            // Copy the data to the caller's buffer
            bytesRead = pbuf_copy_partial( fd->recvPbuf, buffer, numBytes, fd->rxOffset );
            tcp_recved( fd->lwipPcb, bytesRead );

            // Consumed all of the data -->free the buffer
            if ( fd->recvPbuf->tot_len == bytesRead + fd->rxOffset )
            {
                pbuf_free( fd->recvPbuf );
                fd->rxOffset = 0;
                fd->recvPbuf = nullptr;
            }

            // Adjust the offset to begin at the start of the 'unread data'
            else
            {
                fd->rxOffset += bytesRead;
            }
        }
    }

    PICO_UNLOCK();
    return result;
}

bool InputOutput::available()
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;

    PICO_LOCK();
    bool haveData = fd->recvPbuf != nullptr;
    PICO_UNLOCK();

    return haveData;
}


//////////////////////
bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    Socket_T* fd     = (Socket_T *) m_fd.m_handlePtr;
    bool      result = false;
    PICO_LOCK();

    // Throw an error if the socket had already been closed
    if ( fd != nullptr )
    {
        // Fail if there was socket error
        if ( fd->lwipPcb == nullptr )
        {
            close();
        }

        // Ignore write requests of ZERO bytes 
        else if ( maxBytes == 0 )
        {
            bytesWritten = 0;
            result       = true;
        }

        // Attempt to write data...
        else
        {
            // Check if there is any outgoing buffer space
            uint16_t availLen = tcp_sndbuf( fd->lwipPcb );
            if ( availLen == ERR_MEM || availLen == 0 )
            {
                bytesWritten = 0;
                result = true;
            }
            else
            {
                // Adjust how many bytes can be sent
                if ( maxBytes > availLen )
                {
                    maxBytes = availLen;
                }

                // Send the data
                err_t err = tcp_write( fd->lwipPcb, buffer, maxBytes, TCP_WRITE_FLAG_COPY );
                if ( !err )
                {
                    // Success
                    bytesWritten = maxBytes;
                    result       = true;
                }

                // If there is out of memory error -->wait for something to free up
                else if ( err == ERR_MEM )
                {
                    bytesWritten = 0;
                    result       = true;
                }

                // Unrecoverable error
                else
                {
                    close();
                }
            }
        }
    }

    PICO_UNLOCK();
    return result;
}

void InputOutput::flush()
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;
    PICO_LOCK();

    if ( fd != nullptr && fd->lwipPcb )
    {
        tcp_output( fd->lwipPcb );
    }

    PICO_LOCK();
}

bool InputOutput::isEos()
{
    PICO_LOCK();
    bool eos = m_eos;
    PICO_UNLOCK();
    return eos;
}

void InputOutput::close()
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;

    PICO_LOCK();

    // Ignore if the connection had already been closed
    if ( fd != nullptr && fd->lwipPcb != nullptr )
    {
        // Make sure any unprocessed PBUF gets freed
        if ( fd->recvPbuf )
        {
            pbuf_free( fd->recvPbuf );
            fd->recvPbuf = nullptr;
        }

        tcp_close( fd->lwipPcb );
    }

    fd->lwipPcb      = nullptr;
    m_fd.m_handlePtr = nullptr;
    m_eos            = true;
    PICO_UNLOCK();
}

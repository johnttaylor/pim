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
#include "Cpl/System/FatalError.h"
#include "Cpl/Io/Tcp/lwIP/Picow/AsyncConnector.h"
#include "pico/cyw43_arch.h"


using namespace Cpl::Io::Tcp::lwIP::Picow;

#ifdef PICO_CYW43_ARCH_THREADSAFE_BACKGROUND
#define PICO_CB_LOCK()
#define PICO_CB_UNLOCK()

#elif PICO_CYW43_ARCH_POLL
Cpl::System::Mutex Cpl::Io::Tcp::lwIP::Picow::g_internalLock;

#define PICO_CB_LOCK   g_internalLock.lock      
#define PICO_CB_UNLOCK g_internalLock.unlock

#else
#error MUST be compiled with: PICO_CYW43_ARCH_POLL=1 or PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1
#endif


//////////////////////////////////////////////////////////////////////////////
err_t Cpl::Io::Tcp::lwIP::Picow::lwipCb_dataSent_( void* arg, struct tcp_pcb* pcb, uint16_t len )
{
    // Not used/needed
    return ERR_OK;
}

err_t Cpl::Io::Tcp::lwIP::Picow::lwipCb_poll_( void* arg, struct tcp_pcb* pcb )
{
    Socket_T* fd = (Socket_T *) arg;
    err_t err = ERR_OK;
    PICO_CB_LOCK();

    // Flush any pending output
    if ( fd != nullptr && pcb != nullptr && fd->lwipPcb == pcb )
    {
        err = tcp_output( pcb );

    }

    PICO_CB_UNLOCK();
    return err;
}

err_t Cpl::Io::Tcp::lwIP::Picow::lwipCb_dataReceived_( void* arg, struct tcp_pcb* pcb, struct pbuf* pbuf, err_t err )
{
    Socket_T* fd     = (Socket_T *) arg;
    err_t     result = ERR_OK;
    PICO_CB_LOCK();

    // Fail if no valid upper layer 
    if ( fd == nullptr )
    {
        tcp_abort( pcb );
        result = ERR_ABRT;
    }

    // Trap additional failures
    else if ( pcb == nullptr || err != ERR_OK || fd->lwipPcb != pcb )
    {
        if ( fd->recvPbuf )
        {
            pbuf_free( fd->recvPbuf );
        }
        if ( pbuf != nullptr )
        {
            pbuf_free( pbuf );
        }

        fd->recvPbuf = nullptr;
        fd->lwipPcb  = nullptr;
        tcp_abort( pcb );
        result = ERR_ABRT;
    }

    // If we haven't processed the last PBUF - add to the current pbuf
    else if ( pbuf )
    {
        if ( fd->recvPbuf != nullptr )
        {
            // Account for overflowing the max pbuf size
            uint32_t newLen = fd->recvPbuf->tot_len + pbuf->tot_len;
            if ( newLen > 0xFFFF )
            {
                result = ERR_WOULDBLOCK;
            }
            else
            {
                // Append the new data to my current pbuf
                pbuf_cat( fd->recvPbuf, pbuf );
            }
        }

        // Cache the incoming PBUF/receive data so the next read call will process it
        else
        {
            fd->recvPbuf = pbuf;
        }
    }

    PICO_CB_UNLOCK();
    return result;
}

void Cpl::Io::Tcp::lwIP::Picow::lwipCb_error_( void* arg, err_t err )
{
    Socket_T* fd = (Socket_T *) arg;
    PICO_CB_LOCK();

    if ( fd != nullptr )
    {
        // Clear my socket info immediately to prevent possible recursive-error-death-loop
        tcp_pcb*                pcb       = fd->lwipPcb;
        pbuf*                   pbuf      = fd->recvPbuf;
        AsyncConnector::Client* clientPtr = (AsyncConnector::Client*) fd->connnectorClientPtr;
        memset( fd, 0, sizeof( Socket_T ) );

        if ( pcb )
        {
            // Check for connector rejected by the remote host
            if ( clientPtr != nullptr )
            {
                clientPtr->connectionFailed( Cpl::Io::Tcp::AsyncConnector::Client::eREFUSED );
            }

            // Make sure any unprocessed PBUF gets freed
            if ( pbuf )
            {
                pbuf_free( pbuf );
            }

            // close the socket
            if ( err != ERR_ABRT )
            {
                tcp_close( pcb );
            }
        }
    }

    PICO_CB_UNLOCK();
}

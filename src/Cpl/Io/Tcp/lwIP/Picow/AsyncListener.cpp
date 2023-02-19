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
/** @file */

#include "AsyncListener.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include "pico/cyw43_arch.h"

#define SECT_ "Cpl::Io::Tcp::lwIP::Picow"

using namespace Cpl::Io::Tcp::lwIP::Picow;

#ifdef PICO_CYW43_ARCH_THREADSAFE_BACKGROUND
#define PICO_LOCK       cyw43_arch_lwip_begin      
#define PICO_UNLOCK     cyw43_arch_lwip_end
#define PICO_CB_LOCK()
#define PICO_CB_UNLOCK()

#elif PICO_CYW43_ARCH_POLL
#define PICO_LOCK       g_internalLock.lock      
#define PICO_UNLOCK     g_internalLock.unlock
#define PICO_CB_LOCK    g_internalLock.lock    
#define PICO_CB_UNLOCK  g_internalLock.unlock

#else
#error MUST be compiled with: PICO_CYW43_ARCH_POLL=1 or PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1
#endif


///////////////////////////////////////////////////
AsyncListener::AsyncListener()
    : m_listenerPcb( nullptr )
    , m_clientPtr( nullptr )
{
}

AsyncListener::~AsyncListener()
{
    terminate();
}

///////////////////////////////////////////////////
bool AsyncListener::startListening( Client& client,
                                    int     portNumToListenOn ) noexcept
{
    bool result = false;
    PICO_LOCK();
    
    // Fail if already 'started'
    if ( m_clientPtr == nullptr )
    {
        // Housekeeping
        m_clientPtr = &client;

        // Create the PCB
        struct tcp_pcb* pcb = tcp_new_ip_type( IPADDR_TYPE_ANY );
        if ( !pcb )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("AsyncListener: failed to create listen pcb") );
        }
        
        // PCB created
        else
        {
            // Bind the socket
            err_t err = tcp_bind( pcb, NULL, portNumToListenOn );
            if ( err )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("AsyncListener: failed to bind to port %d (err=%d)", portNumToListenOn, err) );
            }
            
            // The bind was successful
            else
            {
                // Start listening
                m_listenerPcb = tcp_listen_with_backlog( pcb, 1 );
                if ( m_listenerPcb != nullptr )
                {
                    // Set the callback function for accept function
                    tcp_arg( m_listenerPcb, this );
                    tcp_accept( m_listenerPcb, lwIPCb_accept );
                    result = true;
                }
                
                // The listen call failed!
                else
                {
                    if ( pcb )
                    {
                        tcp_close( pcb );
                    }
                    CPL_SYSTEM_TRACE_MSG( SECT_, ("AsyncListener: failed to listen") );
                }
            }
        }
    }

    PICO_UNLOCK();
    return result;
}

err_t AsyncListener::lwIPCb_accept( void* arg, struct tcp_pcb* newpcb, err_t err )
{
    AsyncListener* theOne = (AsyncListener*) arg;
    err_t          result = ERR_OK;
    PICO_CB_LOCK();

    // Trap errors
    if ( theOne == nullptr || err != ERR_OK || newpcb == nullptr )
    {
        result = ERR_VAL;
    }

    // Reject the request if there is already an active connection
    else if ( theOne->m_connectionFd.lwipPcb != nullptr )
    {
        tcp_abort( newpcb );
        result = ERR_ABRT;
    }
    
    else
    {
        // Make sure that the client is in the closed state (i.e. handle the use case of reconnecting)
        theOne->m_clientPtr->close();

        // Set up the new 'file descriptor' for the connection
        memset( &(theOne->m_connectionFd), 0, sizeof( Socket_T ) );
        theOne->m_connectionFd.lwipPcb   = newpcb;
        Cpl::Io::Descriptor newfd        = (void*) &(theOne->m_connectionFd);

        // Set keep alive for the connection
        ip_set_option( newpcb, SOF_KEEPALIVE );

        // Set the callback function for the connection
        tcp_arg( newpcb, &(theOne->m_connectionFd) );
        tcp_sent( newpcb, lwipCb_dataSent_ );
        tcp_recv( newpcb, lwipCb_dataReceived_ );
        tcp_poll( newpcb, lwipCb_poll_, OPTION_CPL_IO_TCP_LWIP_PICOW_POLL_TICKS );
        tcp_err( newpcb, lwipCb_error_ );

        // Inform the client of new connection
        if ( !theOne->m_clientPtr->newConnection( newfd, ip4addr_ntoa( &(newpcb->remote_ip) ) ) )
        {
            // Housekeeping
            memset( &(theOne->m_connectionFd), 0, sizeof( Socket_T ) );

            // Client rejected the connection
            tcp_abort( newpcb );
            result = ERR_ABRT;
        }
    }

    PICO_CB_UNLOCK();
    return result;
}

void AsyncListener::poll() noexcept
{
    PICO_LOCK();
    cyw43_arch_poll();  // Note: this is only needed/meaningful when compiled with: PICO_CYW43_ARCH_POLL=1 
    PICO_UNLOCK();
}

void AsyncListener::terminate() noexcept
{
    PICO_LOCK();
    
    // Close listener PCB
    if ( m_listenerPcb != nullptr )
    {
        tcp_close( m_listenerPcb );
    }

    // Terminate the accepted connection 
    if ( m_clientPtr )
    {
        m_clientPtr->close();
    }
    
    // Clear my 'started state'
    m_clientPtr   = nullptr;
    m_listenerPcb = nullptr;
    memset( &m_connectionFd, 0, sizeof( Socket_T ) );

    PICO_UNLOCK();
}




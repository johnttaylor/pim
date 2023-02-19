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

#include "Driver/Wifi/Station/Connection.h"
#include "Cpl/System/Trace.h"
#include <stdint.h>
#include "pico/cyw43_arch.h"
#include "cyw43.h"
#include "Cpl/System/Mutex.h"

//
using namespace Driver::Wifi::Station;

#define SECT_ "Driver::Wifi::Station"


#ifdef PICO_CYW43_ARCH_THREADSAFE_BACKGROUND
static Cpl::System::Mutex guard_;
#define PICO_SCOPE_LOCK()   Cpl::System::Mutex::ScopeBlock lock(guard_)

#elif PICO_CYW43_ARCH_POLL
#define PICO_SCOPE_LOCK()      

#else
#error MUST be compiled with: PICO_CYW43_ARCH_POLL=1 or PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1
#endif


static uint32_t convertAuthMethodToCyw43Arch( Connection::Authentication_T genericAuthMethod )
{
    switch ( genericAuthMethod )
    {
    case Connection::eOPEN:
        return CYW43_AUTH_OPEN;
    case Connection::eWPA_TKIP_PSK:
        return CYW43_AUTH_WPA_TKIP_PSK;
    case Connection::eWPA2_AES_PSK:
        return CYW43_AUTH_WPA2_AES_PSK;
    case Connection::eWPA2_MIXED_PSK:
        return CYW43_AUTH_WPA2_MIXED_PSK;
    default:
        return CYW43_AUTH_OPEN; // If get here the authentication is not support - so we pick something that will fail
    }
}

static Connection::State_T convertCYW43ToStatus( int linkStatus )
{
    switch ( linkStatus )
    {
        case CYW43_LINK_DOWN:
            return Connection::eLINK_DOWN;
        case CYW43_LINK_JOIN:
            return Connection::eLINK_JOINED;
        case CYW43_LINK_NOIP:
            return Connection::eLINK_JOINED_NOIP;
        case CYW43_LINK_UP:
            return Connection::eLINK_UP;
        case CYW43_LINK_FAIL:
            return Connection::eLINK_FAILED;
        case CYW43_LINK_NONET:
            return Connection::eLINK_NO_NETWORK;
        case CYW43_LINK_BADAUTH:
            return Connection::eLINK_BAD_AUTHENTICATION;
        default:
            return Connection::eLINK_FAILED;    // This should happen -->so return the generic fail if it does!
    }
}

const char* Connection::toString( State_T linkStatus ) noexcept
{
    switch ( linkStatus )
    {
    case Connection::eLINK_DOWN:
        return "eLINK_DOWN";
    case Connection::eLINK_JOINED:
        return "eLINK_JOINED";
    case Connection::eLINK_JOINED_NOIP:
        return "eLINK_JOINED_NOIP";
    case Connection::eLINK_UP:
        return "eLINK_UP";
    case Connection::eLINK_FAILED:
        return "eLINK_FAILED";
    case Connection::eLINK_NO_NETWORK:
        return "eLINK_NO_NETWORK";
    case Connection::eLINK_BAD_AUTHENTICATION:
        return "eLINK_BAD_AUTHENTICATION";
    default:
        return "<unknown>";
    }
}

////////////////////

static int                            connectionState_;
static Connection::StateChangedFunc_T cbFunc_;

////////////////////
void Connection::initiailize() noexcept
{
    PICO_SCOPE_LOCK();

    // Note: ASSUMES that the CYW43 sub-system is initialized by the BSP and/or
    //       the application start-up code

    // Enable WIFI station mode
    cyw43_arch_enable_sta_mode();
}

bool Connection::start( const char*         ssid,
                        const char*         password,
                        Authentication_T    authenticationMethod,
                        StateChangedFunc_T  callbackFunc ) noexcept
{
    PICO_SCOPE_LOCK();

    connectionState_ = CYW43_LINK_DOWN;
    int err          = cyw43_arch_wifi_connect_async( ssid, password, convertAuthMethodToCyw43Arch( authenticationMethod ) );
    if ( err )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Wifi connect request failed: %d", err) );
        cbFunc_ = nullptr;
        return false;
    }
    cbFunc_ = callbackFunc;
    return true;
}

void Connection::poll() noexcept
{
    PICO_SCOPE_LOCK();

    // Give a timeslice to the CYW43 driver
    cyw43_arch_poll();

    // Query the current state
    int newStatus = cyw43_tcpip_link_status( &cyw43_state, CYW43_ITF_STA );
    if ( connectionState_ != newStatus )
    {
        // Notify the application of the new status (if a callback was specified)
        if ( cbFunc_ != nullptr )
        {
            (cbFunc_) (convertCYW43ToStatus( newStatus ));
        }
    }
    connectionState_  = newStatus;

}

bool Connection::isConnected() noexcept
{
    PICO_SCOPE_LOCK();

    int newStatus = cyw43_tcpip_link_status( &cyw43_state, CYW43_ITF_STA );
    return newStatus == CYW43_LINK_UP;
}

Connection::State_T Connection::getState() noexcept
{
    return convertCYW43ToStatus( cyw43_tcpip_link_status( &cyw43_state, CYW43_ITF_STA ) );
}

void Connection::stop() noexcept
{
    PICO_SCOPE_LOCK();

    cyw43_wifi_leave( &cyw43_state, CYW43_ITF_STA );
}

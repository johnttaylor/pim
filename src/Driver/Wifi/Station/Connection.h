#ifndef Driver_Wifi_Station_Connection_h_
#define Driver_Wifi_Station_Connection_h_
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

#include <stdint.h>
#include "colony_map.h"

///
namespace Driver {
///
namespace Wifi {
///
namespace Station {

/** This class defines an asynchronous interface for a Station device to
    connect to a WIFI network. 

    This interface does NOT define any thread safety semantics, i.e. the 
    application should assume that the interface is NOT thread safe.

    The interface uses polling semantics, i.e. the application is required to
    call the 'poll()' periodically.

    The class/interface is designed a 'Singleton', i.e. the application can only
    have one instance/implementation of the interface.
*/
class Connection
{
public:
    /// Connection state
    enum State_T
    {
        eLINK_DOWN,                 //!< The link is down
        eLINK_JOINED,               //!< Connected to wifi
        eLINK_JOINED_NOIP,          //!< Connected to wifi, but no IP address
        eLINK_UP,                   //!< Connect to wifi with an IP address
        eLINK_FAILED,               //!< Connection failed
        eLINK_NO_NETWORK,           //!< No matching SSID found (could be out of range, or down)
        eLINK_BAD_AUTHENTICATION    //!< Authentication failure
    };

    /// Authorization options
    enum Authentication_T
    {
        eOPEN = 0,                  //!< No authorization required 
        eWPA_TKIP_PSK,              //!< WPA authorization
        eWPA2_AES_PSK,              //!< WPA2 authorization (preferred)
        eWPA2_MIXED_PSK             //!< WPA2/WPA mixed authorization
    };

public:
    /// Defines the function signature for callbacks
    typedef void (*StateChangedFunc_T)(State_T currentState);

public:
    /** This method is used to initialize the WIFI engine in Station Mode.  It 
        should only be called ONCE on startup before any other methods in this 
        interface is called. 

        This method ASSUMES that the hardware, WIFI stack, etc. has ALREADY 
        been initialized and is available for use.
     */
    static void initiailize() noexcept;

    /** This method is used to initiate connecting to a WIFI network.

        The method will optionally generate callbacks to the application when
        the state of the connection changes. Note: Callbacks for EVERY transition
        is NOT guaranteed. What is guaranteed is that latest state change when 
        the poll() method is called will trigger a callback

        The 'ssid' and 'password' arguments MUST stay in scope until stop() is
        called (i.e. the application is responsible for the memory of these
        arguments).
     */
    static bool start( const char*         ssid,
                       const char*         password,
                       Authentication_T    authenticationMethod,
                       StateChangedFunc_T  callbackFunc = nullptr ) noexcept;

    /** This method is used to monitor the current state of the WIFI connection
        and attempts to re-establish the connect if it drops.  The application
        is responsible for calling this method periodically.
     */
    static void poll() noexcept;

    /** This method returns true if the state of the WIFI connection is in
        the eLINK_UP state.
     */
    static bool isConnected() noexcept;

    /** This method returns true the current state of the WIFI connection
     */
    static State_T getState() noexcept;

    /** This method disconnects the device from the from the WIFI network. The
        Application must call start() again to reconnect to a/the WIFI network
     */
    static void stop() noexcept;

    /// Convience method that convert the binary State_T enum to string
    static const char* toString( State_T linkStatus ) noexcept;
};



/*--------------------------------------------------------------------------*/
}       // End namespace(s)
}
}
#endif  // end header latch

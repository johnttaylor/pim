#ifndef Cpl_System_SharedEventHandler_h_
#define Cpl_System_SharedEventHandler_h_
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


#include "Cpl/System/EventFlag.h"


 ///
namespace Cpl {
///
namespace System {


/** This abstract class defines the interface for a Shared Event Handler.
    A Shared Event Handler is used to have function table instead of a hard
    coded switch() statement for handling/processing event flags notification.
 */
class SharedEventHandlerApi
{
public:
    /// Definition of event flag callback functions
    typedef void (*EventCallbackFunc_T)( void* context );

    /** Define a callback entry (i.e. provide a 'context' pointer for
        the callback
     */
    typedef struct
    {
        EventCallbackFunc_T callbackFunc;   //!< Callback function pointer
        void*               context;        //!< Context for the callback.  The callback function is required to 'understand' the actual type of the context pointer being passed to it.
    } EventCallback_T;

public:
    /** This method 'dispatches' the processing associated with the specified
        Event based the list/array of callback functions.
     */
    virtual void processEventFlag( uint8_t eventNumber ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~SharedEventHandlerApi() {}
};



/** This template concrete class is a 'strategy class' in that in provides most
    of work needed for a Runnable object to support a list of callback functions
    for handling event-signally - instead of hard coded switch statement
    in its processEventFlag() method.

    Template Arg: N - the max number of callbacks supported.
 */
template <int N>
class SharedEventHandler: public SharedEventHandlerApi
{

public:
    /** Constructor. The 'myCallbacks' argument is an an array of callback
        entries pointers.  The order of array maps directly to the Event flag
        number, i.e. index 0 is the callback function for Event Flag 0.  If no
        callback is supported for a particular Event Fla/Index use a zero (or
        nullptr) for the .callbackFunc field/value for the array entry.
     */
    SharedEventHandler( EventCallback_T( &myCallbacks )[N] )
        :m_callbacks( myCallbacks )
    {
    }


public:
    /** This method 'dispatches' the processing associated with the specified
        Event based the list/array of callback functions.
     */
    void processEventFlag( uint8_t eventNumber ) noexcept
    {
        if ( eventNumber >= N || m_callbacks[eventNumber].callbackFunc == 0 )
        {
            return;
        }

        ( m_callbacks[eventNumber].callbackFunc )( m_callbacks[eventNumber].context );
    }

protected:
    /// Reference to my callback functions
    EventCallback_T( &m_callbacks )[N];
};

};      // end namespaces
};
#endif  // end header latch

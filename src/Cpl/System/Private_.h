#ifndef Cpl_System_Private_x_h__
#define Cpl_System_Private_x_h__
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
/** @file

    This file contains a collection of classes, interfaces, etc. that are
    are intended to be USE ONLY by Colony.Core sub-system - NOT by clients,
    consumers of the Colony.Core package.
*/

#include "Cpl/Container/Item.h"
#include "Cpl/Text/String.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/Trace.h"


/// 
namespace Cpl {
/// 
namespace System {


/** This class provide access to Colony.Core's internal global mutexes.
 */
class Locks_
{
public:
    /// Global mutex for use for interfaces/class in the Cpl::System namespace
    static Mutex& system( void );

    /// Same as 'system', except usage is for 'long' critical sections (i.e. walking lists, printing, etc.)
    static Mutex& sysLists( void );

    /** Global mutex for use for tracing sub-system.  This mutex is to protect 
        the Trace engine's internal/meta data.  This mutex is NOT used to guard
        the Trace output.  The Trace engine uses two mutex so that its output
        can be 'serialized' with the TShell's output (or other sub-systems) BUT
        not cause high priority threads to block on the trace output when their
        trace statements are not enabled at run time.
     */
    static Mutex& tracing( void );

    /// Global mutex for use for the tracing sub-system OUTPUT (see above for more details)
    static Mutex& tracingOutput( void );
};



/** This class defines a start-up handler/hook that allows a Colony.Core
    sub-system to register to be called when the Api::initialize()
    method is called.

    The intended usage of this interface is that a sub-system creates
    a sub-class of this class and then creates a static instance of
    its sub-class.  During the init process, the instance's 'notifyInit'
    method will be called.

    Note: This class/interface SHOULD ONLY be used by Colony.Core
          interfaces - the application should NEVER register a
          startup hook!
 */
class StartupHook_ : public Cpl::Container::Item
{
public:
    /** This enum defines 'init_levels'.  The init levels are processed
        in order (lowest value to highest), i.e. callbacks for lowest
        numbered init level are called first and the callback for highest
        numbered init level are called last.  Within a init level there is
        no guaranteed order to the callbacks.
     */
    enum InitLevel_T
    {
        eTEST_INFRA=0,    /// Initialized first
        eSYSTEM,
        eMIDDLE_WARE,
        eAPPLICATION
    }; /// Initialized last


public:
    /** This method is called as part of the Colony.Core initialize()
        process. The 'init_level' informs the client what initialize
        level context the notifyInit() method is being called.
     */
    virtual void notify( InitLevel_T init_level ) = 0;


protected:
    /** Base Class constructor -->performs the callback/init-level
        registration
     */
    StartupHook_( InitLevel_T myInitLevel );

public:
    /// Ensure the destructor is virtual
    virtual ~StartupHook_() {}


public:
    /** This is used to register instance of this class.  This method is
        NOT implemented by the 'client sub-system' - it is a singleton that
        is implemented Cpl::System::Startup classes.

        NOTE: This is the ONE Colony.Core method that can be called BEFORE
              Cpl::System::Api::initialize() is called.
     */
    static void registerHook( StartupHook_& callbackInstance, InitLevel_T initOrder );

    /** This method is intended to be USED ONLY by the Cpl::System::Api::init()
        method to trigger all of the registered init callbacks.
     */
    static void notifyStartupClients( void );
};


/** This class defines the interface to the platform specific implementation of the
    Cpl::System::Trace interface
 */
class TracePlatform_
{
public:
    /** This method append the 'info' information to the provide String for the
        specified infoLevel.
     */
    static void appendInfo( Cpl::Text::String& dst, Trace::InfoLevel_T info, const char* section, const char* filename, int linenum, const char* funcname );

    /** This method output the string to the logging destination/stream/media
     */
    static void output( Cpl::Text::String& src );
};



};      // end namespaces
};
#endif  // end header latch


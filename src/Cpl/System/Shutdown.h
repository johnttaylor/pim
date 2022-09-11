#ifndef Cpl_System_Shutdown_h_
#define Cpl_System_Shutdown_h_
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


#include "colony_config.h"
#include "Cpl/Container/Item.h"


/** Specifies the default value used for the application exit code when
    terminating 'successfully'
 */
#ifndef OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE
#define OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE       0
#endif


 /** Specifies the default value used for the application exit code when
       terminating 'on a failure'
  */
#ifndef OPTION_CPL_SYSTEM_SHUTDOWN_FAILURE_ERROR_CODE
#define OPTION_CPL_SYSTEM_SHUTDOWN_FAILURE_ERROR_CODE       1
#endif


  /// 
namespace Cpl {
/// 
namespace System {


/** This class defines methods for forcibly terminating the application.
    How gracefully (or not) the shutdown is dependent on the actual platform
    implementation. In addition, what 'shutdown' means is also platform
    dependent, e.g. on an embedded platform it may invoke a reboot and/or
    restart of the application.

    Since this is a forcibly shutdown there are no guaranties with respect to
    releasing resources.

    The interface does provide for callback method(s) to be called on shutdown.
    These methods are guaranteed to be called (or at least attempted to be
    called in the case of true code failure).
 */
class Shutdown
{
public:
    /** This call defines the callback interface that is used when the
        application is shutdown
     */
    class Handler : public Cpl::Container::Item
    {
    public:
        /** This method is called when the application is shutting down. The
            method is passed 'exit_code' which is the exit code provided by the
            application when called the Shutdown interface.  The return value
            from the handler will be used as the application shutdown exit code.
            To leave the exit code unaltered - return the passed in 'exit_code'.

            Notes:

                o The final exit code is the serialized 'sum' of the all of the
                  shutdown handlers.
                o The notify() method is called in the thread context that
                  initiated the shutdown.
         */
        virtual int notify( int exit_code ) = 0;

    public:
        /// Ensure the destructor is virtual
        virtual ~Handler() {}

    };


public:
    /** This function will force a shutdown of the application with a
        'success' exit code.  What 'forced' means is dependent on the
        actual platform.  All registered callback methods will be called
        before exiting the application.
     */
    static int success( void );

    /** This function will force a shutdown of the application with a 'failure'
        exit code.  The caller can optional specify an exit code. What 'forced'
        means is dependent on the actual platform.  All registered callback
        methods will be called before exiting the application. The returned
        value - assuming the method actually returns - will be the 'final'
        exit code (the passed in exit_code can be alter by registered Shutdown
        Handlers).

        Note: The recommended approach for exiting the application due to an
              error is to use the Cpl::System::FatalError interface so that
              the 'why' of the failure has the potential for being captured.
     */
    static int failure( int exit_code = OPTION_CPL_SYSTEM_SHUTDOWN_FAILURE_ERROR_CODE );


public:
    /** This method is used to register a callback method that will be called
        when success() or failure() is called and before the application exits.
        There is NO guaranteed order on when the register callback methods
        (when there is multiple register callbacks) will be called OR with
        respect to application code running. The only guarantee is that the
        callbacks will be called AFTER the above methods are called and BEFORE
        the application shutdown.

        During shutdown, the 'notify()' method of 'instanceToRegister'
        is called.
     */
    static void registerHandler( Shutdown::Handler& instanceToRegister );


public:
    /** This COMPONENT Scoped method.  The application SHOULD not directly
        access this method (unless you are REALLY REALLY REALLY sure you
        know what you are doing).
        
        Helper method used to notify registered shutdown handlers
     */
    static int notifyShutdownHandlers_( int exit_code );

};


};      // end namespaces
};
#endif  // end header latch


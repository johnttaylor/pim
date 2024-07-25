#ifndef Cpl_MApp_MAppApi_h_
#define Cpl_MApp_MAppApi_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "colony_config.h"
#include "Cpl/Container/MapItem.h"


/// CPL Trace Section identifier for a common trace output section
#ifndef OPTION_CPL_MAPP_TRACE_SECTION
#define OPTION_CPL_MAPP_TRACE_SECTION   "MApp"
#endif

///
namespace Cpl {
///
namespace MApp {


/** This abstract class defines the interface for a Micro Application (MApp).  
    A MApp is essentially a 'mini-application' that performs a series of steps 
    and typically reports its output using the CPL_SYSTEM_TRACE_MSG() method.

    Multiple MApp instances can be executing at the same.  However, all MApps 
    execute in a single/same thread.

    The following are the 'thread safety' requirements for a MApp:
        o Data can safely be exchanged via Model Points
        o The application can safely pass initial settings, configuration, 
          options when the MApp is started
 */
class MAppApi : public Cpl::Container::MapItem
{
public:
    /** This method returns the MApp name.  The MApp name must an printable
        ASCII string with NO whitespace.  The name is used to uniquely
        identify the individual MApp
     */
    virtual const char* getName() const noexcept = 0;

    /** This method returns brief description/summary of what the MApp does.  
        Typically this information will be displayed on the Debug console to 
        aid the user on how to execute the MApp.  The content/verbosity of the 
        text is MApp specific. 
     */
    virtual const char* getDescription() const noexcept = 0;

    /** This method returns help for the MApp (optional) command line arguments.
        Typically this information will be displayed on the Debug console to 
        aid the user on how to execute the MApp.  The content/verbosity of the 
        text is MApp specific. 
     */
    virtual const char* getUsage() const noexcept = 0;

public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::MApp namespace.  The Application should
        NEVER call this method.

        This method is used to perform any needed initialization on start-up of
        Application. This method is called when the MApp Manager is opened.

        This method MUST be called in the thread that the MApp executes in.
     */
    virtual void intialize_() noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::MApp namespace.  The Application should
        NEVER call this method.

        This method is used to perform any needed shutdown when the application
        is shutdown.  This method is called when the MApp Manager is closed.

        This method MUST be called in the thread that the MApp executes in.
     */
    virtual void shutdown_() noexcept = 0;


public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::MApp namespace.  The Application should
        NEVER call this method, instead the Application must call the Manager
        to start a MApp.
        
        If the client has no 'optionalArgs' to pass to the MApp instance - it must
        provide an empty/blank null terminated string, i.e can NOT pass a nullptr.

        This method is used to start a MApp. If the MApp is unable to start then
        false is returned; else true is returned.

        This method MUST be called in the thread that the MApp executes in.

        Note: The MApp is allowed to perform destructive parsing on 'optionalArgs',
              however it must honor the string len of 'optionalArgs'
     */
    virtual bool start_( char* optionalArgs ) noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::MApp namespace.  The Application should
        NEVER call this method, instead the Application must call the Manager
        to stop a MApp.
        
        This method is used to stop a MApp.
     */
    virtual void stop_() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~MAppApi() {}
};

};      // end namespaces
};
#endif  // end header latch

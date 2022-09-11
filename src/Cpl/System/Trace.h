#ifndef Cpl_System_Trace_h_
#define Cpl_System_Trace_h_
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
#include "Cpl/Text/String.h"
#include "Cpl/System/Thread.h" 
#include "Cpl/Io/Output.h"


/// 
namespace Cpl {
/// 
namespace System {

/** This concrete class provide a 'printf' tracing mechanism. The output of
    tracing messages typically defaults (dependent on the target platform)
    to stdout.  However, the output cat be redirected to any
    Cpl::Stream::Output instance.

    The trace engine is thread safe, i.e. it guarantees that outputting a single
    trace message (info + user msg) is an Atomic operation within a multi-thread
    environment.

    All of the `methods SHOULD be access via the preprocessor macros below to
    allow the 'trace code' to be compiled out of a 'release' build if desired.
    In addition to a compile time decision to use/exclude tracing, there are
    run-time switches to selective enable and/or control what is displayed.

    There are numerous application customizable (at compile time) options
    for how the internals of how the trace engine works (see OPTION_xxx
    macros below).

    NOTE: The trace engine supports does NOT support tracing BEFORE the Colony
          library is initialized.  This means that constructors of objects that
          are created statically can NOT contain trace logging calls.

    <pre>

    For trace message to appear on the logging media the following
    must done:

        1. Enable Trace at compile time by defining the USE_CPL_SYSTEM_TRACE
           preprocessor symbol.
        2. Enable the Trace engine at run time by calling CPL_SYSTEM_TRACE_ENABLE()
        3. Enable at least one 'section' by calling CPL_SYSTEM_TRACE_ENABLE_SECTION()



    How to use the CPL_SYSTEM_TRACE_xxx macros
    ------------------------------------------
    The trace functionality is provided by the CPL_SYSTEM_TRACE and
    CPL_SYSTEM_TRACEMSG macros.  The remaining macros are used to enable/disable
    and control the actual trace output.  If the symbol, USE_CPL_SYSTEM_TRACE,
    is NOT defined at compile time -- all of the trace macros are compiled
    out.  If the symbol is defined, then the trace output can be enabled
    and disabled at run time.

    CPL_SYSTEM_TRACE()    - This macro is used to trace the entry and exit of a
                            scope block (a scope block is a pair of '{}').
                            For example:
                                void foobar( int x )
                                    {
                                    // Traces the entry & exit of foobar(), when
                                    // tracing is enabled for section "bob"
                                    CPL_SYSTEM_TRACE("bob");
                                    int y = 2*x;
                                    ...
                                    }

    CPL_SYSTEM_TRACEMSG() - This macro is for the programmer/application to
                            generate formatted message that are outputted to the
                            "trace media." The macro has printf() semantics.
                            The problem is that ANSI C++03 does not directly
                            support var_args in a macro - so you HAVE TO USE AN
                            EXTRA SET OF '()'.

                            For example:
                                void foobar( int x )
                                    {
                                    // Traces the entry & exit of foobar(), when
                                    // tracing is enabled for section "bob"
                                    CPL_SYSTEM_TRACE("bob");
                                    int y = 2*x;
                                    ...
                                    // Trace message outputs when tracing is
                                    // enabled for section "bob"
                                    CPL_SYSTEM_TRACEMSG( "bob, ("This is an example Trace Msg. Y=%d, X=d", y, x) );
                                    ...
                                    }

</pre> */
class Trace
{
public:
    /// Constructor
    Trace( const char* filename, int linenum, const char* funcname, const char* section, const char* scope_name );

    /// Destructor
    ~Trace();

public:
    /** This enum is used to globally control the amount of 'Prologue' information
        that is prepended to the Trace log messages.  The specifics of what
        information is added is platform specific.
     */
    enum InfoLevel_T
    {
        eNONE=0,         //!< Turns off ALL prologue info
        eBRIEF,          //!< Default setting 
        eINFO,           //!< Additional prologue info
        eVERBOSE,        //!< more prologue info  
        eMAX             //!< Maximum prologue info
    };

public:
    /** This function is used to trace the 'location' for general purpose trace
        messages.

        NOTE: NEVER call this method directly
     */
    static void traceLocation_( const char* section, const char* filename, int linenum, const char* funcname );

    /** This function is used to generate general purpose trace messages.

        NOTE: NEVER call this method directly
     */
    static void traceUserMsg_( const char* format, ... );

public:
    /** This method enables the output/logging of trace message at run-time.
        The default is start with tracing enabled.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static void enable_( void );

    /** This method disables the output/logging of trace message at run-time.
        The default is start with tracing enabled.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static void disable_( void );

    /** Returns true if tracing is enabled, i.e. enable_() has been called.,

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static bool isEnabled_( void );

    /** This method sets the information level for the trace messages. The method
        returns the previous level setting.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static InfoLevel_T setInfoLevel_( InfoLevel_T newLevel );

    /** This method returns the current information level setting

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static InfoLevel_T getInfoLevel_( void ) noexcept;

    /** This method enables the output/logging of trace message at run-time for
        the specified 'section'.  A 'section' name can NOT contain any white
        space.  The method returns true if the section was enabled; else false
        is return, i.e. exceeded max active enabled sections.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static bool enableSection_( const char* sectionToEnable );

    /** This method disables the output/logging of trace message at run-time for
        the specified 'section'.  Setting  'sectionToDisable' to 0 will disable
        all sections.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static void disableSection_( const char* sectionToDisable );

    /** Returns true if tracing is enabled AND the specified 'section' has been
        enabled.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static bool isSectionEnabled_( const char* section );

    /** This method returns the number of enabled 'sections' and returns the
        actual section name(s) via the String 'dst'.  It is the caller
        responsibility to ensure that 'dst' is large enough to hold all of the
        enable sections.  If 'dst' is to short, the results will be truncated.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static unsigned getSections_( Cpl::Text::String& dst );

    /** This method applies a filter by thread name when generating output, i.e.
        for a message to be output, trace must be enabled, the message's section
        enabled, and must be running in the one of the threads specified
        by the thread filter.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static void setThreadFilter_( const char* threadName1,
                                  const char* threadName2 =0,
                                  const char* threadName3 =0,
                                  const char* threadName4 =0
                                );

    /** This method returns the number of enabled 'thread filters' and returns
        the actual thread name(s) via the String 'dst'.  It is the caller
        responsibility to ensure that 'dst' is large enough to hold all of the
        enable thread filters.  If 'dst' is to short, the results will be
        truncated.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static unsigned getThreadFilters_( Cpl::Text::String& dst );

    /** This method removes  that thread filter (if there is one present).  If
        no filter has been applied when this method is called, then this
        method does nothing.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static void clearThreadFilter_( void );

    /** This method returns true if the thread filter has been set and the
        specified name 'passes' the thread filter check, i.e. the specified
        name match one of the names in the filter list.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static bool passedThreadFilter_( const char* threadNameToTest = Cpl::System::Thread::myName() );


public:
    /** This method is used to redirect the trace output to the provided Output
        stream. There is no guaranty on what happens to trace message(s) during
        the transition to the new output/media destination.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static void redirect_( Cpl::Io::Output& newMedia );

    /** This method is used to revert the trace output to its default output
        destination/stream/media. There is no guaranty on what happens to trace
        message(s) during the transition to the default output/media
        destination.

        NOTE: NEVER call this method directly -->use the CPL_SYSTEM_TRACE_xxx()
              macros.
     */
    static void revert_( void );

public:
    /** This COMPONENT Scoped method provides the Output stream to the Trace
        Engine.  The Output stream instance MUST be created statically. However,
        the Output stream does not necessarily need to fully 'operational' when
        statically constructed - it just need to be  "operationally" by the time
        Cpl::System::Api::initialize() is called.

        NOTE: The Application is RESPONSIBLE for implementing this method!
     */
    static Cpl::Io::Output* getDefaultOutputStream_( void ) noexcept;


protected:
    /// Caches trace info for exit message
    const char* m_filename;

    /// Caches trace info for exit message
    int         m_linenum;

    /// Caches trace info for exit message
    const char* m_funcname;

    /// Caches trace info for exit message
    const char* m_section;

    /// Caches trace info for exit message
    const char* m_scope;
};

};
};      // end namespaces

//////////////////////////////////////////////////////////////////////////////
#ifdef USE_CPL_SYSTEM_TRACE


// PRETTY_FUNCTION macro is non-standard
#if defined(__GNUC__)
/// Take advantage of GCC's pretty function symbol
#define CPL_SYSTEM_TRACE_PRETTY_FUNCNAME    __PRETTY_FUNCTION__

#elif defined(_MSC_VER)
/// Take advantage of Microsoft's pretty function symbol
#define CPL_SYSTEM_TRACE_PRETTY_FUNCNAME    __FUNCSIG__

#else
/// Use C++11 function name

#define CPL_SYSTEM_TRACE_PRETTY_FUNCNAME    __func__
#endif  // end __PRETTY_FUNCTION__


    /// Macro Wrapper
#define CPL_SYSTEM_TRACE_FUNC(sect)                Cpl::System::Trace cplSystemTraceInstance_ (__FILE__,__LINE__,CPL_SYSTEM_TRACE_PRETTY_FUNCNAME,sect,CPL_SYSTEM_TRACE_PRETTY_FUNCNAME)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_SCOPE(sect,label)         Cpl::System::Trace cplSystemTraceInstance_ (__FILE__,__LINE__,CPL_SYSTEM_TRACE_PRETTY_FUNCNAME,sect,label)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_MSG(sect, var_args)        do { if ( Cpl::System::Trace::isSectionEnabled_(sect) && Cpl::System::Trace::passedThreadFilter_() ) {Cpl::System::Trace::traceLocation_(sect,__FILE__,__LINE__,CPL_SYSTEM_TRACE_PRETTY_FUNCNAME);Cpl::System::Trace::traceUserMsg_ var_args;}} while(0)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_ENABLE()                   Cpl::System::Trace::enable_()

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_DISABLE()                  Cpl::System::Trace::disable_()

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_IS_ENABLED()			    Cpl::System::Trace::isEnabled_()

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_IS_SECTION_ENABLED(sect)   Cpl::System::Trace::isSectionEnabled(sect)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_SET_INFO_LEVEL(l)          Cpl::System::Trace::setInfoLevel_(l)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_GET_INFO_LEVEL()           Cpl::System::Trace::getInfoLevel_()

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_ENABLE_SECTION(sect)       Cpl::System::Trace::enableSection_(sect)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_DISABLE_SECTION(sect)      Cpl::System::Trace::disableSection_(sect)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_GET_SECTIONS(dst)          Cpl::System::Trace::getSections_(dst)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_SET_THREAD_FILTER(t1)              Cpl::System::Trace::setThreadFilter_(t1)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_SET_THREAD_2FILTERS(t1,t2)         Cpl::System::Trace::setThreadFilter_(t1,t2)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_SET_THREAD_3FILTERS(t1,t2,t3)      Cpl::System::Trace::setThreadFilter_(t1,t2,t3)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_SET_THREAD_4FILTERS(t1,t2,t3,t4)   Cpl::System::Trace::setThreadFilter_(t1,t2,t3,t4)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_GET_THREAD_FILTERS(dst)            Cpl::System::Trace::getThreadFilters_(dst)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_CLEAR_THREAD_FILTER()              Cpl::System::Trace::clearThreadFilter_()

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_PASSED_THREAD_FILTER(tname)        Cpl::System::Trace::passedThreadFilter_( tname )

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_DISABLE_SECTION(sect)              Cpl::System::Trace::disableSection_(sect)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_REDIRECT(newdst)                   Cpl::System::Trace::redirect_(newdst)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_REVERT()                           Cpl::System::Trace::revert_()

/// Returns true if TRACE was activated/enabled at compile time
#define CPL_SYSTEM_TRACE_IS_COMPILED()                      true

/// Allocate a variable that is only 'used' when tracing is enabled
#define CPL_SYSTEM_TRACE_ALLOCATE(type,varname,initval)    type varname = initval


#else
    /// Macro Wrapper 
#define CPL_SYSTEM_TRACE_FUNC(sect)

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_SCOPE(sect,label)                      

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_MSG(sect, var_args)         

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_ENABLE()                   

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_DISABLE()                  

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_IS_ENABLED()			    false

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_IS_SECTION_ENABLED(sect)   false

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_SET_INFO_LEVEL(l)          

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_GET_INFO_LEVEL()           ((Cpl::System::Trace::InfoLevel_T)0)

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_ENABLE_SECTION(sect)       

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_DISABLE_SECTION(sect)      

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_SET_THREAD_FILTER(t1)           

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_SET_THREAD_2FILTERS(t1,t2)      

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_SET_THREAD_3FILTERS(t1,t2,t3)   

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_SET_THREAD_4FILTERS(t1,t2,t3,t4)

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_GET_THREAD_FILTERS(dst)        0

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_CLEAR_THREAD_FILTER()        

/// Macro Wrapper
#define CPL_SYSTEM_TRACE_PASSED_THREAD_FILTER(tname)    false

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_GET_SECTIONS(dst)          	0  

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_REDIRECT(newdst)           

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_REVERT()                   

/// Returns false if TRACE was NOT activated/enabled at compile time
#define CPL_SYSTEM_TRACE_IS_COMPILED()                  false

/// Macro Wrapper 
#define CPL_SYSTEM_TRACE_ALLOCATE(type,varname,initval)                

#endif  // USE_CPL_SYSTEM_TRACE


//////////////////////////////////////////////////////////////////////////////
/** The default maximum number of actively enabled 'sections' supported.
 */
#ifndef OPTION_CPL_SYSTEM_TRACE_MAX_SECTIONS
#define OPTION_CPL_SYSTEM_TRACE_MAX_SECTIONS            8
#endif

 /** The default maximum size, in bytes, for a section name (not including
     the null terminator).
  */
#ifndef OPTION_CPL_SYSTEM_TRACE_MAX_SECTION_NAME_LEN   
#define OPTION_CPL_SYSTEM_TRACE_MAX_SECTION_NAME_LEN    63
#endif

  /** The size, in bytes (without the null terminator), of trace engine's internal
      String use to construct a complete (info + user msg) trace message.
   */
#ifndef OPTION_CPL_SYSTEM_TRACE_MAX_BUFFER
#define OPTION_CPL_SYSTEM_TRACE_MAX_BUFFER              511
#endif


   /** String literal that is the prefix for all trace messages
    */
#ifndef OPTION_CPL_SYSTEM_TRACE_PREFIX_STRING
#define OPTION_CPL_SYSTEM_TRACE_PREFIX_STRING           ">> "
#endif


    /** String literal that is the final suffix for all trace messages
     */
#ifndef OPTION_CPL_SYSTEM_TRACE_SUFFIX_STRING
#define OPTION_CPL_SYSTEM_TRACE_SUFFIX_STRING           "\n"
#endif

     /** The compile time default setting for the info level
      */
#ifndef OPTION_CPL_SYSTEM_TRACE_DEFAULT_INFO_LEVEL
#define OPTION_CPL_SYSTEM_TRACE_DEFAULT_INFO_LEVEL      Cpl::System::Trace::eBRIEF
#endif

      /** The compile time default setting what enable/disable state the trace
          starts in
       */
#ifndef OPTION_CPL_SYSTEM_TRACE_DEFAULT_ENABLE_STATE
#define OPTION_CPL_SYSTEM_TRACE_DEFAULT_ENABLE_STATE    true
#endif


#endif  // end header latch


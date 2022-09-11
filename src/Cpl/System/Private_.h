#ifndef Cpl_System_Private_x_h__
#define Cpl_System_Private_x_h__
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
/** @file

    This file contains a collection of classes, interfaces, etc. that are
    are intended to be USE ONLY by Colony.Core sub-system - NOT by clients,
    consumers of the Colony.Core package.
*/

#include "Cpl/Text/String.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/PrivateStartup_.h"     // Include hered to maintain backwards compatibility with older projects


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


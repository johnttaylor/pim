#ifndef Cpl_System_Tls_h_
#define Cpl_System_Tls_h_
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
#include "colony_map.h"


/// Defer the definition of the a raw TLS key/index type to the application's 'platform'
#define Cpl_System_TlsKey_T             Cpl_System_TlsKey_T_MAP


/** This constant defines the DESIRED minimum of number of TLS instances/index.
    However, this is only a 'suggestion' to the underlying platform
    implementation, i.e. Platforms are NOT required to honor the specified
    minimum
 */
#ifndef OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES
#define OPTION_CPL_SYSTEM_TLS_DESIRED_MIN_INDEXES       8
#endif


 ///
namespace Cpl {
///
namespace System {

/** This concrete class defines the interface for Thread Local Storage (TLS).
    TLS provides a mechanism for each thread to have its own instance of a
    global variable.  A canonical example of TLS is the C error code variable
    'errno'.

    NOTES:

        o The initial contents (per thread) of a TLS variable will be zero.
        o If a new TLS variable/index can NOT be created (i.e. exceeded the
          Platforms supported limits) a Fatal Error is generated.
        o A TLS variable can ONLY store a void pointer (or a size_t integer).
        o Each instance of this class represents a single TLS variable that
          is SAME across ALL threads, i.e. do NOT create a TLS instance per
          thread, create a TLS instance per global variable.
        o TLS instances can NOT be created statically, i.e. they must be
          created AFTER the Cpl::System::Api::initialize() is called.
        o TLS instances can NOT be copied.
 */
class Tls
{
public:
    /// Constructor
    Tls();

    /// Destructor
    ~Tls();

public:
    /** Returns the thread-based stored value.
     */
    virtual void* get( void );

    /** This method sets the thread-based stored value.
     */
    virtual void set( void* newValue );


protected:
    /// Raw TLS key/handle/index 
    Cpl_System_TlsKey_T  m_key;


private:
    /// Prevent access to the copy constructor -->TLS instances can not be copied!
    Tls( const Tls& m );

    /// Prevent access to the assignment operator -->TLS instances can not be copied!
    const Tls& operator=( const Tls& m );

};



};      // end namespaces
};
#endif  // end header latch

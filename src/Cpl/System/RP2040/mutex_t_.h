#ifndef Cpl_System_RP2040_mutex_t_h_
#define Cpl_System_RP2040_mutex_t_h_
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

#include "Cpl/System/PrivateStartup_.h"
#include "pico/mutex.h"
#include <new>

///
namespace Cpl {
///
namespace System {
///
namespace RP2040 {

/** This class implements a brute force approach to initializing a 'pico
    mutex' when the Cpl C++ library is initialized.  This is to support
    statically allocated/created Cpl::System::Mutex instances. The issue here
    is that 'pico mutex' need to be initialized AFTER main() is called.  The 
    Pico-SDK solution for statically allocating mutex instances is C solution 
    that does not work inside the Cpl::System::Mutex object.
 */
class Mutex_T : public Cpl::System::StartupHook_
{
public:
    /** Constructor.
        Note: The registration is assumed to be done from a static allocated
              instance (i.e. before main() and Cpl::System::Api::initialize() is
              called).  However, registering AFTER wards is a don't care because
              the 'start-up list' is only used during the call to Cpl::System::Api::initialize() 
     */
    Mutex_T() :StartupHook_( eSYSTEM ), m_rp2040Mutex(nullptr) {}


protected:
    /// This method is called when Cpl::System::Api::initialize() executes
    void notify( InitLevel_T init_level )
    {
        m_rp2040Mutex = new recursive_mutex_t;
        if ( m_rp2040Mutex )
        {
            recursive_mutex_init( m_rp2040Mutex );
        }
    }

public:
    /// Platform Mutex instance. Note: Made public to simply access from the Cpl::System::Mutex class
    recursive_mutex_t*   m_rp2040Mutex;
};


}   // End namespace(s)
}
}
#endif  // end header latch

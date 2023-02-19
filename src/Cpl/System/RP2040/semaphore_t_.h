#ifndef Cpl_System_RP2040_semaphore_t_h_
#define Cpl_System_RP2040_semaphore_t_h_
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
#include "pico/sem.h"
#include <new>

///
namespace Cpl {
///
namespace System {
///
namespace RP2040 {

/** This class implements a brute force approach to initializing a 'pico
    semaphore' when the Cpl C++ library is initialized.  This is to support
    statically allocated/created Cpl::System::Semphore instances. The issue here
    is that 'pico semaphore' need to be initialized AFTER main() is called.  
 */
class Semaphore_T : public Cpl::System::StartupHook_
{
public:
    /** Constructor.
        Note: The registration is assumed to be done from a static allocated
              instance (i.e. before main() and Cpl::System::Api::initialize() is
              called).  However, registering AFTER wards is a don't care because
              the 'start-up list' is only used during the call to Cpl::System::Api::initialize()
     */
    Semaphore_T( unsigned initialCount ) 
        : StartupHook_( eSYSTEM )
        , m_rp2040Sema( nullptr )
        , m_initialCount( initialCount ) 
    {
    }


protected:
    /// This method is called when Cpl::System::Api::initialize() executes
    void notify( InitLevel_T init_level )
    {
        m_rp2040Sema = new semaphore_t;
        if ( m_rp2040Sema )
        {
            sem_init( m_rp2040Sema, m_initialCount, 0x7FFF );
        }
    }

public:
    /// Platform Semaphore instance. Note: Made public to simply access from the Cpl::System::Semaphore class
    semaphore_t*    m_rp2040Sema;

protected:
    /// Initial count
    unsigned        m_initialCount;
};


}   // End namespace(s)
}
}
#endif  // end header latch

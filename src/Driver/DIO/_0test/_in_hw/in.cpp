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
#include "in.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"

///
using namespace Driver::DIO;

#define SECT_ "_0test"

////////////////////////////////////////////////////////////////////////////////
namespace {



class MyRunnable : public Cpl::System::Runnable
{
public:
    ///
    Driver::DIO::In&    m_uutIn;
    ///
    uint32_t            m_delay;

public:
    ///
    MyRunnable( Driver::DIO::In& uutIn, uint32_t m_delay )
        : m_uutIn( uutIn )
        , m_delay( m_delay )
    {
    }

public:
    ///
    void appRun()
    {

        // Run forever
        for ( int i=1; true; i++ )
        {
            // Start the driver
            if ( !m_uutIn.start() )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Failed to start the IN driver") );
                for ( ;;);
            }

            bool inValue = m_uutIn.sample();
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Loop %d, in=%s", i, inValue ? "ASSERTED" : "deasserted") );

            // stop the driver
            m_uutIn.stop();
            Cpl::System::Api::sleep( m_delay );
        }
    }
};

} // end anonymous namespace

///////////////////////////////////////////////////////////////////////
void runtests( Driver::DIO::In& uutIn,
               uint32_t         loopDelayMs )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting test for the DIO::In driver (delay=%u)", loopDelayMs) );

    MyRunnable*  uutloop     = new(std::nothrow) MyRunnable( uutIn, loopDelayMs );
    Cpl::System::Thread::create( *uutloop, "UUT" );

    // Start the scheduler (when it is NOT already running)
    if ( !Cpl::System::Api::isSchedulingEnabled() )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting scheduler...") );
        Cpl::System::Api::enableScheduling();
    }
}
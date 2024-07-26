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
#include "out_pwm.h"
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
    Driver::DIO::Out&   m_uutOut;
    ///
    Driver::DIO::Pwm&   m_uutPwm;
    ///
    uint32_t            m_delay;

public:
    ///
    MyRunnable( Driver::DIO::Out& uutOut, Driver::DIO::Pwm& uutPwm, uint32_t m_delay )
        : m_uutOut( uutOut )
        , m_uutPwm( uutPwm )
        , m_delay( m_delay )
    {
    }

public:
    ///
    void appRun()
    {
        static uint16_t pwwValues[] ={ 0, OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE / 4, OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE / 2, OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE * 3 / 4, OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE };
        unsigned pwmIdx = 0;
        uint16_t curPwm = pwwValues[pwmIdx];

        // Run forever
        for ( int i=1; true; i++ )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Loop %d, pwm=%d (%d %%)", i, curPwm, (curPwm*100)/ OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE) );

            // Start the driver
            if ( !m_uutOut.start( true ) )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Failed to start the OUT driver") );
                for ( ;;);
            }
            if ( !m_uutPwm.start( curPwm ) )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("Failed to start the PWM driver") );
                for ( ;;);
            }


            /** NOTE: With a SYSCLK of 96MHz - the expected PWM frequency is 1.46KHz */
            for ( int i=0; i < ((int)((10*1000) / m_delay)); i++ )
            {
                Cpl::System::Api::sleep( m_delay );
                bool cur = m_uutOut.getOutput();
                m_uutOut.setOutput( !cur );
            }
            m_uutOut.deassertOutput();

            // stop the driver
            m_uutOut.stop();
            m_uutPwm.stop();
            pwmIdx++;
            if ( pwmIdx > 4 )
            {
                pwmIdx = 0;
            }
            curPwm = pwwValues[pwmIdx];
        }
    }
};

} // end anonymous namespace

///////////////////////////////////////////////////////////////////////
void runtests( Driver::DIO::Out& uutOut,
               Driver::DIO::Pwm& uutPwm,
               uint32_t          loopDelayMs )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting test for the DIO::Out driver (delay=%u)", loopDelayMs) );

    MyRunnable*  uutloop     = new(std::nothrow) MyRunnable( uutOut, uutPwm, loopDelayMs );
    Cpl::System::Thread::create( *uutloop, "UUT" );

    // Start the scheduler (when it is NOT already running)
    if ( !Cpl::System::Api::isSchedulingEnabled() )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting scheduler...") );
        Cpl::System::Api::enableScheduling();
    }
}
#ifndef Driver_DIO_Pwm_h_
#define Driver_DIO_Pwm_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2023  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "colony_map.h"
#include "colony_config.h"

/// Defer the definition of the PWM configuration to the application's 'platform'
#define DriverDioPwmConfig_T          DriverDioPwmConfig_T_MAP

/// Value for Maximum Duty/100% cycle
#ifndef OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE  
#define OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE  0xFFFF
#endif

///
namespace Driver {
///
namespace DIO {

/** This class defines a generic interface for controlling a simple PWM 
    output signal.  
 */
class Pwm
{
public:
    /** Constructor Note: the 'pinConfig' struct MUST stay in scope as long
        as the driver is in scope.
     */
    Pwm( DriverDioPwmConfig_T pinConfig );

public:
    /** Starts the driver.  For 'initialLogicalDutyCycle', a value of 0 is 
        0% duty cycle. A value of OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE
        is 100% duty cycle.

        Returns false if an error was encountered 
     */
    bool start( size_t initialLogicalDutyCycle );

    /** Stops the driver, places the output into a 'safe' state.  The safe state
        is defined by the platform
     */
    void stop();

public:
    /** Sets/updates the duty cycle.  A value of 0 is 0% duty cycle. A
        value of OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE is 100% duty cycle 
     */
    void setDutyCycle( size_t logicalDutyCycle );


protected:
    /// PWM info
    DriverDioPwmConfig_T    m_pwm;

    /// Started flag
    bool                    m_started;
};

} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch

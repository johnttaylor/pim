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

#include "colony_config.h"
#include "Driver/DIO/Pwm.h"

using namespace Driver::DIO;


Pwm::Pwm( Cpl::Dm::Mp::Uint32& pwmConfig )
    : m_pwm( pwmConfig )
    , m_started( false )

{
}
bool Pwm::start( size_t initialDutyCycle )
{
    if ( !m_started )
    {
        m_started = true;
        setDutyCycle( initialDutyCycle );
        return true;
    }

    return false;
}

void Pwm::stop()
{
    if ( m_started )
    {
        m_started = false;
        m_pwm.setInvalid();
    }
}

void Pwm::setDutyCycle( size_t dutyCycle )
{
    if ( m_started )
    {
        if ( dutyCycle > OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE )
        {
            dutyCycle = OPTION_DRIVER_DIO_PWM_MAX_DUTY_CYCLE_VALUE;
        }
        m_pwm.write( dutyCycle );
    }
}



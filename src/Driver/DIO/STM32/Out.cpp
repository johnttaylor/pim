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

#include "Driver/DIO/Out.h"

using namespace Driver::DIO;


Out::Out( DriverDioOutSTM32PinConfig_T pinConfig, bool assertedHigh )
    : m_pin( pinConfig )
    , m_assertedHigh( assertedHigh )
    , m_started( false )
{
}
bool Out::start( bool initialState )
{
    if ( !m_started )
    {
        m_started = true;
        // TODO: Support config without using ST's HAL/MX IDE
        setOutput( initialState );
        return true;
    }

    return false;
}

void Out::stop()
{
    if ( m_started )
    {
        setOutput( false );     // Assumes logical OFF is the a 'safe' state.
        m_started = false;
    }
}

bool Out::getOutput() const
{
    if ( m_started )
    {
        GPIO_PinState phy = HAL_GPIO_ReadPin( m_pin.port, m_pin.pin );
        bool          log = phy ? true : false;
        return m_assertedHigh ? log : !log;
    }

    return false;
}

void Out::setOutput( bool asserted )
{
    if ( m_started )
    {
        bool          phy    = m_assertedHigh ? asserted : !asserted;
        GPIO_PinState pstate = phy ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin( m_pin.port, m_pin.pin, pstate );
    }
}



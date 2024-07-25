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


Out::Out( Cpl::Dm::Mp::Bool& pinConfig, bool assertedHigh )
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
        setOutput( initialState );
        return true;
    }

    return false;
}

void Out::stop()
{
    if ( m_started )
    {
        m_pin.setInvalid();  // Invalidate the MP 
        m_started = false;
    }
}

bool Out::getOutput() const
{
    if ( m_started )
    {
        bool state = false; // Default the state to 'off' when the MP is invalid
        m_pin.read( state );
        return m_assertedHigh ? state : !state;
    }

    return false;
}

void Out::setOutput( bool asserted )
{
    if ( m_started )
    {
        m_pin.write( m_assertedHigh ? asserted : !asserted );
    }
}



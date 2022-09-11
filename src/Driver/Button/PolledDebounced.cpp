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

#include "PolledDebounced.h"

using namespace Driver::Button;

/////////////////////
void PolledDebounced::sample() noexcept
{
    bool rawState = Driver_Button_Hal_getRawPressedState( m_buttonHdl );

    // Reset the count if there is state change
    if ( rawState != m_previousRawPressed )
    {
        m_counts = 0;
    }
    
    // Count same RAW state
    else
    {
        m_counts++;
        
        // Minimum count reached?
        if ( m_counts >= m_requiredCount )
        {
            m_pressed = rawState;
        }
    }
    
    m_previousRawPressed = rawState;
}

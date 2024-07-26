#ifndef Driver_DIO_In_h_
#define Driver_DIO_In_h_
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

/// Defer the definition of the pin configuration to the application's 'platform'
#define DriverDioInPinConfig_T          DriverDioInPinConfig_T_MAP


///
namespace Driver {
///
namespace DIO {

/** This class defines a generic interface for sampling a single Digital Input
    signal.

    The 'asserted' Pin state maps to the signal being logically high.
 */
class In
{
public:
    /** Constructor.  Note: the 'pinConfig' struct MUST stay in scope as long
        as the driver is in scope.
     */
    In( DriverDioInPinConfig_T pinConfig, bool assertedHigh = true );

public:
    /** Starts the driver.
        Returns false if an error was encountered 
     */
    bool start();

    /** Stops the driver.
     */
    void stop();

public:
    /** Returns the current input value. A true value indicates that the 
        signal is asserted state
     */
    bool sample() const;

    /// Returns true if the logical asserted state maps to a physical high signal
    inline bool isAssertedHigh() const
    {
        return m_assertedHigh;
    }

protected:
    /// PIN info
    DriverDioInPinConfig_T    m_pin;

    /// Polarity of the output ping
    bool                      m_assertedHigh;

    /// Started flag
    bool                      m_started;
};

} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch

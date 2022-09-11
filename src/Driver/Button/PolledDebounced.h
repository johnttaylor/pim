#ifndef Driver_Button_PolledDebounced_h_
#define Driver_Button_PolledDebounced_h_
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

#include "Driver/Button/Hal.h"


///
namespace Driver {
///
namespace Button {

/** This concrete class implements a button driver where a single button
    is polled and its raw button state is de-bounced.  The Application
    is responsible for the providing the sampling intervals by calling
    the interface's sample() method.

    The application is RESPONSIBLE for ensuring that the 'button handle'
    is properly initialized before attempting to use the button driver
    instance.

    The class is NOT thread safe.  The application is RESPONSIBLE for ensuring
    thread-safe usage.
 */
class PolledDebounced
{
public:
    /** Constructor. The physical button being is sampled is specified by
        'buttonHandle'.  The 'numConsecutiveCounts' specifies the number of
        consecutive sample periods - without the raw button state changing -
        to declare a new de-bounced state.
    */
    PolledDebounced( Driver_Button_Hal_T buttonHandle,
                     unsigned            numConsecutiveCounts = 2 ) noexcept
        : m_buttonHdl( buttonHandle )
        , m_requiredCount( numConsecutiveCounts )
        , m_counts( 0 )
        , m_previousRawPressed( false )
        , m_pressed( false )
    {
    }

public:
    /** This method is used to start/initialize the driver.  The method can
        also be used to restart the driver.
     */
    inline void start() noexcept { m_counts = 0; m_pressed = false; m_previousRawPressed = false; }


public:
    /** This method returns the de-bounced pressed button state.
     */
    inline bool isPressed() noexcept { return m_pressed; }

    /** The application is required to call this method on fixed periodic
        intervals.  The raw button state is sampled during this call.
     */
    void sample() noexcept;

public:
    /// Returns the driver's button handle
    inline Driver_Button_Hal_T getHandle() { return m_buttonHdl; }


protected:
    /// Handle to the button
    Driver_Button_Hal_T m_buttonHdl;

    /// Required number of consecutive counts
    unsigned            m_requiredCount;

    /// Consecutive counts
    unsigned            m_counts;

    /// Previous raw state
    bool                m_previousRawPressed;

    /// De-bounced state
    bool                m_pressed;
};

} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch

#ifndef ModelPoints_h
#define ModelPoints_h
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
/** @file

    This header file defines the model point instances for my application

 */

#include "colony_config.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Bool.h"


/*----------------------------------------------------------------------------*/
/// Encapsulate all of the model points in the 'mp' namespace to avoid naming collisions
namespace mp {

/// Reference to my Data Model database
extern Cpl::Dm::ModelDatabase modelDatabase;


/*-------------------------------------------------------*/
/** Logical Button: Alt key

    \b Units:
        bool

    \b Range:
        true|false  - A button pressed occurred.
        invalid     - Button has never been pressed OR the driver has not started.

    \b Notes:
        The actual valid value of the MP is NOT used.  The 'button pressed 
        event' is indicated by a change the MP's sequence number when there
        is a physical transition from the pressed-to-unpressed state OR when
        a pressed-repeat-event occurs.  A change in the sequence number will
        generate change notifications to all subscribed clients.
*/
extern Cpl::Dm::Mp::Bool    buttonEventAlt;

/*-------------------------------------------------------*/
/** Logical Button: Move Right key

    \b Units:
        bool

    \b Range:
        true|false  - A button pressed occurred.
        invalid     - Button has never been pressed OR the driver has not started.

    \b Notes:
        The actual valid value of the MP is NOT used.  The 'button pressed 
        event' is indicated by a change the MP's sequence number when there
        is a physical transition from the pressed-to-unpressed state OR when
        a pressed-repeat-event occurs.  A change in the sequence number will
        generate change notifications to all subscribed clients.
*/
extern Cpl::Dm::Mp::Bool    buttonEventRight;

/*-------------------------------------------------------*/
/** Logical Button: Move Left key

    \b Units:
        bool

    \b Range:
        true|false  - A button pressed occurred.
        invalid     - Button has never been pressed OR the driver has not started.

    \b Notes:
        The actual valid value of the MP is NOT used.  The 'button pressed
        event' is indicated by a change the MP's sequence number when there
        is a physical transition from the pressed-to-unpressed state OR when
        a pressed-repeat-event occurs.  A change in the sequence number will
        generate change notifications to all subscribed clients.
*/
extern Cpl::Dm::Mp::Bool    buttonEventLeft;

/*-------------------------------------------------------*/
/** Logical Button: Move Up key

    \b Units:
        bool

    \b Range:
        true|false  - A button pressed occurred.
        invalid     - Button has never been pressed OR the driver has not started.

    \b Notes:
        The actual valid value of the MP is NOT used.  The 'button pressed
        event' is indicated by a change the MP's sequence number when there
        is a physical transition from the pressed-to-unpressed state OR when
        a pressed-repeat-event occurs.  A change in the sequence number will
        generate change notifications to all subscribed clients.
*/
extern Cpl::Dm::Mp::Bool    buttonEventUp;

/*-------------------------------------------------------*/
/** Logical Button: Move Down key

    \b Units:
        bool

    \b Range:
        true|false  - A button pressed occurred.
        invalid     - Button has never been pressed OR the driver has not started.

    \b Notes:
        The actual valid value of the MP is NOT used.  The 'button pressed
        event' is indicated by a change the MP's sequence number when there
        is a physical transition from the pressed-to-unpressed state OR when
        a pressed-repeat-event occurs.  A change in the sequence number will
        generate change notifications to all subscribed clients.
 */
extern Cpl::Dm::Mp::Bool    buttonEventDown;

/*-------------------------------------------------------*/
/** Logical Button: Toggle Color key

    \b Units:
        bool

    \b Range:
        true|false  - A button pressed occurred.
        invalid     - Button has never been pressed OR the driver has not started.

    \b Notes:
        The actual valid value of the MP is NOT used.  The 'button pressed
        event' is indicated by a change the MP's sequence number when there
        is a physical transition from the pressed-to-unpressed state OR when
        a pressed-repeat-event occurs.  A change in the sequence number will
        generate change notifications to all subscribed clients.
*/
extern Cpl::Dm::Mp::Bool    buttonEventColor;

/*-------------------------------------------------------*/
/** Logical Button: Toggle Size key

    \b Units:
        bool

    \b Range:
        true|false  - A button pressed occurred.
        invalid     - Button has never been pressed OR the driver has not started.

    \b Notes:
        The actual valid value of the MP is NOT used.  The 'button pressed
        event' is indicated by a change the MP's sequence number when there
        is a physical transition from the pressed-to-unpressed state OR when
        a pressed-repeat-event occurs.  A change in the sequence number will
        generate change notifications to all subscribed clients.
*/
extern Cpl::Dm::Mp::Bool    buttonEventSize;


/*-------------------------------------------------------*/
/** Logical Button: Clear Screen key

    \b Units:
        bool

    \b Range:
        true|false  - A button pressed occurred.
        invalid     - Button has never been pressed OR the driver has not started.

    \b Notes:
        The actual valid value of the MP is NOT used.  The 'button pressed
        event' is indicated by a change the MP's sequence number when there
        is a physical transition from the pressed-to-unpressed state OR when
        a pressed-repeat-event occurs.  A change in the sequence number will
        generate change notifications to all subscribed clients.
*/
extern Cpl::Dm::Mp::Bool    buttonEventClear;



/*-------------------------------------------------------*/
} // end namespace(s)

#endif // end header latch

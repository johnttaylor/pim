/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

/* Command line options: -p CADIFRA -doxygen -o Fsm -l cppx -Trace Fsm.cdd   */
/* This file is generated from Fsm.cdd - do not edit manually  */
/* Generated on:  version 4.0 */


#ifndef __FSM_STORM_COMPONENT_EQUIPMENT_STAGE_EXT_H__
#define __FSM_STORM_COMPONENT_EQUIPMENT_STAGE_EXT_H__

// Events which can be sent to the state-machine

#ifndef DOXYGEN_WILL_SKIP_THIS

namespace Storm { namespace Component { namespace Equipment { namespace Stage  {

    enum FSM_EVENT_T {
            Fsm_evBackTransitionCompleted=0U,
            Fsm_evLessCapacityNeeded,
            Fsm_evOnRequest,
            Fsm_evOffModeRequest,
            Fsm_evStartingOnTimeExpired,
            Fsm_evStartingOffTimeExpired,
            Fsm_evOffTimeExpired,
            Fsm_evExcessCapacity,
            Fsm_evOnTimeExpired,
            Fsm_evFromTransitionCompleted,
            Fsm_evNeedMoreCapacity,
            FSM_NO_MSG
    };






}}}} //namespace Storm { namespace Component { namespace Equipment { namespace Stage 


#endif // !DOXYGEN_WILL_SKIP_THIS

#endif

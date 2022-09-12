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
/* This file is generated from Fsm_trace - do not edit manually  */
/* Generated on: version 4.0 */ 


#ifndef __FSM_TRACE_STORM_COMPONENT_EQUIPMENT_STAGE_H__
#define __FSM_TRACE_STORM_COMPONENT_EQUIPMENT_STAGE_H__

#include "Cpl/System/Trace.h"

/// Trace Section
#define SECT_ "Storm::Component::Equipment::Stage::Fsm"


#ifndef DOXYGEN_WILL_SKIP_THIS

namespace Storm { namespace Component { namespace Equipment { namespace Stage  {


/*Simulation which can be sent to the state-machine statediagram.simulator  */

#define FsmTraceEvent(a) CPL_SYSTEM_TRACE_MSG( SECT_, ( "Old State=%s, Event=%s", getNameByState(getInnermostActiveState()), FsmTraceEvents[a] ));

const char* const FsmTraceEvents[] = {
    "Fsm_evStartingOnTimeExpired",
    "Fsm_evOnTimeExpired",
    "Fsm_evStartingOffTimeExpired",
    "Fsm_evOffTimeExpired",
    "",
    "Fsm_evFromTransitionCompleted",
    "Fsm_evBackTransitionCompleted",
    "Fsm_evOffModeRequest",
    "Fsm_evNeedMoreCapacity",
    "Fsm_evLessCapacityNeeded",
    "Fsm_evExcessCapacity[isBeingSupplemented()]",
    "Fsm_evExcessCapacity[else]",
    "Fsm_evOnRequest[isBeingSupplemented()]",
    "Fsm_evOnRequest[else]"
};


}}}} //namespace Storm { namespace Component { namespace Equipment { namespace Stage 


#endif // !DOXYGEN_WILL_SKIP_THIS

#endif

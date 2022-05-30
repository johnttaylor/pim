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



/* This file is auto-generated DO NOT MANUALLY EDIT this file! */


#include "FsmEventQueue_.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"

#define SECT_ "Storm::Component::Equipment::Stage::Fsm"

/// Namespace(s)
namespace Storm { namespace Component { namespace Equipment { namespace Stage { 


FsmEventQueue_::FsmEventQueue_()
:Cpl::Container::RingBuffer<FSM_EVENT_T>( 8, m_eventQueMemory )
,m_processingFsmEvent(false)
    {
    }


void FsmEventQueue_::generateEvent( FSM_EVENT_T msg )
    {
    // Queue my event
    if ( !add( msg ) )
        {
        Cpl::System::FatalError::logf( "Storm::Component::Equipment::Stage::FsmEventQueue_: - Buffer Overflow!" );
        }

    // Protect against in-thread 'feedback loops' that can potentially generate events
    if ( !m_processingFsmEvent )
        {
        m_processingFsmEvent = true;
        while( remove( msg ) )
            {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Processing event:= %s, current state=%s ...", getNameByEvent(msg), getNameByState(getInnermostActiveState())) );
            if ( processEvent(msg) == 0 )
                {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("  Event IGNORED:= %s", getNameByEvent(msg)) );
                }
            CPL_SYSTEM_TRACE_MSG( SECT_, ("Event Completed:=  %s, end state=%s", getNameByEvent(msg), getNameByState(getInnermostActiveState())) );
            }

        m_processingFsmEvent = false;
        }
    }

};};};};  /// end namespace(s)

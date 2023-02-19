/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "application.h"
#include "Bob.h"
#include "Cpl/System/Trace.h"
#include "Bsp/Api.h"         // Get access to LED methods


///////////////////////////////////////////////////
Bob::Bob( Cpl::Dm::MailboxServer&  myMbox,
          Cpl::Dm::Mp::Uint32&     delayTimeModelPoint,
          Cpl::Dm::Mp::Bool&       verboseModelPoint )
    : m_mpDelayTime( delayTimeModelPoint )
    , m_mpVerbose( verboseModelPoint )
    , m_observerDelay( myMbox, *this, &Bob::delayModelPointChange )
    , m_observerVerbose( myMbox, *this, &Bob::verboseModelPointChange )
    , m_numEdges( 0 )
    , m_verbose( false )
{
}

///////////////////////////////////////////////////
void Bob::start()
{
    // Skip if already opened
    if ( !m_opened )
    {
        // Housekeeping
        m_opened     = true;
        m_timeMarker = Cpl::System::ElapsedTime::precision();
        m_numEdges   = 1;

        // Subscribe to my model points
        m_mpDelayTime.attach( m_observerDelay );
        m_mpVerbose.attach( m_observerVerbose );

        // Start with the LED on
        Bsp_Api_turnOn_debug1();
    }
}

void Bob::stop()
{
    // Skip if already closed
    if ( m_opened )
    {
        // Housekeeping
        m_opened = false;

        // Cancel my subscriptions
        m_mpDelayTime.detach( m_observerDelay );
        m_mpVerbose.detach( m_observerVerbose );

        // Make sure that the LED is turned off
        Bsp_Api_turnOff_debug1();
    }
}

///////////////////////////////////////////////////
void Bob::delayModelPointChange( Cpl::Dm::Mp::Uint32 & mp )
{
    CPL_SYSTEM_TRACE_MSG( "app", ("Bob:delayModelPointChange()") );

    uint32_t delayMs;
    if ( mp.readAndSync( delayMs, m_observerDelay ) )
    {
        m_delayMs = delayMs;
    }
}

void Bob::verboseModelPointChange( Cpl::Dm::Mp::Bool & mp )
{
    CPL_SYSTEM_TRACE_MSG( "app", ("Bob:verboseModelPointChange()") );
    bool verbose;
    if ( mp.readAndSync( verbose, m_observerVerbose ) )
    {
        m_verbose = verbose;
    }
}

void Bob::periodicInterval( Cpl::System::ElapsedTime::Precision_T currentTick,
                            Cpl::System::ElapsedTime::Precision_T currentInterval,
                            void*                                 context )
{
    //CPL_SYSTEM_TRACE_MSG( "app", ("Bob:periodicInterval()") );

    Bob* self = (Bob*) context;

    // Toggle the LED when my delay timing expires
    if ( Cpl::System::ElapsedTime::expiredPrecision( self->m_timeMarker, self->m_delayMs, currentInterval ) )
    {
        Bsp_Api_toggle_debug1();
        self->m_timeMarker = currentInterval;
        self->m_numEdges++;
        if ( self->m_verbose )
        {
            CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Bob: edge counter=%u", self->m_numEdges) );
        }
    }
}



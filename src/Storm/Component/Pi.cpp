/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/


#include "Pi.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Assert.h"

#define SECT_   "Storm::Component::Pi"


/// Namespaces
using namespace Storm::Component;



///////////////////////////////
Pi::Pi( struct Input_T ins, struct Output_T outs )
    : m_in( ins )
    , m_out( outs )
{
    CPL_SYSTEM_ASSERT( m_in.freezePiRefCnt );
    CPL_SYSTEM_ASSERT( m_in.idtDeltaError );
    CPL_SYSTEM_ASSERT( m_in.inhibitfRefCnt );
    CPL_SYSTEM_ASSERT( m_in.pulseResetPi );
    CPL_SYSTEM_ASSERT( m_in.systemConfig );
    CPL_SYSTEM_ASSERT( m_out.pvInhibited );
    CPL_SYSTEM_ASSERT( m_out.pvOut );
    CPL_SYSTEM_ASSERT( m_out.sumError );
}

bool Pi::start( Cpl::System::ElapsedTime::Precision_T& intervalTime )
{
    // Initialize my data
    m_dt           = ( float) (intervalTime.m_thousandths + intervalTime.m_seconds * 1000);
    m_prevSumError = 0.0F;
    m_prevPvOut    = 0.0F;
    m_maxSumError  = 0.0F;

    // Initialize parent class
    return Base::start( intervalTime );
}


///////////////////////////////
bool Pi::execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                  Cpl::System::ElapsedTime::Precision_T currentInterval
)
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    //--------------------------------------------------------------------------
    // Pre-Algorithm processing
    //--------------------------------------------------------------------------

    // Get my inputs
    bool                                  resetPi            = false;
    float                                 deltaError         = 0.0F;
    uint32_t                              freezeRefCnt       = 0;
    uint32_t                              inhibitRefCnt      = 0;
    Storm::Type::SystemConfig_T           sysCfg             = { 0, };
    int8_t                                validResetPi       = m_in.pulseResetPi->read( resetPi );
    int8_t                                validDeltaError    = m_in.idtDeltaError->read( deltaError );
    int8_t                                validSystem        = m_in.systemConfig->read( sysCfg );
    int8_t                                validFreezeRefCnt  = m_in.freezePiRefCnt->read( freezeRefCnt );
    int8_t                                validInhibitRefCnt = m_in.inhibitfRefCnt->read( inhibitRefCnt );
    if ( validResetPi == false ||
         validDeltaError == false ||
         validSystem == false ||
         validFreezeRefCnt == false ||
         validInhibitRefCnt == false )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Pi::execute. One or more invalid MPs (resetPi=%d, deltaError=%d, system=%d, freezeRefCnt=%d, inhibitRefCnt=%d", validResetPi, validDeltaError, validSystem, validFreezeRefCnt, validInhibitRefCnt ) );

        // 'Freeze' the current PI values if the we don't have all of the required inputs
        freezeRefCnt = 1;
    }

    // Housekeeping
    float gain      = sysCfg.gain;
    float resetTime = sysCfg.reset;
    float maxPvOut  = sysCfg.maxPvOut;

    // Default the output values
    float pvOut        = m_prevPvOut;
    float sumError     = m_prevSumError;
    bool  inhibitState = false;


    //--------------------------------------------------------------------------
    // Algorithm processing
    //
    // NOTE: Floating point operations that have the potential to overflow the 
    //       precision of a float are up cast to a double then the final result 
    //       is cast back to a float to minimize the arithmetic errors.
    //--------------------------------------------------------------------------

    // Trap a reset-the-Controller request
    if ( resetPi )
    {
        pvOut         = m_prevPvOut    = 0.0F;
        sumError      = m_prevSumError = 0.0F;
        m_maxSumError = 0.0F;
    }

    // Check for freeze-the-output request
    if ( freezeRefCnt != 0 )
    {
        inhibitState = true;
    }

    // NOT stopped/frozen -->go do stuff :)
    else
    {
        // Sum the delta error (but don't allow negative sums)
        float newSumError = m_prevSumError + deltaError;
        if ( newSumError < 0.0F )
        {
            newSumError = 0.0F;
        }

        // Clamp the sum error when it exceeds the 'max integral' value
        bool  noUpdateToSumError = false;
        if ( Cpl::Math::areFloatsEqual( m_maxSumError, 0.0F ) == true ) // Only calculate maxSumError once
        {
            m_maxSumError  = ( float) ( ( ( double) ( maxPvOut ) * ( double) ( resetTime ) ) / ( ( double) ( gain ) * ( double) ( m_dt ) ) );
        }
        if ( newSumError > m_maxSumError )
        {
            newSumError  = m_maxSumError;
            inhibitState = true;
        }

        // Calculate the OUT value
        pvOut = ( float) ( ( double) ( gain ) * ( ( double) ( deltaError) +( ( ( double) newSumError * ( double) ( m_dt ) ) / ( double) ( resetTime ) ) ) );


        // Do not let the OUT value go negative
        if ( pvOut < 0.0F )
        {
            pvOut = 0.0F;
        }

        // Clamp the OUT value when required
        else if ( pvOut > maxPvOut )
        {
            pvOut              = maxPvOut;
            noUpdateToSumError = true;
        }


        // Update my integral term when not inhibited
        if ( inhibitRefCnt == 0 && noUpdateToSumError == false )
        {
            sumError = newSumError;
        }
        else
        {
            inhibitState = true;
        }

        // Cache my final outputs
        m_prevSumError = sumError;
        m_prevPvOut    = pvOut;
    }


    //--------------------------------------------------------------------------
    // Post-Algorithm processing
    //--------------------------------------------------------------------------

    // Set my outputs
    m_out.pvOut->write( pvOut );
    m_out.sumError->write( sumError );
    m_out.pvInhibited->write( inhibitState );

    // If I get here -->everything worked!
    return true;
}



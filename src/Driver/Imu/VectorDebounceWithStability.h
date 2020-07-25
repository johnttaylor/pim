#ifndef Driver_Imu_VectorDebounceWithStability_h_
#define Driver_Imu_VectorDebounceWithStability_h_
/*-----------------------------------------------------------------------------
* This file is part of the Arduino Project.  The Arduino Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/arduino/license.txt
*
* Copyright (c) 2017  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Driver/Imu/VectorFilter.h"
#include <stdint.h>


namespace Driver {
namespace Imu {

/** This concrete class provides a basic debounce filter algorithm across all
    axises of the Vector.  In addition it supports the concept of stability, 
    i.e. the debounced value has not changed for at least N samples.
 */
template <class T>
class VectorDebounceWithStability : public VectorFilter<T>
{
protected:
    /// Debounce counter
    uint16_t    m_debounceCounter;

    /// Required debounce counts;
    uint16_t    m_debounceLimit;

    /// Equality tolerance
    T           m_tolerance;

    /// Last raw value
    Driver::Imu::Vector<T> m_lastRaw;

    /// Last debounced value;
    Driver::Imu::Vector<T> m_lastDebounced;

    /// Last stable value
    Driver::Imu::Vector<T> m_lastStable;

    /// Stability counter
    uint16_t    m_stabilityCounter;

    /// Required number consecutive debounced values for stability
    uint16_t    m_stableLimit;

    /// When true, only stable values are returned
    bool        m_onlyReturnStableValues;

    /// Stability State
    bool        m_stable;

    /// Flag used to keep track of the initial filter call
    bool        m_haveBeenCalledAtLeastOnce;

public:

    /** Constructor.  The argument 'consecutiveCounts' is the number
        consecutive samples that are equal - within the 'equalTolerance' - to
        accept a new value.  The 'stabilityCounts' argument is number of
        consecutive valid debounced values required to declare the current
        debounced value stable.
     */
    VectorDebounceWithStability( T equalTolerance, uint16_t consecutiveCounts=1, uint16_t stabiliyCounts=1, bool onlyReturnStableValues=false )
        : m_debounceCounter( 0 )
        , m_debounceLimit( consecutiveCounts )
        , m_tolerance( equalTolerance )
        , m_stabilityCounter( 0 )
        , m_stableLimit( stabiliyCounts )
        , m_onlyReturnStableValues( onlyReturnStableValues )
        , m_stable( false )
        , m_haveBeenCalledAtLeastOnce( false )
    {
    }

public:
    /// Return the current stability state
    bool isStable( void ) { return m_stable; }

    /// See Filter
    Driver::Imu::Vector<T> filterValue( const Driver::Imu::Vector<T>& rawValue )
    {
        // Allow the first value to come through unfiltered
        if ( m_haveBeenCalledAtLeastOnce )
        {
            m_lastDebounced = rawValue;
            m_lastStable    = rawValue;
            m_stable        = true;
        }

        // Apply the debounce algorithm
        else
        {
            // Debounce the incoming raw values
            bool isXEqual = equalEnough( rawValue.x, m_lastRaw.x, m_tolerance );
            bool isYEqual = equalEnough( rawValue.y, m_lastRaw.y, m_tolerance );
            bool isZEqual = equalEnough( rawValue.z, m_lastRaw.z, m_tolerance );
            if ( !isXEqual || !isYEqual || !isZEqual )
            {
                m_debounceCounter   = 0;
                m_stabilityCounter  = 0;
                m_stable            = false;
            }
            else
            {
                if ( ++m_debounceCounter >= m_debounceLimit )
                {
                    m_lastDebounced   = rawValue;
                    m_debounceCounter = m_debounceLimit;

                    // Check for stability
                    if ( ++m_stabilityCounter >= m_stableLimit )
                    {
                        m_stable            = true;
                        m_lastStable        = rawValue;
                        m_stabilityCounter  = m_stableLimit;
                    }
                }
            }
            m_lastRaw = rawValue;
        }

        return m_onlyReturnStableValues ? m_lastStable : m_lastDebounced;
    }


protected:
    /// Helper function
    static bool equalEnough( T a, T b, T tolerance )
    {
        if ( a <= b )
        {
            if ( a >= b - tolerance )
            {
                return true;
            }
        }
        else
        {
            if ( a <= b + tolerance )
            {
                return true;
            }
        }

        return false;
    }
};


};      // end Namespaces
};
#endif  // end Header latch
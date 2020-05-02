#ifndef Storm_Utils_DutyCycle_h_
#define Storm_Utils_DutyCycle_h_
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
/** @file */

#include <stdint.h>
#include "Storm/Type/Cph.h"

/// Namespaces
namespace Storm {
/// Namespaces
namespace Utils {

/** This static class provides methods to calculates on / off cycle times based
    on the output of PI component.The PI output has linear mapping to duty cycle
    as defined as :

    dutyCycle =      onTime
    --------------
    onTime + offTime

    max - PI - Output   = 100 % duty cycle.
    max - PI - output / 2 = 50 % duty cycle.
    zero - PI - output  = 0 % duty cycle.
 */
class DutyCycle
{
public:
    /** This method calculates the off cycle time in milliseconds based on the
        provided 'process variable'.  The mapping of the 'pvVar' is done
        over of the range specified by the 'pvLowerBound' and 'pvUpperBound', e.g.
        if the pvVar = 125 and pvLowerBound=100 and pvUpperBound=150, then the
        'pvVar' input is normalized to 50% before the cycle time is calculated.

        The calculated time will never be less than 'minOffTime'.  However, the
        specified 'minOffTime' may be lower based on the CPH setting to ensure
        that the relationship between the Duty Cycle and the Process Variable 
        value does NOT have any horizontal regions (i.e. dead bands in the
        mapping).

        The 'cyclePerHour' argument is used to determine the 50% duty on/off
        times.  For example the 50% on/off times for 3CPH are 10min on/off.
     */
    static uint32_t calculateOffTime( float pvVar, uint32_t minOffTimeSec, Storm::Type::Cph cyclesPerHour, float pvLowerBound=0.0F, float pvUpperBound=100.0F );


    /** This method calculates the on cycle time in milliseconds based on the
        provided 'process variable'.  The mapping of the 'pvVar' is done
        over of the range specified by the 'pvLowerBound' and 'pvUpperBound', e.g.
        if the pvVar = 125 and pvLowerBound=0 and pvUpperBound=100, then the
        'pvVar' input is normalized to 100% before the cycle time is calculated

        The calculated time will never be less than 'minOffTime'.  However, the
        specified 'minOffTime' may be lower based on the CPH setting to ensure
        that the relationship between the Duty Cycle and the Process Variable
        value does NOT have any horizontal regions (i.e. dead bands in the
        mapping).

        The 'cyclePerHour' argument is used to determine the 50% duty on/off
        times.  For example the 50% on/off times for 6CPH are 5min on/off
     */
    static uint32_t calculateOnTime( float pvVar, uint32_t minOnTimeSec, Storm::Type::Cph cyclesPerHour, float pvLowerBound=0.0F, float pvUpperBound=100.0F );

public:
    /** This method returns the max allowed minOnTime (in milliseconds) for the specified
        CPH option.
     */
    static uint32_t getMaximumMinOnTime( Storm::Type::Cph cyclesPerHour );

    /** This method returns the max allowed minOffTime (in milliseconds) for the specified
        CPH option.
     */
    static uint32_t getMaximumMinOffTime( Storm::Type::Cph cyclesPerHour );


};

};      // end namespace
};
#endif  // end header latch

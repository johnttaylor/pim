#ifndef Storm_Type_EquipmentTimes_h_
#define Storm_Type_EquipmentTimes_h_
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

#include "Cpl/System/ElapsedTime.h"


///
namespace Storm {
///
namespace Type {

/** The following typedef is used to store the 'begin times' for the Indoor and
    Outdoor HVAC equipment.  This information is used to enforce minimum equipment
    on/off times.
 */
typedef struct
{
    Cpl::System::ElapsedTime::Precision_T   outdoorUnitBeginOnTime;     //!< The starting time of when the Outdoor unit (aka the compressor) was turned on
    Cpl::System::ElapsedTime::Precision_T   outdoorUnitBeginOffTime;    //!< The starting time of when the Outdoor unit (aka the compressor) was turned off
    Cpl::System::ElapsedTime::Precision_T   indoorUnitBeginOnTime;      //!< The starting time of when the Indoor unit (aka indoor heat) was turned on
    Cpl::System::ElapsedTime::Precision_T   indoorUnitBeginOffTime;     //!< The starting time of when the Indoor unit (aka indoor heat) was turned off
    Cpl::System::ElapsedTime::Precision_T   systemBeginOnTime;          //!< The starting time of when at least one 'stage' of the SYSTEM was turned on
    Cpl::System::ElapsedTime::Precision_T   systemBeginOffTime;         //!< The starting time of when ALL 'stages' of the SYSTEM was turned off
} EquipmentTimes_T;


};      // end namespaces
};
#endif  // end header latch

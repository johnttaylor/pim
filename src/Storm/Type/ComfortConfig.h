#ifndef Storm_Type_ComfortConfig_h_
#define Storm_Type_ComfortConfig_h_
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

#include "Storm/Type/Cph.h"


///
namespace Storm {
///
namespace Type {

/** The 'comfort' parameters per stage
 */
typedef struct
{
    int         cph;         //!< Cycle-Per-Hour settings.   The actual type is: Storm::Type::Cph ('betterenums' do not play well with classes/struct)
    uint32_t    minOnTime;   //!< Minimum on time in seconds
    uint32_t    minOffTime;  //!< Minimum off time in seconds
} ComfortStageParameters_T;

/** The 'comfort' parameters for the system
 */
typedef struct
{
    ComfortStageParameters_T compressorCooling;     //!< Parameters for compressor cooling operation
    ComfortStageParameters_T compressorHeating;     //!< Parameters for compressor heating operation
    ComfortStageParameters_T indoorHeating;         //!< Parameters for indoor heating operation
} ComfortConfig_T;



};      // end namespaces
};
#endif  // end header latch

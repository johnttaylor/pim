#ifndef Storm_Type_HvacRelayOutputs_h_
#define Storm_Type_HvacRelayOutputs_h_
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

#include "Storm/Constants.h"
#include <stdint.h>



///
namespace Storm {
///
namespace Type {

/** The following typedef is used to hold the HVAC Relay outputs, i.e. HVAC 
    outputs for a non-communicating, 24V wire/signals controlled HVAC system.
 */
typedef struct
{
 
    bool      g;                                                //!< Indoor blower relay.  True=on, False=off
    uint8_t   bk;                                               //!< PWM Output to control indoor blower speed. 0=off, 100 = 100% speed
    bool      w1;                                               //!< First stage of indoor heat.  True=on, False=off
    bool      w2;                                               //!< Second stage of indoor heat.  True=on, False=off
    bool      w3;                                               //!< Third stage of indoor heat.  True=on, False=off
    bool      o;                                                //!< Switch-over-valve (for a HP).  True=cooling, False=heating;
    bool      y1;                                               //!< First compressor stage.  True=on, False=off;
    bool      y2;                                               //!< Second compressor stage.  True=on, False=off;
} HvacRelayOutputs_T;


};      // end namespaces
};
#endif  // end header latch

#ifndef Storm_Type_CycleStatus_h_
#define Storm_Type_CycleStatus_h_
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


#include "Cpl/Type/enum.h"


///
namespace Storm {
///
namespace Type {

/** The following enum/class defines the state/status of the current on/off
    cycle.

    @param CycleStatus              - Class name for the Cycle Status enum
    @param int                      - storage type of the enum
    @param eOFF                     - The equipment/system is off
    @param eON_CYCLE                - Currently executing an on cycle
    @param eOFF_CYCLE               - Currently executing an off cycle
    @param eTRANSITIONING_UP        - The system is processing transitioning to the next/higher stage - and not explicitly executing an tradition on/off cycle
    @param eTRANSITIONING_DOWN      - The system is processing transitioning to the previous/lower stage - and not explicitly executing an tradition on/off cycle
 */ 
BETTER_ENUM( CycleStatus, int, eOFF=0, eON_CYCLE, eOFF_CYCLE, eTRANSITIONING_UP, eTRANSITIONING_DOWN );


};      // end namespaces
};
#endif  // end header latch

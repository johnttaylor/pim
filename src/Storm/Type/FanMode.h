#ifndef Storm_Type_FanMode_h_
#define Storm_Type_FanMode_h_
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

/** The following enum/class defines the allowed Fan Modes.  The Fan mode is 
    set/selected by the User.

    @param FanMode                  - Class name for the Fan Mode enum
    @param int                      - storage type of the enum
    @param eAUTO                    - Auto Mode (the fan turns on when there is active conditioning)
    @param eCONTINUOUS              - Continuous Mode (the fan is always on)
 */
BETTER_ENUM( FanMode, int, eAUTO=0, eCONTINUOUS );


};      // end namespaces
};
#endif  // end header latch

#ifndef Storm_Type_Cph_h_
#define Storm_Type_Cph_h_
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

#include "Cpl/Type/enum.h"


///
namespace Storm {
///
namespace Type {

/** The following enum/class defines the valid Cycles-Per-Hours options. 

    @param Cph                     - Class name for the Cycles Per Hour enum
    @param int                      - storage type of the enum
    @param e2CPH                    - 2 cycles per hour
    @param e3CPH                    - 3 cycles per hour
    @param e4CPH                    - 4 cycles per hour
    @param e5CPH                    - 5 cycles per hour
    @param e6CPH                    - 6 cycles per hour
    @param eNUM_OPTIONS             - Number of CPH options
 */
BETTER_ENUM( Cph, int, e2CPH=0, e3CPH, e4CPH, e5CPH, e6CPH, eNUM_OPTIONS );


};      // end namespaces
};
#endif  // end header latch
#ifndef Storm_Type_OduType_h_
#define Storm_Type_OduType_h_
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

/** The following enum/class defines the support Outdoor Unit Types.

    @param OduType           - Class name for the Indoor Unit Type enum
    @param int               - storage type of the enum
    @param eAC               - Outdoor Unit is an Air Conditioner
    @param eHP               - Outdoor Unit is a heat pump
 */
BETTER_ENUM( OduType, int, eAC, eHP );


};      // end namespaces
};
#endif  // end header latch

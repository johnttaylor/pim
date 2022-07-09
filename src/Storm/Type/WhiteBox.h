#ifndef Storm_Type_WhiteBox_h_
#define Storm_Type_WhiteBox_h_
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



///
namespace Storm {
///
namespace Type {

/** The following typedef contains variables/settings/flags/etc. that are
    use to facilitate white box testing of the system.  All of the flags,
    settings SHOULD BE designed such that a 'zero' value means NORMAL operation.
 */
typedef struct
{
    bool defeatEquipMinOffTime;     //!< When set to true the equipment minimum off time is NOT enforced
    bool abortOnOffCycle;           //!< When set to true the current on/off cycle is aborted and the opposite cycle (i.e. ON->off, or off->ON) begins. Note: This flag is self CLEARED at the end of every 'algorithm processing cycle'
} WhiteBox_T;



};      // end namespaces
};
#endif  // end header latch

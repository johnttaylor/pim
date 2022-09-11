#ifndef Storm_Component_HvacRelayOutputs_h_
#define Storm_Component_HvacRelayOutputs_h_
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

#include "Storm/Component/Base.h"
#include "Cpl/Dm/Mp/Float.h"
#include "Cpl/Dm/Mp/Bool.h"
#include "Cpl/Dm/Mp/RefCounter.h"
#include "Storm/Dm/MpSystemConfig.h"
#include "Storm/Dm/MpVirtualOutputs.h"
#include "Storm/Dm/MpEquipmentBeginTimes.h"
#include "Storm/Dm/MpHvacRelayOutputs.h"


/// Namespaces
namespace Storm
{
/// Namespaces
namespace Component
{
/** This class is responsible for implementing the Indoor Fan Policies (e.g.
    Fan Continuous operation)
 */
class HvacRelayOutputs : public Base
{
public:
    /// Input Model Points
    struct Input_T
    {
        Storm::Dm::MpVirtualOutputs*        vOutputs;               //!< The virtual system outputs
        Storm::Dm::MpEquipmentBeginTimes*   equipmentBeginTimes;    //!< The begin times for when the HVAC outputs turned on/off
        Cpl::Dm::Mp::RefCounter*            systemForcedOffRefCnt;	//!< Reference Counter: When greater the zero the system is required to be forced off.
        Cpl::Dm::Mp::Bool*                  systemOn;               //!< Indicates that system is actively Cooling or Heating.  Note: this is not the same thing as the equipment is physically on, e.g I am actively conditioning the space - but currently in an off cycle
    };


    /// Output Model Points
    struct Output_T
    {
        Storm::Dm::MpEquipmentBeginTimes*   equipmentBeginTimes;    //!< The begin times for when the HVAC outputs turned on/off
        Storm::Dm::MpHvacRelayOutputs*      relayOutputs;           //!< The HVAC system's relay outputs
    };


public:
    /// Constructor
    HvacRelayOutputs( struct Input_T ins, struct Output_T outs );

    /// See Storm::Component::Api
    bool start( Cpl::System::ElapsedTime::Precision_T& intervalTime );

protected:
    /// See Storm::Component::Base
    bool execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                  Cpl::System::ElapsedTime::Precision_T currentInterval );

protected:
    /// My Model Point inputs
    struct Input_T  m_in;

    /// My Model Point outputs
    struct Output_T m_out;

    /// Previous system-on state
    bool            m_systemOn;

    /// Previous Y1 state
    bool            m_y1;

    /// Previous W1 state
    bool            m_w1;
};



};      // end namespace
};      // end namespace
#endif  // end header latch

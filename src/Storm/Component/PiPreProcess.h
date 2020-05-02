#ifndef Storm_Component_PiPreProcess_h_
#define Storm_Component_PiPreProcess_h_
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

#include "colony_config.h"
#include "Storm/Component/Base.h"
#include "Storm/Dm/MpSetpoints.h"
#include "Storm/Dm/MpSystemConfig.h"
#include "Cpl/Dm/Mp/Float.h"
#include "Cpl/Dm/Mp/Bool.h"


/// Namespace
namespace Storm {
/// Namespace
namespace Component {


/** This concrete class provide the context for a PI controller.  The
    'context' is responsible defining the current delta error input, PI gains
    & reset times, max OUT, etc. to the PI controller.
 */
class PiPreProcess : public Base
{
public:
    /// Input Model Points
    struct Input_T
    {
        Cpl::Dm::Mp::Float*             activeIdt;              //!< The current indoor temperature (in degrees F)
        Storm::Dm::MpSystemConfig*      systemConfig;           //!< Current system configuration based on equipment and current operating mode
        Cpl::Dm::Mp::Bool*              operatingModeChange;    //!< When true, indicates that the current operating mode (heating vs cooling) has changed
        Storm::Dm::MpSetpoints*         setpoints;              //!< The current heating/cooling setpoints (in degrees F)
    };


    /// Output Model Points
    struct Output_T
    {
        Cpl::Dm::Mp::Float*             activeSetpoint;         //!< The active setpoint (based on the current operating mode)
        Cpl::Dm::Mp::Float*             idtDeltaError;          //!< The delta error (in degrees F) between the current IDT the 'active' setpoint
        Cpl::Dm::Mp::Float*             setpointDelta;          //!< The change (in degrees F) in the 'active' setpoint when the active setpoint changes value
        Cpl::Dm::Mp::Bool*              setpointChanged;        //!< When true, indicates that the current 'active' setpoint has changed value.  Note: this flag is NOT set if/when the active setpoint changes between heating/cooling modes
    };


protected:
    /// The Component's inputs
    Input_T  m_in;          

    /// The Component's outputs
    Output_T m_out;          

    /// Cached active setpoint value;
    float    m_prevActiveSetpoint;


public:
    /// Constructor.  
    PiPreProcess( struct Input_T ins, struct Output_T outs );

    /// Component specific initialization
    bool start( Cpl::System::ElapsedTime::Precision_T newInterval );

protected:
    /// See Storm::Component::Base
    bool execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                  Cpl::System::ElapsedTime::Precision_T currentInterval );

};



};      // end namespace
};      // end namespace
#endif  // end header latch

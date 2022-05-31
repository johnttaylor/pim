#ifndef Storm_Component_Pi_h_
#define Storm_Component_Pi_h_
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
#include "Storm/Dm/MpSystemConfig.h"
#include "Cpl/Dm/Mp/RefCounter.h"
#include "Cpl/Dm/Mp/Float.h"
#include "Cpl/Dm/Mp/Bool.h"


/// Namespaces
namespace Storm
{
/// Namespaces
namespace Component
{
/** This concrete class implements a simple (P)roportional (I)ntegral
    controller that only allows the integral term to be a positive number. This
    class is simplification of a basic PID Controller (see https://en.wikipedia.org/wiki/PID_controller)
    
    The class has the following 'integral wind-up' protections:

        o Allows many external entities to explicitly inhibit the integral term.

        o The output of PI is clamped based on the configuration parameter
          piConstants.maxPvOut.  When the output is clamped the integral term is
          inhibited.

        o The integral term is clamped such that integral term by itself
          (i.e. with zero error) does not exceed the maximum configured
          output of the PI.
 */
class Pi : public Base
{
public:
    /// Input Model Points
    struct Input_T
    {
        Cpl::Dm::Mp::Bool*          pulseResetPi;           //!< Triggers a reset-the-PI-controller request
        Cpl::Dm::Mp::Float*         idtDeltaError;          //!< The delta error (in degrees F) between the current IDT the 'active' setpoint
        Storm::Dm::MpSystemConfig*  systemConfig;           //!< Current system configuration based on equipment and current operating mode
        Cpl::Dm::Mp::RefCounter*    freezePiRefCnt;	        //!< Reference Counter: When greater the zero the PI output value is 'frozen' (i.e. won't change value) and the internal integral term is not updated/changed
        Cpl::Dm::Mp::RefCounter*    inhibitfRefCnt;	        //!< Reference Counter: When greater the zero the internal integral term is not update/changed.
    };


    /// Output Model Points
    struct Output_T
    {
        Cpl::Dm::Mp::Float*         pvOut;                  //!< Output of the PI Controller.  This is unit-less positive number that ranges from 0.0 to piConstants.maxPvOut
        Cpl::Dm::Mp::Float*         sumError;               //!< An internal intermediate value/variable that represents the integral term of the PI
        Cpl::Dm::Mp::Bool*          pvInhibited;            //!< This flag is true if the PI Controller's integral is currently inhibited. Note: This flag includes any internal inhibiting of the integral term as well as being set true when the PI 'frozen'
    };


public:
    /// Constructor
    Pi( struct Input_T ins, struct Output_T outs );

    /// See Storm::Component::Api
    bool start( Cpl::System::ElapsedTime::Precision_T& intervalTime );

protected:
    /// See Storm::Component::Base
    bool execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                  Cpl::System::ElapsedTime::Precision_T currentInterval );

protected:
    /// The Component's inputs
    Input_T  m_in;

    /// The Component's outputs
    Output_T m_out;

    /// dt interval time - in milliseconds - as a float (instead of Precision_T struct)
    float m_dt;

    /// Previous sum error term
    float m_prevSumError;

    /// Previous OUT value
    float m_prevPvOut;

    /// Maximum Allowed sum error term
    float m_maxSumError;
};



};      // end namespace
};      // end namespace
#endif  // end header latch

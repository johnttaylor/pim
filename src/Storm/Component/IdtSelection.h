#ifndef Storm_Component_IdtSelection_h_
#define Storm_Component_IdtSelection_h_
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

#include "Cpl/Dm/Mp/Float.h"
#include "Cpl/Dm/Mp/Bool.h"
#include "Cpl/Dm/Mp/RefCounter.h"
#include "Storm/Dm/MpIdtAlarm.h"
#include "Storm/Component/Base.h"




/// Namespaces
namespace Storm {
/// Namespaces
namespace Component {


/** This concrete class determines which Indoor-Temperature (IDT) value to be used
    for the rest of the algorithm.  In addition, it is responsible for generating
    alarms associated with the IDT inputs.
 */
class IdtSelection : public Base
{
public:
    /// Input Model Points
    struct Input_T
    {
        Cpl::Dm::Mp::Float*      primaryIdt;                //!< Current indoor temperature in degrees Fahrenheit of the PRIMARY IDT sensor (aka the onboard sensor)
        Cpl::Dm::Mp::Float*      secondaryIdt;              //!< Current indoor temperature in degrees Fahrenheit of the SECONDARY IDT sensor (aka the remote sensor)
        Cpl::Dm::Mp::Bool*       enabledSecondaryIdt;		//!< Indicates that Secondary IDT sensor is available
    };


    /// Output Parameters
    struct Output_T
    {
        Cpl::Dm::Mp::Float*      activeIdt;				    //!< Current indoor temperature in degrees Fahrenheit to be used for the algorithm
        Cpl::Dm::Mp::RefCounter* systemForcedOffRefCnt;	    //!< Reference Counter: When greater the zero the system is required to be forced off.
        Storm::Dm::MpIdtAlarm*   idtAlarms;                 //!< Indoor Temperature Alarms
    };

    /// Component specific initialization
    bool start( Cpl::System::ElapsedTime::Precision_T& newInterval );

protected:
    /// My Model Point inputs
    struct Input_T  m_in;

    /// My Model Point outputs
    struct Output_T m_out;

    /// Remember my critical state
    bool            m_critical;

public:
    ///Constructor.  
    IdtSelection( struct Input_T inputs, struct Output_T outputs );


protected:
    /// See Storm::Component::Base
    bool execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                  Cpl::System::ElapsedTime::Precision_T currentInterval );
};



};      // end namespace
};      // end namespace
#endif  // end header latch

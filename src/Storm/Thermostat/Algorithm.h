#ifndef Storm_Thermostat_Algorithm_h_
#define Storm_Thermostat_Algorithm_h_
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

#include "Storm/Component/IdtSelection.h"
#include "Storm/Component/OperatingMode.h"
#include "Storm/Component/PiPreProcess.h"
#include "Storm/Component/Pi.h"
#include "Storm/Component/Control.h"
#include "Storm/Component/FanControl.h"
#include "Storm/Component/HvacRelayOutputs.h"   
#include "Storm/Component/AirFilterMonitor.h"   
#include "Storm/Component/Equipment/Cooling.h"
#include "Storm/Component/Equipment/IndoorHeating.h"
#include "Storm/Component/Equipment/Off.h"
#include "Storm/Component/Equipment/Stage/BasicCooling.h"
#include "Storm/Component/Equipment/Stage/BasicIndoorHeat.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/System/Timer.h"




///
namespace Storm {
///
namespace Thermostat {


/** This concrete class is a "maker" pattern assembles the objects needed for the
    thermostat's control algorithm.
 */
class Algorithm : public Cpl::Dm::MailboxServer,
    public Cpl::Itc::CloseSync,
    public Cpl::System::Timer
{
protected:
    /// Component
    Storm::Component::IdtSelection                      m_idtSelection;

    /// Component
    Storm::Component::OperatingMode                     m_operatingMode;

    /// Component
    Storm::Component::PiPreProcess                      m_piPreProcess;

    /// Component
    Storm::Component::Pi                                m_pi;

    /// Component
    Storm::Component::Control                           m_controlCooling;

    /// Component
    Storm::Component::Control                           m_controlIdHeating;

    /// Component
    Storm::Component::Control                           m_controlOff;

    /// Component
    Storm::Component::FanControl                        m_fanControl;

    /// Component
    Storm::Component::AirFilterMonitor                  m_airFilterMonitor;

    /// Component
    Storm::Component::HvacRelayOutputs                  m_hvacRelayOutputs;

    /// Equipment
    Storm::Component::Equipment::Cooling                m_equipmentCooling;

    /// Equipment
    Storm::Component::Equipment::IndoorHeating          m_equipmentIndoorHeating;

    /// Equipment
    Storm::Component::Equipment::Off                    m_equipmentOff;

    /// Cooling stage
    Storm::Component::Equipment::Stage::BasicCooling    m_stage1Cooling;

    /// Indoor Heating stage
    Storm::Component::Equipment::Stage::BasicIndoorHeat m_stage1IndoorHeat;

    /// Indoor Heating stage
    Storm::Component::Equipment::Stage::BasicIndoorHeat m_stage2IndoorHeat;

    /// Indoor Heating stage
    Storm::Component::Equipment::Stage::BasicIndoorHeat m_stage3IndoorHeat;

    /// My open/closed state
    bool                                                m_opened;


public:
    /// Constructor.
    Algorithm();


public:
    /// See Cpl::Itc::OpenRequest
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg );

    /// See Cpl::Itc::CloseRequest
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg );


protected:
    /// Timer callback
    void expired( void ) noexcept;

protected:
    /// Helper method used to start the various components
    void startComponents( void ) noexcept;
    
};


};      // end namespace
};      // end namespace
#endif  // end header latch

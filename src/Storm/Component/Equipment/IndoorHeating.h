#ifndef Storm_Component_Equipment_IndoorHeating_h_
#define Storm_Component_Equipment_IndoorHeating_h_
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

#include "Storm/Component/Control.h"
#include "Storm/Component/Equipment/StageApi.h"


/** Process Variable (PV) turn-on threshold for indoor heating operation
 */
#ifndef OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_THRESHOLD  
#define OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_THRESHOLD              5.0F
#endif

/** Process Variable (PV) turn-off threshold for indoor heating operation
 */
#ifndef OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_THRESHOLD  
#define OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_THRESHOLD             0.0F
#endif

/** Process Variable (PV) turn-on threshold for 2nd stage indoor heating operation
 */
#ifndef OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_2ND_STAGE_THRESHOLD  
#define OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_2ND_STAGE_THRESHOLD    115.0F
#endif

/** Process Variable (PV) turn-off threshold for 2nd stage indoor heating operation
 */
#ifndef OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_2ND_STAGE_THRESHOLD  
#define OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_2ND_STAGE_THRESHOLD   100.0F
#endif

/** Process Variable (PV) turn-on threshold for 3rd stage indoor heating operation
 */
#ifndef OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_3RD_STAGE_THRESHOLD  
#define OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_ON_3RD_STAGE_THRESHOLD    215.0F
#endif

/** Process Variable (PV) turn-off threshold for 3rd stage indoor heating operation
 */
#ifndef OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_3RD_STAGE_THRESHOLD  
#define OPTION_STORM_COMPONENT_EQUIPMENT_INDOOR_HEATING_TURN_OFF_3RD_STAGE_THRESHOLD   200.0F
#endif

 ///
namespace Storm
{
///
namespace Component
{
///
namespace Equipment
{
/** This concrete class provides the business logic for controlling the HVAC
    system when the system is in the IndoorHeating Mode.  Specifically this class
    contain the logic how to use/manage up to 3 stages of indoor heating capacity
 */
class IndoorHeating : public Storm::Component::Control::Equipment
{
public:
    /// Constructor. 
    IndoorHeating( StageApi& firstStageIdHeating, StageApi& secondStageIdHeating, StageApi& thirdStageIdHeating );


public:
    /// See Storm::Component::Control::Equipment
    bool executeActive( Args_T& args ) noexcept;

    /// See Storm::Component::Control::Equipment
    bool executeOff( Args_T& args ) noexcept;

    /// See Storm::Component::Control::Equipment
    bool start( Cpl::System::ElapsedTime::Precision_T intervalTime ) noexcept;
    
    /// See Storm::Component::Control::Equipment
    void reset() noexcept;

    /// See Storm::Component::Control::Equipment
    Storm::Type::OperatingMode getOperatingMode() const noexcept { return Storm::Type::OperatingMode::eID_HEATING; }


protected:
    /// Control Single Staged IndoorHeating equipment.  Returns true if 'action was taken'
    bool singleStage(Args_T& args ) noexcept;

    /// Control Two Staged IndoorHeating equipment. Returns true if 'action was taken'
    bool secondStage( Args_T& args ) noexcept;

    /// Control Three Staged IndoorHeating equipment. Returns true if 'action was taken'
    bool thirdStage( Args_T& args ) noexcept;

protected:
    /// Reference to Single State Indoor Heat logic
    StageApi&  m_1Stage;

    /// Reference to Second State Indoor Heat logic
    StageApi&  m_2Stage;

    /// Reference to Third State Indoor Heat logic
    StageApi&  m_3Stage;

};



};      // end namespace
};
};
#endif  // end header latch

#ifndef Storm_Component_Control_h_
#define Storm_Component_Control_h_
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
#include "Storm/Dm/MpSystemConfig.h"
#include "Storm/Dm/MpVirtualOutputs.h"
#include "Storm/Dm/MpEquipmentBeginTimes.h"
#include "Storm/Dm/MpComfortConfig.h"
#include "Storm/Dm/MpCycleInfo.h"
#include "Storm/Dm/MpWhiteBox.h"

/// Namespaces
namespace Storm
{
/// Namespaces
namespace Component
{
/** This concrete class provides the Component wrapper that is responsible for
    determining how to control the HVAC equipment to meet the current load and/or
    user inputs.  The concrete Equipment object (provide as a constructor
    argument) implements the actual control business logic.

 */
class Control : public Base
{
public:

    /** This abstract class defines the interface for the Control Component to
        invoke the control logic/processing on HVAC system with a specific equipment
        configuration and/or operating mode (i.e. heating or cooling)
     */
    class Equipment
    {
    public:
        /** This structure holds all of the data/info/configuration/etc. that
            is needed by Equipment instances to executing their business logic.
            A struct is used to reduce the number of arguments passed to the
            various methods.
         */
        typedef struct
        {
            float                                  pvOut;                       //!< IN:  The process variable that represents the 'load' on the entire system
            Storm::Type::SystemConfig_T            systemConfig;                //!< IN:  Current System configuration (based on current operating mode)
            Cpl::System::ElapsedTime::Precision_T  currentTick;                 //!< IN:  Value represents the current time of the main() loop iteration, i.e.the time of the start of the loop iteration.
            Cpl::System::ElapsedTime::Precision_T  currentInterval;             //!< IN:  Value is the deterministic interval boundary / time of when the component is logically executing.
            Storm::Type::EquipmentTimes_T          equipmentBeginTimes;         //!< IN:  The starting time for equipment's on/off cycles
            Storm::Type::WhiteBox_T                whiteBox;                    //!< IN:  Collection of WhiteBox testing flags/settings/etc.
            Storm::Type::VirtualOutputs_T          vOutputs;                    //!< IO:  The System's HVAC output values
            Storm::Type::CycleInfo_T               cycleInfo;                   //!< IO:  Information about the current off/on cycle 
            bool                                   systemOn;                    //!< IO:  Is set to true if the system is actively conditioning the space (is NOT the same as is-the-equipment-on)
        } Args_T;

    public:
        /** This method will be called on a periodic basis (as determined by the
            calling Control Component instance) to perform active conditioning.
            This method will only be called when the current operating mode 'matches'
            the operating configuration of the Equipment instance.

            The method returns true if no error(s) occurred; else false is returned.
            Note: If false is return, then the Component's MP HVAC outputs will be
                  updated to an "all off state" (the Outdoor SOV output will
                  not be changed if/when this happens).
         */
        virtual bool executeActive( Args_T& args ) noexcept = 0;

        /** This method will be called on a periodic basis (as determined by the
            calling Control Component instance) to perform active conditioning.
            This method will only be called when the current operating mode DOES
            NOT MATCH the operating configuration of the Equipment instance.

            The method returns true if no error(s) occurred; else false is returned.

            Note: This method NEVER updates the Component's MP HVAC outputs.
         */
        virtual bool executeOff( Args_T& args ) noexcept = 0;

        /** This method completes any/all initialization for the Equipment
            instance.  The method is called when the containing Component instance's
            start() method is called. This method return true when successfully;
            else false is returned.
         */
        virtual bool start( Cpl::System::ElapsedTime::Precision_T& intervalTime ) noexcept = 0;

        /** This method is used to 'reset' the equipment.  This method is intended
            to be called whenever the System Type changes
         */
        virtual void reset() noexcept = 0;

        /** Returns the Equipment instance's operating mode
         */
        virtual Storm::Type::OperatingMode getOperatingMode() const noexcept = 0;

    public:
        /// Virtual destructor
        virtual ~Equipment() {}
    };

public:
    /// Input Model Points
    struct Input_T
    {
        Storm::Dm::MpSystemConfig*          systemConfig;           //!< Current system configuration based on equipment and current operating mode
        Cpl::Dm::Mp::Float*                 pvOut;                  //!< Output of the PI Controller.  This is unit-less positive number that ranges from 0.0 to piConstants.maxPvOut
        Storm::Dm::MpVirtualOutputs*        vOutputs;               //!< The virtual system outputs
        Storm::Dm::MpEquipmentBeginTimes*   equipmentBeginTimes;    //!< The begin times for when the HVAC outputs turned on/off
        Cpl::Dm::Mp::Bool*                  systemOn;               //!< Indicates that system is actively Cooling or Heating.  Note: this is not the same thing as the equipment is physically on, e.g I am actively conditioning the space - but currently in an off cycle
        Storm::Dm::MpCycleInfo*             cycleInfo;              //!< Information (typically used for debugging) about the current on/off cycling
        Cpl::Dm::Mp::Bool*                  operatingModeChanged;   //!< When true, indicates that the operating mode changed during the processing; else the output is set to false
        Storm::Dm::MpWhiteBox*              whiteBox;               //!< White Box testing flags/settings/etc.
    };


    /// Output Model Points
    struct Output_T
    {
        Storm::Dm::MpVirtualOutputs*        vOutputs;               //!< The virtual system outputs
        Storm::Dm::MpCycleInfo*             cycleInfo;              //!< Information about the current on/off cycling
        Cpl::Dm::Mp::Bool*                  systemOn;               //!< Indicates that system is actively Cooling or Heating.  Note: this is not the same thing as the equipment is physically on, e.g I am actively conditioning the space - but currently in an off cycle
    };


public:
    /// Constructor
    Control( Equipment& controlLogic, struct Input_T ins, struct Output_T outs );

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

    /// The Control logic
    Equipment&      m_equipment;

    /// Used detect changes in the System configuration
    uint16_t        m_sysCfgSequenceNumber;
};



};      // end namespace
};      // end namespace
#endif  // end header latch

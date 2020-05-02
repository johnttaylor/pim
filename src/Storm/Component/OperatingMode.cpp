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


#include "OperatingMode.h"
#include "Storm/Dm/MpVirtualOutputs.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Assert.h"


#define SECT_                   "Storm::Component::OperatingMode"


/// Namespaces
using namespace Storm::Component;


///////////////////////////////
OperatingMode::OperatingMode( struct Input_T ins, struct Output_T outs )
    : m_in( ins )
    , m_out( outs )
    , m_prevOperatingMode( Storm::Type::OperatingMode::eUNKNOWN )
    , m_equipCfgSequenceNumber( Cpl::Dm::ModelPoint::SEQUENCE_NUMBER_UNKNOWN )
    , m_comfortCfgSequenceNumber( Cpl::Dm::ModelPoint::SEQUENCE_NUMBER_UNKNOWN )
    , m_inAuto( false )
    , m_forcedOff( false )
{
    CPL_SYSTEM_ASSERT( m_in.comfortConfig );
    CPL_SYSTEM_ASSERT( m_in.equipmentBeginTimes );
    CPL_SYSTEM_ASSERT( m_in.equipmentConfig);
    CPL_SYSTEM_ASSERT( m_in.idt);
    CPL_SYSTEM_ASSERT( m_in.setpoints);
    CPL_SYSTEM_ASSERT( m_in.systemForcedOffRefCnt);
    CPL_SYSTEM_ASSERT( m_in.systemOn);
    CPL_SYSTEM_ASSERT( m_in.userMode );
    CPL_SYSTEM_ASSERT( m_out.noActiveConditioningAlarm );
    CPL_SYSTEM_ASSERT( m_out.operatingModeChanged );
    CPL_SYSTEM_ASSERT( m_out.pulseResetPi);
    CPL_SYSTEM_ASSERT( m_out.systemConfig);
    CPL_SYSTEM_ASSERT( m_out.systemForcedOffRefCnt);
    CPL_SYSTEM_ASSERT( m_out.userConfigModeAlarm);
}

bool OperatingMode::start( Cpl::System::ElapsedTime::Precision_T intervalTime )
{
    // Initialize my data
    m_prevOperatingMode  = Storm::Type::OperatingMode::eUNKNOWN;
    m_inAuto             = false;

    // Initialize parent class
    return Base::start( intervalTime );
}


///////////////////////////////
bool OperatingMode::execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                             Cpl::System::ElapsedTime::Precision_T currentInterval
)
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    //--------------------------------------------------------------------------
    // Pre-Algorithm processing
    //--------------------------------------------------------------------------

    // Get my inputs
    bool                                  badInputs         = false;
    float                                 idt               = 0.0F; // Default to 'bad value'
    float                                 heatSetpt         = 0.0F;
    float                                 coolSetpt         = 0.0F;
    bool                                  systemOn          = false;
    Storm::Dm::MpEquipmentConfig::Data    equipmentCfg      = { 0, };
    Storm::Type::EquipmentTimes_T         equipmentTimes    = { 0, };
    Storm::Type::ThermostatMode           userMode          = Storm::Type::ThermostatMode::eOFF;
    Storm::Type::ComfortConfig_T          comfortConfig     = { 0, };
    int8_t                                validIdt          = m_in.idt->read( idt );
    int8_t                                validUserMode     = m_in.userMode->read( userMode );
    int8_t                                validSetpoints    = m_in.setpoints->read( coolSetpt, heatSetpt );
    int8_t                                validSystemOn     = m_in.systemOn->read( systemOn );
    int8_t                                validEquipTimes   = m_in.equipmentBeginTimes->read( equipmentTimes );
    int8_t                                validComfort      = m_in.comfortConfig->read( comfortConfig );
    int8_t                                validEquipment    = m_in.equipmentConfig->read( equipmentCfg );
    if ( Cpl::Dm::ModelPoint::IS_VALID( validIdt ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( validUserMode ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( validSetpoints ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( validSystemOn ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( validEquipTimes ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( validComfort ) == false ||
         Cpl::Dm::ModelPoint::IS_VALID( validEquipment ) == false )
    {
        badInputs = true;
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "OperatingMode::execute. One or more invalid MPs (idt=%d, userMode=%d, setpts=%d, sysOn=%d, equipTimes=%d, comfort=%d, equipCfg=%d", validIdt, validUserMode, validSetpoints, validSystemOn, validEquipTimes, validComfort, validEquipment ) );
    }

    // Default the output value(s)
    m_out.operatingModeChanged->write( false );

    // Housekeeping
    bool haveHeatPump = false;
    bool haveCooling  = false;
    bool haveHeating  = false;


    //--------------------------------------------------------------------------
    // Algorithm processing
    //--------------------------------------------------------------------------


    // Force the system off if/when my inputs are bad
    if ( badInputs )
    {
        setNewOperatingMode( Storm::Type::OperatingMode::eOFF, haveHeatPump, equipmentCfg, comfortConfig );
        setNoActiveConditioningAlarm( true );
    }

    // My Inputs are valid....
    else
    {
        // Determine basic system configuration, i.e. do I have active heating and/or cooling
        determineAllowedModes( equipmentCfg, haveCooling, haveHeating, haveHeatPump );

        // If I am in the forced-off state -->Set my mode to OFF (and do nothing else)
        // NOTE: This check must be done AFTER the determineAllowedModes() call since 
        //       that method can force the system off (with a noActiveConditioningAlarm)
        uint32_t forceOffRefCnt = 0;
        int8_t   validForceOff  = m_in.systemForcedOffRefCnt->read( forceOffRefCnt );
        if ( !Cpl::Dm::ModelPoint::IS_VALID( validForceOff ) || forceOffRefCnt > 0 )
        {
            setNewOperatingMode( Storm::Type::OperatingMode::eOFF, haveHeatPump, equipmentCfg, comfortConfig );
        }

        //
        // Determine current Operating Mode
        //
        else
        {
            bool alarmActive = false;

            // 'correct' the user mode based on what the system is capable of
            if ( userMode == +Storm::Type::ThermostatMode::eCOOLING && !haveCooling )
            {
                userMode    = Storm::Type::ThermostatMode::eOFF;
                alarmActive = true;
            }
            else if ( ( userMode == +Storm::Type::ThermostatMode::eHEATING || userMode == +Storm::Type::ThermostatMode::eID_HEATING ) && !haveHeating )
            {
                userMode    = Storm::Type::ThermostatMode::eOFF;
                alarmActive = true;
            }
            else if ( userMode == +Storm::Type::ThermostatMode::eAUTO && ( !haveCooling || !haveHeating ) )
            {
                if ( haveCooling )
                {
                    userMode = Storm::Type::ThermostatMode::eCOOLING;
                }
                else
                {
                    userMode = Storm::Type::ThermostatMode::eHEATING;
                }
            }

            // Throw an alarm if the system has been forced off due to the 'bad' user/system modes
            if ( alarmActive )
            {
                m_out.userConfigModeAlarm->setAlarm( true, true );
            }
            else
            {
                m_out.userConfigModeAlarm->setAlarm( false );
            }

            // Convert the User Thermostat mode to Operating mode
            switch ( userMode )
            {
                // COOLING
            case Storm::Type::ThermostatMode::eCOOLING:
                m_inAuto = false;
                setNewOperatingMode( Storm::Type::OperatingMode::eCOOLING, haveHeatPump, equipmentCfg, comfortConfig );
                break;

                // HEATING
            case Storm::Type::ThermostatMode::eHEATING:
                m_inAuto = false;
                setNewOperatingMode( Storm::Type::OperatingMode::eHEATING, haveHeatPump, equipmentCfg, comfortConfig );
                break;

            case Storm::Type::ThermostatMode::eID_HEATING:
                m_inAuto = false;
                setNewOperatingMode( Storm::Type::OperatingMode::eID_HEATING, haveHeatPump, equipmentCfg, comfortConfig );
                break;

                // Resolve AUTO mode
            case Storm::Type::ThermostatMode::eAUTO:
                // Trap first time through
                if ( !m_inAuto )
                {
                    m_inAuto = true;
                    if ( idt <= heatSetpt )
                    {
                        setNewOperatingMode( Storm::Type::OperatingMode::eHEATING, haveHeatPump, equipmentCfg, comfortConfig );
                    }
                    else
                    {
                        setNewOperatingMode( Storm::Type::OperatingMode::eCOOLING, haveHeatPump, equipmentCfg, comfortConfig );
                    }
                }

                // Nominal path
                else
                {
                    static const Cpl::System::ElapsedTime::Precision_T timeHysteresis = { OPTION_STORM_COMPONENT_OPERATING_MODE_SECONDS_HYSTERESIS, 0 };

                    // Only switch modes if the system has been off for at least N seconds.
                    if ( systemOn == false && Cpl::System::ElapsedTime::expiredPrecision( equipmentTimes.systemBeginOffTime, timeHysteresis, currentInterval ) )
                    {
                        if ( idt >= coolSetpt - OPTION_STORM_COMPONENT_OPERATING_MODE_COOLING_OFFSET )
                        {
                            setNewOperatingMode( Storm::Type::OperatingMode::eCOOLING, haveHeatPump, equipmentCfg, comfortConfig );
                        }
                        else
                        {
                            setNewOperatingMode( Storm::Type::OperatingMode::eHEATING, haveHeatPump, equipmentCfg, comfortConfig );
                        }
                    }
                }
                break;


                // OFF mode (and any invalid mode settings)
            default:
                m_inAuto = false;
                setNewOperatingMode( Storm::Type::OperatingMode::eOFF, haveHeatPump, equipmentCfg, comfortConfig );
                break;
            }
        }
    }

    // If I get here -->everything worked!
    return true;
}

/////////////////////////////////////
void OperatingMode::setSystemConfig( Storm::Type::OperatingMode newOpMode, Storm::Type::SystemConfig_T& sysCfg, const Storm::Dm::MpEquipmentConfig::Data& equipmentCfg, const Storm::Type::ComfortConfig_T& comfortCfg )
{
    // Housekeeping
    sysCfg.currentOpMode       = newOpMode;
    sysCfg.outdoorUnitType     = equipmentCfg.oduType;
    sysCfg.indoorUnitType      = equipmentCfg.iduType;
    sysCfg.fanContinuousSpeed  = equipmentCfg.hasVspBlower == false ? STORM_DM_MP_VIRTUAL_OUTPUTS_ON : ( uint16_t) ( STORM_DM_MP_VIRTUAL_OUTPUTS_ON * OPTION_STORM_DEFAULT_VSP_BLOWER_FAN_CONT_SPEED );
    uint16_t i;
    float    pv;

    // Use the current operating mode to populate the system configuration
    switch ( newOpMode )
    {
        // COOLING.  Discrete stages/speeds, support for VSP blower
    case Storm::Type::OperatingMode::eCOOLING:
        sysCfg.numCompressorStages = equipmentCfg.numCompStages;
        sysCfg.numIndoorStages     = 0;
        sysCfg.totalStages         = equipmentCfg.numCompStages;
        sysCfg.gain                = OPTION_STORM_PI_CONSTANTS_COOLING_NORMAL_GAIN;
        sysCfg.reset               = OPTION_STORM_PI_CONSTANTS_COOLING_NORMAL_RESET;
        for ( i=0, pv=0.0F; i < sysCfg.numCompressorStages; i++, pv += OPTION_STORM_COOLING_LV_PER_STAGE )
        {
            sysCfg.stages[i].cph          = comfortCfg.compressorCooling.cph;
            sysCfg.stages[i].minOffTime   = comfortCfg.compressorCooling.minOffTime;
            sysCfg.stages[i].minOnTime    = comfortCfg.compressorCooling.minOnTime;
            sysCfg.stages[i].lowerBound   = pv;
            sysCfg.stages[i].upperBound   = pv + OPTION_STORM_COOLING_LV_PER_STAGE;
            sysCfg.maxPvOut               = sysCfg.stages[i].upperBound;
            sysCfg.stages[i].minIndoorFan = equipmentCfg.hasVspBlower == false ? STORM_DM_MP_VIRTUAL_OUTPUTS_ON : ( uint16_t) ( STORM_DM_MP_VIRTUAL_OUTPUTS_ON * OPTION_STORM_DEFAULT_VSP_BLOWER_COOLING_SPEED );
            sysCfg.stages[i].maxIndoorFan = sysCfg.stages[i].minIndoorFan;
        }
        break;

        // HEATING. Indoor only heat (Furnace and AirHandler)
    case Storm::Type::OperatingMode::eID_HEATING:
        sysCfg.numCompressorStages = 0;
        sysCfg.numIndoorStages     = equipmentCfg.numIduHeatingStages;
        sysCfg.totalStages         = equipmentCfg.numIduHeatingStages;
        sysCfg.gain                = OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_GAIN;
        sysCfg.reset               = OPTION_STORM_PI_CONSTANTS_HEATING_NORMAL_RESET;
        for ( i=0, pv=0.0F; i < sysCfg.numIndoorStages; i++, pv += OPTION_STORM_HEATING_LV_PER_STAGE )
        {
            sysCfg.stages[i].cph          = comfortCfg.indoorHeating.cph;
            sysCfg.stages[i].minOffTime   = comfortCfg.indoorHeating.minOffTime;
            sysCfg.stages[i].minOnTime    = comfortCfg.indoorHeating.minOnTime;
            sysCfg.stages[i].lowerBound   = pv;
            sysCfg.stages[i].upperBound   = pv + OPTION_STORM_HEATING_LV_PER_STAGE;
            sysCfg.maxPvOut               = sysCfg.stages[i].upperBound;
            sysCfg.stages[i].minIndoorFan = equipmentCfg.iduType == Storm::Type::IduType::eFURNACE ? STORM_DM_MP_VIRTUAL_OUTPUTS_OFF : STORM_DM_MP_VIRTUAL_OUTPUTS_ON;
            sysCfg.stages[i].maxIndoorFan = sysCfg.stages[i].minIndoorFan;
        }
        break;

        // OFF mode (and any invalid mode settings)
    default:
        Storm::Dm::MpSystemConfig::setToOff( sysCfg );
        break;
    }
}

void OperatingMode::determineAllowedModes( const Storm::Dm::MpEquipmentConfig::Data& equipmentCfg, bool& haveCooling, bool& haveHeating, bool& haveHeatPump )
{

    if ( equipmentCfg.oduType == Storm::Type::OduType::eAC )
    {
        if ( equipmentCfg.numCompStages > 0 )
        {
            haveCooling  = true;
        }
    }
    else if ( equipmentCfg.oduType == Storm::Type::OduType::eHP )
    {
        if ( equipmentCfg.numCompStages > 0 )
        {
            haveCooling  = true;
            haveHeating  = true;
            haveHeatPump = true;
        }
    }

    if ( equipmentCfg.numIduHeatingStages > 0 )
    {
        haveHeating = true;
    }


    // Trap/trigger no-active conditioning alarm
    setNoActiveConditioningAlarm( !haveCooling && !haveHeating );
}

void OperatingMode::setNoActiveConditioningAlarm( bool alarmIsActive ) noexcept
{
    if ( alarmIsActive )
    {
        m_out.noActiveConditioningAlarm->setAlarm( true, true );
        if ( m_forcedOff == false )
        {
            m_out.systemForcedOffRefCnt->increment();
            m_forcedOff = true;
        }
    }
    else
    {
        m_out.noActiveConditioningAlarm->setAlarm( false );
        if ( m_forcedOff == true )
        {
            m_out.systemForcedOffRefCnt->decrement();
            m_forcedOff = false;
        }
    }
}

void OperatingMode::setNewOperatingMode( Storm::Type::OperatingMode   newOpMode,
                                         bool                         haveHeatPump,
                                         const                        Storm::Dm::MpEquipmentConfig::Data& equipmentCfg,
                                         const                        Storm::Type::ComfortConfig_T& comfortCfg )
{
    // Trap the case of 'Heating' but ONLY indoor heat is available
    if ( newOpMode == +Storm::Type::OperatingMode::eHEATING && !haveHeatPump )
    {
        newOpMode = Storm::Type::OperatingMode::eID_HEATING;
    }

    // React to a change in the operating mode
    bool modeChanged = false;
    if ( newOpMode != m_prevOperatingMode )
    {
        // Set indication that the Operating mode is/was changed
        m_out.operatingModeChanged->write( true );

        // Reset PI on mode changes
        m_out.pulseResetPi->write( true );

        // Set the new mode and cache it       
        m_prevOperatingMode = newOpMode;
        modeChanged         = true;
    }

    // Update system configuration (but only on change)
    uint16_t ccSeqNum = m_in.comfortConfig->getSequenceNumber();
    uint16_t eqSeqNum = m_in.equipmentConfig->getSequenceNumber();
    if ( modeChanged || m_comfortCfgSequenceNumber != ccSeqNum || m_equipCfgSequenceNumber != eqSeqNum )
    {
        Storm::Type::SystemConfig_T sysCfg;
        setSystemConfig( newOpMode, sysCfg, equipmentCfg, comfortCfg );
        m_out.systemConfig->write( sysCfg );
        m_comfortCfgSequenceNumber = ccSeqNum;
        m_equipCfgSequenceNumber   = eqSeqNum;
    }
}


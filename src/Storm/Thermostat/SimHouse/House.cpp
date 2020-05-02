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

#include "House.h"
#include "ModelPoints.h"
#include "Storm/Thermostat/ModelPoints.h"

using namespace Storm::Thermostat::SimHouse;


#define RESISTANCE_NO_CAPACITY      50
#define RESISTANCE_COOLING_CAPCITY  (RESISTANCE_NO_CAPACITY*2.0/3.0)
#define RESISTANCE_HEATING_CAPCITY  (RESISTANCE_COOLING_CAPCITY*3.0)

//////////////////////
House::House()
    : EventLoop( OPTION_STORM_THERMOSTAT_SIM_HOUSE_SIM_TIMING_MS )       // Wake up once a second
    , m_sim( OPTION_STORM_THERMOSTAT_SIM_HOUSE_SIM_TIMING_MS / 1000.0    // Convert to seconds
             , 70.0                         // ODT
             , 120.0                        // max odt
             , -20.0                        // min odt
             , 0.33                         // ODT Cooling load rating
             , 0.80                         // ODT Heating load rating
             , RESISTANCE_NO_CAPACITY       // systemEnvResistance
             , RESISTANCE_COOLING_CAPCITY   // systemCoolingEnvResistance
             , RESISTANCE_HEATING_CAPCITY ) // systemHeatingEnvResistance
{
}


void House::appRun()
{
    startEventLoop();
    unsigned long timeMark = Cpl::System::ElapsedTime::milliseconds();

    bool run = true;
    while ( run )
    {
        run = waitAndProcessEvents();
        if ( run )
        {
            unsigned long timeNow = Cpl::System::ElapsedTime::milliseconds();
            while ( Cpl::System::ElapsedTime::deltaMilliseconds( timeMark, timeNow ) >= OPTION_STORM_THERMOSTAT_SIM_HOUSE_SIM_TIMING_MS )
            {
                timeMark += OPTION_STORM_THERMOSTAT_SIM_HOUSE_SIM_TIMING_MS;
                executeSimulation();
            }
        }
    }
}

void House::executeSimulation()
{
    float                           odt;
    Storm::Type::SystemConfig_T     sysCfg;
    Storm::Type::HvacRelayOutputs_T relays;
    bool                            simEnabled;
    if ( Cpl::Dm::ModelPoint::IS_VALID( mp_outdoorTemp.read( odt ) ) == true &&
         Cpl::Dm::ModelPoint::IS_VALID( mp_systemConfig.read( sysCfg ) ) == true &&
         Cpl::Dm::ModelPoint::IS_VALID( mp_relayOutputs.read( relays ) ) == true &&
         Cpl::Dm::ModelPoint::IS_VALID( mp_houseSimEnabled.read( simEnabled ) ) == true && simEnabled )
    {
        double capacity = 0.0;
        bool   cooling  = true;

        // Cooling capacity
        if ( sysCfg.currentOpMode == Storm::Type::OperatingMode::eCOOLING && sysCfg.numCompressorStages != 0 )
        {
            double stageCapacity = 1.0 / sysCfg.numCompressorStages;
            capacity += relays.y1 ? stageCapacity : 0.0;
            if ( sysCfg.numCompressorStages > 1 )
            {
                capacity += relays.y2 ? stageCapacity : 0.0;
            }
        }

        // Compressor + Indoor Heating capacity
        else if ( sysCfg.currentOpMode == Storm::Type::OperatingMode::eHEATING )
        {
            cooling = false;

            // HeatPump with Electric heat
            if ( sysCfg.indoorUnitType == Storm::Type::IduType::eAIR_HANDLER && ( sysCfg.numCompressorStages + sysCfg.numIndoorStages != 0) )
            {
                double stageCapacity = 1.0 / ( sysCfg.numCompressorStages + sysCfg.numIndoorStages );
                capacity += relays.y1 ? stageCapacity : 0.0;
                if ( sysCfg.numCompressorStages > 1 )
                {
                    capacity += relays.y2 ? stageCapacity : 0.0;
                }
                if ( sysCfg.numIndoorStages > 0 )
                {
                    capacity += relays.w1 ? stageCapacity : 0.0;
                }
                if ( sysCfg.numIndoorStages > 1 )
                {
                    capacity += relays.w2 ? stageCapacity : 0.0;
                }
                if ( sysCfg.numIndoorStages > 2 )
                {
                    capacity += relays.w3 ? stageCapacity : 0.0;
                }
            }

            // DUAL fuel
            else  if ( sysCfg.indoorUnitType == Storm::Type::IduType::eFURNACE && sysCfg.numCompressorStages != 0)
            {
                // TODO
            }
        }

        // Indoor Heating capacity
        else if ( sysCfg.currentOpMode == Storm::Type::OperatingMode::eID_HEATING && sysCfg.numIndoorStages != 0 )
        {
            cooling = false;
            double stageCapacity = 1.0 / sysCfg.numIndoorStages;
            capacity += relays.w1 ? stageCapacity : 0.0;
            if ( sysCfg.numIndoorStages > 1 )
            {
                capacity += relays.w2 ? stageCapacity : 0.0;
            }
            if ( sysCfg.numIndoorStages > 2 )
            {
                capacity += relays.w3 ? stageCapacity : 0.0;
            }
        }

        // Run the simulation
        double idt = m_sim.tick( odt, capacity, cooling );
        mp_primaryRawIdt.write( (float) idt );
    }
}

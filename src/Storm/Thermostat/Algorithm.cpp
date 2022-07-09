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


#include "Algorithm.h"
#include "ModelPoints.h"
#include "Logger.h"
#include "Outputs.h"
#include "Cpl/System/FatalError.h"

#define ALGORITHM_PROCESSING_INTERVAL_SEC      2


/// Namespace
using namespace Storm::Thermostat;


static const Storm::Component::IdtSelection::Input_T  idtSelection_ins_          = { &mp_primaryRawIdt, &mp_secondaryRawIdt, &mp_enabledSecondaryIdt };
static const Storm::Component::IdtSelection::Output_T idtSelection_outs_         = { &mp_activeIdt, &mp_systemForcedOffRefCnt , &mp_idtAlarms };

static const Storm::Component::OperatingMode::Input_T  operatingMode_ins_        = { &mp_setpoints,  &mp_userMode, &mp_activeIdt, &mp_equipmentBeginTimes, &mp_systemOn, &mp_systemForcedOffRefCnt, &mp_equipmentConfig, &mp_comfortConfig };
static const Storm::Component::OperatingMode::Output_T operatingMode_outs_       = { &mp_operatingModeChanged, &mp_resetPiPulse, &mp_systemForcedOffRefCnt, &mp_systemConfig, &mp_noActiveConditioningAlarm, &mp_userCfgModeAlarm };

static const Storm::Component::PiPreProcess::Input_T  piPreProcess_ins_          = { &mp_activeIdt, &mp_systemConfig, &mp_operatingModeChanged, &mp_setpoints };
static const Storm::Component::PiPreProcess::Output_T piPreProcess_outs_         = { &mp_activeSetpoint, &mp_deltaIdtError, &mp_deltaSetpoint, &mp_setpointChanged };

static const Storm::Component::Pi::Input_T  pi_ins_                              = { &mp_resetPiPulse, &mp_deltaIdtError, &mp_systemConfig, &mp_freezePiRefCnt, &mp_inhibitfRefCnt };
static const Storm::Component::Pi::Output_T pi_outs_                             = { &mp_pvOut, &mp_sumError, &mp_pvInhibited };

static const Storm::Component::Control::Input_T  control_ins_                    = { &mp_systemConfig, &mp_pvOut, &mp_vOutputs, &mp_equipmentBeginTimes, &mp_systemOn, &mp_cycleInfo, &mp_operatingModeChanged, &mp_whiteBox };
static const Storm::Component::Control::Output_T control_outs_                   = { &mp_vOutputs, &mp_cycleInfo, &mp_systemOn };

static const Storm::Component::FanControl::Input_T  fanControl_ins_              = { &mp_fanMode,  &mp_systemConfig, &mp_vOutputs, &mp_equipmentBeginTimes };
static const Storm::Component::FanControl::Output_T fanControl_outs_             = { &mp_vOutputs };

static const Storm::Component::AirFilterMonitor::Input_T  airFilterMonitor_ins_  = { &mp_maxAirFilterHours,  &mp_airFilterOperationTime, &mp_vOutputs, &mp_airFilterAlert };
static const Storm::Component::AirFilterMonitor::Output_T airFilterMonitor_outs_ = { &mp_airFilterAlert, &mp_airFilterOperationTime };

static const Storm::Component::HvacRelayOutputs::Input_T  hvavRelayOutputs_ins_  = { &mp_vOutputs, &mp_equipmentBeginTimes, &mp_systemForcedOffRefCnt, &mp_systemOn };
static const Storm::Component::HvacRelayOutputs::Output_T hvavRelayOutputs_outs_ = { &mp_equipmentBeginTimes, &mp_relayOutputs };

///////////////////////////////
// Anonymous namespace
namespace
{
/* NOTE: This class is used to 'self-time' the post processing for each
         algorithm cycle
*/
class PostProcessing : public Storm::Component::Base
{
public:
    /// Constructor
    PostProcessing() {};

protected:
    /// See Storm::Component::Base
    bool execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                  Cpl::System::ElapsedTime::Precision_T currentInterval )
    {
        // Update the physical HVAC outputs
        Outputs::updateHVACOutputs();

        // Log my current state
        Logger::recordSystemData( currentInterval );

        // Clear any/all WhiteBox 'Pulse' flags
        mp_whiteBox.resetPulseSettings();
        return true;
    }
};
};

static PostProcessing postProcessor_;

///////////////////////////////
Algorithm::Algorithm()
    : Cpl::Dm::MailboxServer( OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD )
    , Cpl::Itc::CloseSync( *( ( Cpl::Itc::PostApi* )this ) )
    , Cpl::System::Timer( *( ( Cpl::System::TimerManager* )this ) )
    , m_idtSelection( idtSelection_ins_, idtSelection_outs_ )
    , m_operatingMode( operatingMode_ins_, operatingMode_outs_ )
    , m_piPreProcess( piPreProcess_ins_, piPreProcess_outs_ )
    , m_pi( pi_ins_, pi_outs_ )
    , m_controlCooling( m_equipmentCooling, control_ins_, control_outs_ )
    , m_controlIdHeating( m_equipmentIndoorHeating, control_ins_, control_outs_ )
    , m_controlOff( m_equipmentOff, control_ins_, control_outs_ )
    , m_fanControl( fanControl_ins_, fanControl_outs_ )
    , m_airFilterMonitor( airFilterMonitor_ins_, airFilterMonitor_outs_ )
    , m_hvacRelayOutputs( hvavRelayOutputs_ins_, hvavRelayOutputs_outs_ )
    , m_equipmentCooling( m_stage1Cooling )
    , m_equipmentIndoorHeating( m_stage1IndoorHeat, m_stage2IndoorHeat, m_stage3IndoorHeat )
    , m_equipmentOff()
    , m_stage1Cooling( 0, 0 )
    , m_stage1IndoorHeat( 0, 0 )
    , m_stage2IndoorHeat( 1, 1 )
    , m_stage3IndoorHeat( 2, 2 )
    , m_opened( false )
{
}




///////////////////////////////
void Algorithm::startComponents( void ) noexcept
{
    Cpl::System::ElapsedTime::Precision_T time = { ALGORITHM_PROCESSING_INTERVAL_SEC, 0 };

    m_idtSelection.start( time );
    m_operatingMode.start( time );
    m_piPreProcess.start( time );
    m_pi.start( time );
    m_controlCooling.start( time );
    m_controlIdHeating.start( time );
    m_controlOff.start( time );
    m_fanControl.start( time );
    m_airFilterMonitor.start( time );
    m_hvacRelayOutputs.start( time );

    postProcessor_.start( time );
}


void Algorithm::expired( void ) noexcept
{
    // Get the current time
    Cpl::System::ElapsedTime::Precision_T startTime = Cpl::System::ElapsedTime::precision();

    // Reset all Pulse MPs
    mp_resetPiPulse.write( false );

    // Execute the algorithm
    bool success = true;
    success &= m_idtSelection.doWork( success, startTime );
    success &= m_operatingMode.doWork( success, startTime );
    success &= m_piPreProcess.doWork( success, startTime );
    success &= m_pi.doWork( success, startTime );
    success &= m_controlCooling.doWork( success, startTime );
    success &= m_controlIdHeating.doWork( success, startTime );
    success &= m_controlOff.doWork( success, startTime );
    success &= m_fanControl.doWork( success, startTime );
    success &= m_airFilterMonitor.doWork( success, startTime );
    success &= m_hvacRelayOutputs.doWork( success, startTime );

    success &= postProcessor_.doWork( success, startTime );


    // Something broke!!  Not sure what make the most sense here -->for now throw a fatal error
    if ( !success )
    {
        Cpl::System::FatalError::logf( "Algorithm::executeAlgorithm: One or more Components failed! (time=%lu.%u", startTime.m_seconds, startTime.m_thousandths );
    }

    // Restart my interval timer at 1/10 of the actual interval.  The 
    // Components 'time themselves' - so I just need to call the components 
    // often enough to be close-enough to their true interval timing
    Timer::start( ( ALGORITHM_PROCESSING_INTERVAL_SEC * 1000 ) / 100 );
}

///////////////////////////////
void Algorithm::request( Cpl::Itc::OpenRequest::OpenMsg& msg )
{
    if ( !m_opened )
    {
        m_opened = true;
        mp_loopCounter.write( 0 );
        startComponents();
        expired();  // Run the algorithm the first time
    }

    msg.returnToSender();
}

void Algorithm::request( Cpl::Itc::CloseRequest::CloseMsg& msg )
{
    if ( m_opened )
    {
        m_opened = false;
        Timer::stop();
    }

    msg.returnToSender();
}




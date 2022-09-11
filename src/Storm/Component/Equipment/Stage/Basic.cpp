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


#include "Basic.h"
#include "Cpl/System/Assert.h"
#include "Storm/Utils/DutyCycle.h"


/// Namespaces
using namespace Storm::Component::Equipment::Stage;



///////////////////////////////
Basic::Basic( unsigned systemStageIndex, unsigned outputStageIndex )
    : m_systemIndex( systemStageIndex )
    , m_outIndex( outputStageIndex )
    , m_startTime( { 0,0 } )
    , m_args( 0 )
    , m_nextStage( 0 )
    , m_prevStage( 0 )
    , m_startInOffCycle( false )
    , m_supplemented( false )
    , m_requestSupplementStartInOnCycle( true )
    , m_requestOffStartInOnCycle( true )
{
}

void Basic::reconfigure(  unsigned systemStageIndex, unsigned outputStageIndex ) noexcept
{
    m_systemIndex  = systemStageIndex;
    m_outIndex     = outputStageIndex;
}

///////////////////////////////
void Basic::requestOn( Storm::Component::Control::Equipment::Args_T& args, bool startInOnCycle ) noexcept
{
    m_args            = &args;
    m_startInOffCycle = !startInOnCycle;
    m_supplemented    = false;
    generateEvent( Fsm_evOnRequest );
}

void Basic::requestAsSupplement( Storm::Component::Control::Equipment::Args_T& args, StageApi& nextStage, bool startNextStageInOnCycle ) noexcept
{
    m_args                            = &args;
    m_requestSupplementStartInOnCycle = startNextStageInOnCycle;
    m_nextStage                       = &nextStage;
    generateEvent( Fsm_evNeedMoreCapacity );
}

void Basic::requestModeToOff() noexcept
{
    generateEvent( Fsm_evOffModeRequest );
}

void Basic::requestOff( Storm::Component::Control::Equipment::Args_T& args, bool startLowerStageInOnCycle ) noexcept
{
    m_args                     = &args;
    m_requestOffStartInOnCycle = startLowerStageInOnCycle;
    generateEvent( Fsm_evExcessCapacity );
}

void Basic::execute( Storm::Component::Control::Equipment::Args_T& args ) noexcept
{
    m_args = &args;
    generateEvent( FSM_NO_MSG );
}

void Basic::notifyAsActiveStage_( Storm::Component::Control::Equipment::Args_T& args, StageApi& previousStage, bool startInOnCycle ) noexcept
{
    m_args            = &args;
    m_startInOffCycle = !startInOnCycle;
    m_supplemented    = true;
    m_nextStage       = 0;
    m_prevStage       = &previousStage;
    generateEvent( Fsm_evOnRequest );
}

void Basic::notifyAsExitingSupplmenting_( Storm::Component::Control::Equipment::Args_T& args, bool startInOnCycle ) noexcept
{
    m_args            = &args;
    m_startInOffCycle = !startInOnCycle;
    m_nextStage       = 0;
    generateEvent( Fsm_evLessCapacityNeeded );
}


///////////////////////////////
void Basic::initializeStage() noexcept
{
    m_startInOffCycle                 = false;
    m_supplemented                    = false;
    m_nextStage                       = 0;
    m_prevStage                       = 0;
    m_args                            = 0;
    m_requestOffStartInOnCycle        = true;
    m_requestSupplementStartInOnCycle = true;
    m_startTime                       = { 0, 0 };
}

void Basic::enterSupplementing() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );
    CPL_SYSTEM_ASSERT( m_nextStage );

    m_nextStage->notifyAsActiveStage_( *m_args, *this, m_requestSupplementStartInOnCycle );
    stageOn();
}


void Basic::notifyLowerStage() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );
    CPL_SYSTEM_ASSERT( m_prevStage );

    m_prevStage->notifyAsExitingSupplmenting_( *m_args, m_requestOffStartInOnCycle );
    m_prevStage    = 0;
    m_supplemented = false;
}

void Basic::startOnTime() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );
    m_startTime = m_args->currentInterval;
}

void Basic::startOffTime() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );
    m_startTime = m_args->currentInterval;
}

void Basic::initializeBackTransition() noexcept
{
    // Perform an immediate transition
    generateEvent( Fsm_evBackTransitionCompleted );
}

void Basic::initializeFromTransition() noexcept
{
    // Perform an immediate transition
    generateEvent( Fsm_evFromTransitionCompleted );
}

void Basic::startingStageOff() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );

    // Turn off the stage and Update the cycle info's starting time
    stageOff();
    m_args->cycleInfo.beginOffTime = m_startTime;
    m_args->cycleInfo.mode         = Storm::Type::CycleStatus::eOFF_CYCLE;

    // Calculate (and check) the current OFF cycle time
    checkOffTime();

    // Transition directly to the OffTime state (i.e. no fan-off delays supported)
    generateEvent( Fsm_evStartingOffTimeExpired );
}

void Basic::startingStageOn() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );

    // Turn on the stage and Update the cycle info's starting time
    stageOn();
    m_args->cycleInfo.beginOnTime = m_startTime;
    m_args->cycleInfo.mode        = Storm::Type::CycleStatus::eON_CYCLE;

    // Calculate (and check) the current ON cycle time
    checkOnTime();

    // Transition directly to the OnTime state (i.e. no fan-on delays supported)
    generateEvent( Fsm_evStartingOnTimeExpired );
}

void Basic::startCyclingInOffCycle() noexcept
{
    // No action needed/required
}

void Basic::startCyclingInOnCycle() noexcept
{
    // No action needed/required
}

void Basic::shutdownStage() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );
 
    stageOff();
    if ( m_systemIndex == 0 )
    {
        m_args->systemOn       = false;
        m_args->cycleInfo.mode = Storm::Type::CycleStatus::eOFF;
    }
}

void Basic::checkBackTransition() noexcept
{
    // Do nothing -->this action should never be called because we immediately transition to the Off state
}

void Basic::checkFromTransition() noexcept
{
    // Do nothing -->this action should never be called because we immediately transition to the Cycle state
}

void Basic::checkStartingOffTime() noexcept
{
    // Do nothing -->this action should never be called because we immediately transition to the OffTime state
}

void Basic::checkStartingOnTime() noexcept
{
    // Do nothing -->this action should never be called because we immediately transition to the OnTime state
}

void Basic::exitSupplementing() noexcept
{
    // Nothing action needed/required
}

void Basic::initializeActive() noexcept
{
    stageOff();
    if ( m_systemIndex == 0 )
    {
        m_args->systemOn = true;
    }
}

///////////////////////////////
bool Basic::isStartInOffCycle() noexcept
{
    return m_startInOffCycle;
}

bool Basic::isBeingSupplemented() noexcept
{
    return m_supplemented;
}


void Basic::checkOffTime() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );

    // Prevent 'extra' time expired events when transition to an on cycle (it is artifact of the action being the state's 'do action')
    if ( m_args->cycleInfo.mode == Storm::Type::CycleStatus::eOFF_CYCLE )
    {
        // Calculate the CURRENT Off cycle time
        m_args->cycleInfo.offTime = Storm::Utils::DutyCycle::calculateOffTime( m_args->pvOut,
                                                                               m_args->systemConfig.stages[m_systemIndex].minOffTime,
                                                                               Storm::Type::Cph::_from_integral_unchecked( m_args->systemConfig.stages[m_systemIndex].cph ),
                                                                               m_args->systemConfig.stages[m_systemIndex].lowerBound,
                                                                               m_args->systemConfig.stages[m_systemIndex].upperBound );

        // Has the off time cycle expired?
        Cpl::System::ElapsedTime::Precision_T cycleTime = { m_args->cycleInfo.offTime, 0 };
        if ( Cpl::System::ElapsedTime::expiredPrecision( m_startTime, cycleTime, m_args->currentInterval ) || m_args->whiteBox.abortOnOffCycle )
        {
            m_args->whiteBox.abortOnOffCycle = false;
            generateEvent( Fsm_evOffTimeExpired );
        }
    }
}

void Basic::checkOnTime() noexcept
{
    CPL_SYSTEM_ASSERT( m_args );

    // Prevent 'extra' time expired events when transition to an off cycle (it is artifact of the action being the state's 'do action')
    if ( m_args->cycleInfo.mode == Storm::Type::CycleStatus::eON_CYCLE )
    {
        // Calculate the CURRENT On cycle time
        m_args->cycleInfo.onTime = Storm::Utils::DutyCycle::calculateOnTime( m_args->pvOut,
                                                                             m_args->systemConfig.stages[m_systemIndex].minOnTime,
                                                                             Storm::Type::Cph::_from_integral_unchecked( m_args->systemConfig.stages[m_systemIndex].cph ),
                                                                             m_args->systemConfig.stages[m_systemIndex].lowerBound,
                                                                             m_args->systemConfig.stages[m_systemIndex].upperBound );

        // Has the on time cycle expired?
        Cpl::System::ElapsedTime::Precision_T cycleTime = { m_args->cycleInfo.onTime, 0 };
        if ( Cpl::System::ElapsedTime::expiredPrecision( m_startTime, cycleTime, m_args->currentInterval ) || m_args->whiteBox.abortOnOffCycle )
        {
            m_args->whiteBox.abortOnOffCycle = false; 
            generateEvent( Fsm_evOnTimeExpired );
        }
    }
}

///////////////////////////////
bool Basic::isActive() const noexcept
{
    return isInActive();
}

bool Basic::isSupplementing() const noexcept
{
    return isInSupplementingNextStage();
}

bool Basic::isOff() const noexcept
{
    return isInOff();
}

bool Basic::isTransitioningFromLowerStage() const noexcept
{
    return isInTransitioningBackToLowerStage();
}

bool Basic::isTransitioningBackToLowerStage() const noexcept
{
    return isInTransitioningBackToLowerStage();
}

bool Basic::isOnCycle() const noexcept
{
    return isInOnCycle();
}

bool Basic::isOffCycle() const noexcept
{
    return isInOffCycle();
}


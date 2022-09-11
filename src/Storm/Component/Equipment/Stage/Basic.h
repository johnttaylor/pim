#ifndef Storm_Component_Equipment_State_Basic_h_
#define Storm_Component_Equipment_State_Basic_h_
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

#include "Storm/Component/Equipment/StageApi.h"
#include "Storm/Component/Equipment/Stage/FsmEventQueue_.h"


/// Namespaces
namespace Storm {
/// Namespaces
namespace Component {
/// Namespaces
namespace Equipment {
/// Namespaces
namespace Stage {


/** This partially concrete class implements the Storm::Component::Equipment::Stage
    interface for a single stage, or multiple stages where transitions between
    stages have no restrictions other than the minimum cycle on/off times.
 */
class Basic : public Storm::Component::Equipment::StageApi, public FsmEventQueue_
{
protected:
    /// Constructor
    Basic( unsigned systemStageIndex, unsigned outputStageIndex );


public:
    /// See Storm::Component::Equipment::Stage
    void reconfigure( unsigned systemStageIndex, unsigned outputStageIndex ) noexcept;


public:
    /// See Storm::Component::Equipment::Stage
    void requestOn( Storm::Component::Control::Equipment::Args_T& args, bool startInOnCycle=true ) noexcept;

    /// See Storm::Component::Equipment::Stage
    void requestAsSupplement( Storm::Component::Control::Equipment::Args_T& args, StageApi& nextStage, bool startNextStageInOnCycle=true ) noexcept;

    /// See Storm::Component::Equipment::Stage
    void requestOff( Storm::Component::Control::Equipment::Args_T& args, bool startLowerStageInOnCycle=true ) noexcept;

    /// See Storm::Component::Equipment::Stage
    void requestModeToOff() noexcept;

    /// See Storm::Component::Equipment::Stage
    void execute( Storm::Component::Control::Equipment::Args_T& args ) noexcept;

public:
    /// See Storm::Component::Equipment::Stage
    bool isActive() const noexcept;

    /// See Storm::Component::Equipment::Stage
    bool isSupplementing() const noexcept;

    /// See Storm::Component::Equipment::Stage
    bool isOff() const noexcept;

    /// See Storm::Component::Equipment::Stage
    bool isTransitioningFromLowerStage() const noexcept;

    /// See Storm::Component::Equipment::Stage
    bool isTransitioningBackToLowerStage() const noexcept;

    /// See Storm::Component::Equipment::Stage
    bool isOnCycle() const noexcept;

    /// See Storm::Component::Equipment::Stage
    bool isOffCycle() const noexcept;

protected:
    /// See Storm::Component::Equipment::Stage
    void notifyAsActiveStage_( Storm::Component::Control::Equipment::Args_T& args, StageApi& previousStage, bool startInOnCycle ) noexcept;

    /// See Storm::Component::Equipment::Stage
    void notifyAsExitingSupplmenting_( Storm::Component::Control::Equipment::Args_T& args, bool startInOnCycle ) noexcept;


public:
    /// Action. Notifies the next higher stage AND calls stageOn()
    void enterSupplementing() noexcept;

    /// Action.  Empty method (does nothing)
    void exitSupplementing() noexcept;

    /// Action. Performs an immediate transition to the Off state
    void initializeBackTransition() noexcept;

    /// Action. Performs an immediate transition to the Cycling state
    void initializeFromTransition() noexcept;

    /// Action. Resets the stage's internal variables/flags to the default state
    void initializeStage() noexcept;

    /// Action. Notifies the previous lower stage (and housekeeping)
    void notifyLowerStage() noexcept;

    /// Action. Captures the starting time
    void startOnTime() noexcept;

    /// Action. Captures the starting time
    void startOffTime() noexcept;

    /// Action. Immediately transitions to the OffTime state and turns the stage off
    void startingStageOff() noexcept;

    /// Action. Immediately transitions to the OnTime state and turns the stage on
    void startingStageOn() noexcept;

    /// Action.  Empty method (does nothing)
    void startCyclingInOffCycle() noexcept;

    /// Action.  Empty method (does nothing)
    void startCyclingInOnCycle() noexcept;

    /// Action.  Simply calls stageOff()
    void shutdownStage() noexcept;

    /// Action.  Empty method (does nothing) since the default behavior is to skip the 'starting' state
    void checkStartingOffTime() noexcept;

    /// Action.  Empty method (does nothing) since the default behavior is to skip the 'starting' state
    void checkStartingOnTime() noexcept;

    /// Action.  Simply calls stageOff()
    void initializeActive() noexcept;

    /// Action.  Empty method (does nothing) since the default behavior is to skip the 'transition' state
    void checkBackTransition() noexcept;

    /// Action.  Empty method (does nothing) since the default behavior is to skip the 'transition' state
    void checkFromTransition() noexcept;

    /// Action.  Calculates and checks if the Off Cycle time has expired
    void checkOffTime() noexcept;

    /// Action.  Calculates and checks if the ON Cycle time has expired
    void checkOnTime() noexcept;


public:
    /// Guard
    bool isBeingSupplemented() noexcept;

    /// Guard
    bool isStartInOffCycle() noexcept;


protected:
    /** Index for the stage's index into the SystemConfig stage array
        For example a 2 stage HP with a 1 stage furnace in Heating mode: 
            m_systemIndex 0 = 1st stage compressor, 
            m_systemIndex 1 = 2nd compressor stage, 
            m_systemIndex 2 = 1st furnace stage
     */
    unsigned                                        m_systemIndex;

    /// Index for the stage's Virtual HVAC output for the indoorStages[] OR outdoorStages[] output arrays
    unsigned                                        m_outIndex;

    /// The captured/cached starting time
    Cpl::System::ElapsedTime::Precision_T           m_startTime;

    /// Current Equipment arguments
    Storm::Component::Control::Equipment::Args_T*   m_args;

    /// The next higher stage to notify
    StageApi*                                       m_nextStage;

    /// The previous lower stage to notify
    StageApi*                                       m_prevStage;

    /// Flag is true when the stage has been requested to start in OFF cycle
    bool                                            m_startInOffCycle;

    /// Flag is true when the stage is being supplemented
    bool                                            m_supplemented;

    /// startInOnCycle flag from the last requestAsSupplement() call
    bool                                            m_requestSupplementStartInOnCycle;

    /// startInOnCycle flag from the last requestOff() call
    bool                                            m_requestOffStartInOnCycle;

};


};      // end namespace
};
};
};
#endif  // end header latch

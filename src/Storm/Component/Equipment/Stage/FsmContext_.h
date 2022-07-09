#ifndef Storm_Component_Equipment_Stage_FsmContext_x_h_
#define Storm_Component_Equipment_Stage_FsmContext_x_h_
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
/** @file */



/* This file is auto-generated DO NOT MANUALLY EDIT this file! */


/// Namespace(s)
namespace Storm { namespace Component { namespace Equipment { namespace Stage { 


/// Context (aka actions/guards) for my Finite State Machine
class FsmContext_
{
public:
    /// Action
    virtual void checkBackTransition() noexcept = 0;

    /// Action
    virtual void checkFromTransition() noexcept = 0;

    /// Action
    virtual void checkOffTime() noexcept = 0;

    /// Action
    virtual void checkOnTime() noexcept = 0;

    /// Action
    virtual void checkStartingOffTime() noexcept = 0;

    /// Action
    virtual void checkStartingOnTime() noexcept = 0;

    /// Action
    virtual void enterSupplementing() noexcept = 0;

    /// Action
    virtual void exitSupplementing() noexcept = 0;

    /// Action
    virtual void initializeActive() noexcept = 0;

    /// Action
    virtual void initializeBackTransition() noexcept = 0;

    /// Action
    virtual void initializeFromTransition() noexcept = 0;

    /// Action
    virtual void initializeStage() noexcept = 0;

    /// Action
    virtual void notifyLowerStage() noexcept = 0;

    /// Action
    virtual void shutdownStage() noexcept = 0;

    /// Action
    virtual void stageOff() noexcept = 0;

    /// Action
    virtual void stageOn() noexcept = 0;

    /// Action
    virtual void startCyclingInOffCycle() noexcept = 0;

    /// Action
    virtual void startCyclingInOnCycle() noexcept = 0;

    /// Action
    virtual void startOffTime() noexcept = 0;

    /// Action
    virtual void startOnTime() noexcept = 0;

    /// Action
    virtual void startingStageOff() noexcept = 0;

    /// Action
    virtual void startingStageOn() noexcept = 0;


public:
    /// Guard
    virtual bool isBeingSupplemented() noexcept = 0;

    /// Guard
    virtual bool isStartInOffCycle() noexcept = 0;


public:
    /// Virtual Destructor
    virtual ~FsmContext_(){}

};

};};};};  /// end namespace(s)
#endif /// end header latch

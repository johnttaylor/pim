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

/* Command line options: -p CADIFRA -doxygen -o Fsm -l cppx -Trace Fsm.cdd   */
/* This file is generated from Fsm.cdd - do not edit manually  */
/* Generated on:  version 4.0 */


/* Include default headers (no header definition in UML state-chart provided) */
/* Define a header comment in your statediagram.model to define own includes here. */
#include <cstdint>
#include "Storm/Component/Equipment/Stage/Fsm_ext_.h"
#include "Storm/Component/Equipment/Stage/Fsm_.h"
#include "Storm/Component/Equipment/Stage/Fsm_trace_.h"

namespace Storm { namespace Component { namespace Equipment { namespace Stage  {

    Fsm::Fsm(void)
    {
        m_initialized=static_cast<int>(0U);
    }



    /* State names */
    const char states[]=
        "OffTime\0Active\0SupplementingNextStage\0OnTime\0Off\0Cycling\0OnCycle\0StartingOn\0TransitioningFromLowerStage\0OffCycle\0StartingOff\0TransitioningBackToLowerStage\0";

    const unsigned short state_idx[]={
        0,8,15,38,45,49,57,65,76,104,113,125,155};

    /* Event names */
    const char events[]=
        "Fsm_evBackTransitionCompleted\0Fsm_evLessCapacityNeeded\0Fsm_evOnRequest\0Fsm_evOffModeRequest\0Fsm_evStartingOnTimeExpired\0Fsm_evStartingOffTimeExpired\0Fsm_evOffTimeExpired\0Fsm_evExcessCapacity\0Fsm_evOnTimeExpired\0Fsm_evFromTransitionCompleted\0Fsm_evNeedMoreCapacity\0NO_MSG\0";

    const unsigned short evt_idx[]={
        0,30,55,71,92,120,149,170,191,211,241,264};

    const char* Fsm::getNameByState(const unsigned short state) const {
        return states+state_idx[state];
    }

    const char* Fsm::getNameByEvent(const FSM_EVENT_T evt) const {
        return events+evt_idx[evt];
    }


    // Helper(s) to reset history
    void Fsm::resetHistoryActive(void){
        stateVars.stateVarActive=Cycling;
    }

    void Fsm::resetHistoryCycling(void){
        stateVars.stateVarCycling=OnCycle;
    }

    void Fsm::resetHistoryOnCycle(void){
        stateVars.stateVarOnCycle=StartingOn;
    }

    void Fsm::resetHistoryOffCycle(void){
        stateVars.stateVarOffCycle=StartingOff;
    }


    // Helper(s) to find out if the machine is in a certain state

    bool Fsm::isInOffTime(void) const {return(((stateVars.stateVarOffCycle== OffTime)&&(stateVars.stateVarCycling== OffCycle)&&(stateVars.stateVarActive== Cycling)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInActive(void) const {return(((stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInSupplementingNextStage(void) const {return(((stateVars.stateVar== SupplementingNextStage)) ? (true) : (false));}
    bool Fsm::isInOnTime(void) const {return(((stateVars.stateVarOnCycle== OnTime)&&(stateVars.stateVarCycling== OnCycle)&&(stateVars.stateVarActive== Cycling)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInOff(void) const {return(((stateVars.stateVar== Off)) ? (true) : (false));}
    bool Fsm::isInCycling(void) const {return(((stateVars.stateVarActive== Cycling)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInOnCycle(void) const {return(((stateVars.stateVarCycling== OnCycle)&&(stateVars.stateVarActive== Cycling)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInStartingOn(void) const {return(((stateVars.stateVarOnCycle== StartingOn)&&(stateVars.stateVarCycling== OnCycle)&&(stateVars.stateVarActive== Cycling)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInTransitioningFromLowerStage(void) const {return(((stateVars.stateVarActive== TransitioningFromLowerStage)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInOffCycle(void) const {return(((stateVars.stateVarCycling== OffCycle)&&(stateVars.stateVarActive== Cycling)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInStartingOff(void) const {return(((stateVars.stateVarOffCycle== StartingOff)&&(stateVars.stateVarCycling== OffCycle)&&(stateVars.stateVarActive== Cycling)&&(stateVars.stateVar== Active)) ? (true) : (false));}
    bool Fsm::isInTransitioningBackToLowerStage(void) const {return(((stateVars.stateVarActive== TransitioningBackToLowerStage)&&(stateVars.stateVar== Active)) ? (true) : (false));}

    // Helper to get id of innermost active state

    Fsm::States Fsm::getInnermostActiveState(void) const {

        Fsm::States state = NUM_STATES;

        if(isInStartingOff()){
            state = StartingOff;
        }else if(isInStartingOn()){
            state = StartingOn;
        }else if(isInOnTime()){
            state = OnTime;
        }else if(isInOffTime()){
            state = OffTime;
        }else if(isInTransitioningBackToLowerStage()){
            state = TransitioningBackToLowerStage;
        }else if(isInTransitioningFromLowerStage()){
            state = TransitioningFromLowerStage;
        }else if(isInOff()){
            state = Off;
        }else if(isInSupplementingNextStage()){
            state = SupplementingNextStage;
        }else{
            // intentionally left blank
        }
        return state;
    }

    // Initialize method. Must be called once to init the machine
    void Fsm::initialize(){
        //call on entry code of default states
        if(m_initialized==static_cast<int>(0U)){
            m_initialized=static_cast<int>(1U);

            //Create copy of statevar
            stateVarsCopy = stateVars;

            // Set state vars to default states

            stateVarsCopy.stateVar = Off; /* set init state of top state */
            stateVarsCopy.stateVarActive = Cycling; /* set init state of Active */
            stateVarsCopy.stateVarCycling = OnCycle; /* set init state of Cycling */
            stateVarsCopy.stateVarOnCycle = StartingOn; /* set init state of OnCycle */
            stateVarsCopy.stateVarOffCycle = StartingOff; /* set init state of OffCycle */



            // Copy state variables back
            stateVars = stateVarsCopy;

        }

    }


    // State machine event handler
    int Fsm::processEvent(const FSM_EVENT_T msg){

        int evConsumed = 0U;

    

        if(m_initialized==static_cast<int>(0U)){
            return static_cast<int>(0U);
        }

        //Create copy of statevar
        stateVarsCopy = stateVars;


        switch (stateVars.stateVar) {

            case Off:
                if(msg==Fsm_evOnRequest){
                    if(isBeingSupplemented()){
                        /* Transition from Off to TransitioningFromLowerStage */
                        evConsumed=1U;

                        /* Action code for transition  */
                        
                        initializeFromTransition();



                        /* adjust state variables  */
                        stateVarsCopy.stateVar = Active;
                        stateVarsCopy.stateVarActive = TransitioningFromLowerStage;
                        FsmTraceEvent(12U);
                    }else{
                        /* Transition from Off to Active */
                        evConsumed=1U;

                        initializeActive();
                        stateVarsCopy.stateVar = Active;/* Default in entry chain  */
                        stateVarsCopy.stateVarActive = Cycling; /* Default in entry chain  */
                        
                        /* Init ends in choice, select default state based on given guards  */
                        if(isStartInOffCycle()){
                            startCyclingInOffCycle();
                            startOffTime();
                            startingStageOff();
                            stateVarsCopy.stateVarCycling = OffCycle;/* Default in entry chain  */
                            stateVarsCopy.stateVarOffCycle = StartingOff;/* Default in entry chain  */
                        }else{
                            startCyclingInOnCycle();
                            startOnTime();
                            startingStageOn();
                            stateVarsCopy.stateVarCycling = OnCycle;/* Default in entry chain  */
                            stateVarsCopy.stateVarOnCycle = StartingOn;/* Default in entry chain  */
                        }

                        FsmTraceEvent(13U);
                    } /*end of event selection */
                }else{
                    /* Intentionally left blank */
                } /*end of event selection */
            break; /* end of case Off  */

            case Active:

                switch (stateVars.stateVarActive) {

                    case Cycling:

                        switch (stateVars.stateVarCycling) {

                            case OnCycle:

                                switch (stateVars.stateVarOnCycle) {

                                    case OnTime:
                                        /* action code  */
                                        checkOnTime();


                                    break; /* end of case OnTime  */

                                    case StartingOn:
                                        /* action code  */
                                        checkStartingOnTime();


                                        if(msg==Fsm_evStartingOnTimeExpired){
                                            /* Transition from StartingOn to OnTime */
                                            evConsumed=1U;

                                            /* Action code for transition  */
                                            stageOn();


                                            /* adjust state variables  */
                                            stateVarsCopy.stateVarOnCycle = OnTime;
                                            FsmTraceEvent(0U);
                                        }else{
                                            /* Intentionally left blank */
                                        } /*end of event selection */
                                    break; /* end of case StartingOn  */

                                    default:
                                        /* Intentionally left blank */
                                    break;
                                } /* end switch OnCycle */

                                /* Check if event was already processed  */
                                if(evConsumed==0U){

                                    if(msg==Fsm_evOnTimeExpired){
                                        /* Transition from OnCycle to OffCycle */
                                        evConsumed=1U;
                                        
                                        startOffTime();
                                        startingStageOff();
                                        stateVarsCopy.stateVarCycling = OffCycle;/* Default in entry chain  */
                                        stateVarsCopy.stateVarOffCycle = StartingOff;/* Default in entry chain  */

                                        FsmTraceEvent(1U);
                                    }else{
                                        /* Intentionally left blank */
                                    } /*end of event selection */
                                }
                            break; /* end of case OnCycle  */

                            case OffCycle:

                                switch (stateVars.stateVarOffCycle) {

                                    case StartingOff:
                                        /* action code  */
                                        checkStartingOffTime();


                                        if(msg==Fsm_evStartingOffTimeExpired){
                                            /* Transition from StartingOff to OffTime */
                                            evConsumed=1U;

                                            /* Action code for transition  */
                                            stageOff();


                                            /* adjust state variables  */
                                            stateVarsCopy.stateVarOffCycle = OffTime;
                                            FsmTraceEvent(2U);
                                        }else{
                                            /* Intentionally left blank */
                                        } /*end of event selection */
                                    break; /* end of case StartingOff  */

                                    case OffTime:
                                        /* action code  */
                                        checkOffTime();


                                    break; /* end of case OffTime  */

                                    default:
                                        /* Intentionally left blank */
                                    break;
                                } /* end switch OffCycle */

                                /* Check if event was already processed  */
                                if(evConsumed==0U){

                                    if(msg==Fsm_evOffTimeExpired){
                                        /* Transition from OffCycle to OnCycle */
                                        evConsumed=1U;
                                        
                                        startOnTime();
                                        startingStageOn();
                                        stateVarsCopy.stateVarCycling = OnCycle;/* Default in entry chain  */
                                        stateVarsCopy.stateVarOnCycle = StartingOn;/* Default in entry chain  */

                                        FsmTraceEvent(3U);
                                    }else{
                                        /* Intentionally left blank */
                                    } /*end of event selection */
                                }
                            break; /* end of case OffCycle  */

                            default:
                                /* Intentionally left blank */
                            break;
                        } /* end switch Cycling */

                        /* Check if event was already processed  */
                        if(evConsumed==0U){

                            if(msg==Fsm_evExcessCapacity){
                                if(isBeingSupplemented()){
                                    /* Transition from Cycling to TransitioningBackToLowerStage */
                                    evConsumed=1U;
                                
                                    if(stateVars.stateVarCycling == OnCycle){
                                        

                                    }else if(stateVars.stateVarCycling == OffCycle){
                                        
                                    }

                                    /* Action code for transition  */
                                    
                                    initializeBackTransition();


                                    /* adjust state variables  */
                                    stateVarsCopy.stateVarActive = TransitioningBackToLowerStage;
                                    FsmTraceEvent(10U);
                                }else{
                                    /* Transition from Cycling to Off */
                                    evConsumed=1U;
                                
                                    if(stateVars.stateVarCycling == OnCycle){
                                        

                                    }else if(stateVars.stateVarCycling == OffCycle){
                                        
                                    }


                                    /* Action code for transition  */
                                    
                                    shutdownStage();


                                    /* adjust state variables  */
                                    stateVarsCopy.stateVar = Off;
                                    FsmTraceEvent(11U);
                                } /*end of event selection */
                            }else if(msg==Fsm_evNeedMoreCapacity){
                                /* Transition from Cycling to SupplementingNextStage */
                                evConsumed=1U;
                                
                                if(stateVars.stateVarCycling == OnCycle){
                                    

                                }else if(stateVars.stateVarCycling == OffCycle){
                                    
                                }


                                /* Action code for transition  */
                                enterSupplementing();


                                /* adjust state variables  */
                                stateVarsCopy.stateVar = SupplementingNextStage;
                                FsmTraceEvent(8U);
                            }else{
                                /* Intentionally left blank */
                            } /*end of event selection */
                        }
                    break; /* end of case Cycling  */

                    case TransitioningFromLowerStage:
                        /* action code  */
                        checkFromTransition();


                        if(msg==Fsm_evFromTransitionCompleted){
                            /* Transition from TransitioningFromLowerStage to Cycling */
                            evConsumed=1U;

                            stateVarsCopy.stateVarActive = Cycling; /* Default in entry chain  */
                            
                            /* Init ends in choice, select default state based on given guards  */
                            if(isStartInOffCycle()){
                                startCyclingInOffCycle();
                                startOffTime();
                                startingStageOff();
                                stateVarsCopy.stateVarCycling = OffCycle;/* Default in entry chain  */
                                stateVarsCopy.stateVarOffCycle = StartingOff;/* Default in entry chain  */
                            }else{
                                startCyclingInOnCycle();
                                startOnTime();
                                startingStageOn();
                                stateVarsCopy.stateVarCycling = OnCycle;/* Default in entry chain  */
                                stateVarsCopy.stateVarOnCycle = StartingOn;/* Default in entry chain  */
                            }

                            FsmTraceEvent(5U);
                        }else{
                            /* Intentionally left blank */
                        } /*end of event selection */
                    break; /* end of case TransitioningFromLowerStage  */

                    case TransitioningBackToLowerStage:
                        /* action code  */
                        checkBackTransition();


                        if(msg==Fsm_evBackTransitionCompleted){
                            /* Transition from TransitioningBackToLowerStage to Off */
                            evConsumed=1U;


                            /* Action code for transition  */
                            notifyLowerStage();
                            shutdownStage();


                            /* adjust state variables  */
                            stateVarsCopy.stateVar = Off;
                            stateVarsCopy.stateVarActive = Cycling;
                            FsmTraceEvent(6U);
                        }else{
                            /* Intentionally left blank */
                        } /*end of event selection */
                    break; /* end of case TransitioningBackToLowerStage  */

                    default:
                        /* Intentionally left blank */
                    break;
                } /* end switch Active */

                /* Check if event was already processed  */
                if(evConsumed==0U){

                    if(msg==Fsm_evOffModeRequest){
                        /* Transition from Active to Off */
                        evConsumed=1U;
                        
                        if(stateVars.stateVarActive == Cycling){

                            if(stateVars.stateVarCycling == OnCycle){
                                

                            }else if(stateVars.stateVarCycling == OffCycle){
                                
                            }
                            
                        }

                        /* Action code for transition  */
                        
                        initializeStage();


                        /* adjust state variables  */
                        stateVarsCopy.stateVar = Off;
                        FsmTraceEvent(7U);
                    }else{
                        /* Intentionally left blank */
                    } /*end of event selection */
                }
            break; /* end of case Active  */

            case SupplementingNextStage:
                if(msg==Fsm_evLessCapacityNeeded){
                    /* Transition from SupplementingNextStage to Cycling */
                    evConsumed=1U;

                    /* Action code for transition  */
                    exitSupplementing();


                    stateVarsCopy.stateVarActive = Cycling; /* Default in entry chain  */
                    
                    /* Init ends in choice, select default state based on given guards  */
                    if(isStartInOffCycle()){
                        startCyclingInOffCycle();
                        startOffTime();
                        startingStageOff();
                        stateVarsCopy.stateVarCycling = OffCycle;/* Default in entry chain  */
                        stateVarsCopy.stateVarOffCycle = StartingOff;/* Default in entry chain  */
                    }else{
                        startCyclingInOnCycle();
                        startOnTime();
                        startingStageOn();
                        stateVarsCopy.stateVarCycling = OnCycle;/* Default in entry chain  */
                        stateVarsCopy.stateVarOnCycle = StartingOn;/* Default in entry chain  */
                    }

                    /* adjust state variables  */
                    stateVarsCopy.stateVar = Active;
                    FsmTraceEvent(9U);
                }else if(msg==Fsm_evOffModeRequest){
                    /* Transition from SupplementingNextStage to Off */
                    evConsumed=1U;

                    /* Action code for transition  */
                    
                    initializeStage();


                    /* adjust state variables  */
                    stateVarsCopy.stateVar = Off;
                    FsmTraceEvent(7U);
                }else{
                    /* Intentionally left blank */
                } /*end of event selection */
            break; /* end of case SupplementingNextStage  */

            default:
                /* Intentionally left blank */
            break;
        } /* end switch stateVar_root */
        // Copy state variables back
        stateVars = stateVarsCopy;
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "New State=%s", getNameByState(getInnermostActiveState()) ));

        return evConsumed;
    } // end processEvent







}}}} //namespace Storm { namespace Component { namespace Equipment { namespace Stage 


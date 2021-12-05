#ifndef bar_ctx_h_
#define bar_ctx_h_
/*-----------------------------------------------------------------------------
* Copyright notice Blah...
*----------------------------------------------------------------------------*/
/** @file */


#include "bar.h"


/*-------------- CONTEXT STRUCTURE -----------------*/ 
/** Application context instance data */
typedef struct bar_context_tag
{
    /* APPLICATION SPECIFIC data goes here....

    /*
    ** REQUIRED FOR ALL FSMs (do NOT edit!)
    */
    /** FSM instance struct */
    BAR_INSTANCEDATA_T instanceVar;

    /** Event que stuff here */
    int myEventQueue;
} bar_context_t;


/** Helper macro to set Context's FSM instance ID */
#define SET_BAR_CONTEXT_FSM_INSTID(thisPtr,instanceId)     (thisPtr)->instanceVar.inst_id = instanceId


/* NOTE: The following code snippet is example of statically initializing
         the above Context structure.  The application MUST still call
         SET_BAR_CONTEXT_FSM_INSTID() at run time to properly set the FSM ID */
/* bar_context_t myInstance = { ..., BAR_INSTANCEDATA_INIT, eventQueueState }; */




/* BEGIN AUTO-GEN MARKER */
/* The contents below this line are auto-generated DO NOT MANUALLY EDIT this file beyond this marker! */

/*-------------- ACTIONS -----------------*/ 
/** Action */
void actionA(bar_context_t* ctxP);

/** Action */
void actionB(bar_context_t* ctxP);

/** Action */
void actionC(bar_context_t* ctxP);

/** Action */
void exitAndLogError(bar_context_t* ctxP);


/*-------------- GUARDS -----------------*/ 
/** Guard */
bool isGuard1(bar_context_t* ctxP);

/** Guard */
bool isGuard2(bar_context_t* ctxP);


#endif /* end header latch */

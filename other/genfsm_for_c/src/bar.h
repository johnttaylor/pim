/*-----------------------------------------------------------------------------
* Copyright notice Blah...
*----------------------------------------------------------------------------*/
/** @file */

/* Command line options: -p CADIFRA -o bar -l cx -Trace bar.cdd   */
/* This file is generated from bar.cdd - do not edit manually*/
/* Generated on: Sun Nov 22 22:22:52 CST 2015 / version 3.6.9 */


#ifndef __BAR_H__
#define __BAR_H__


/* Header to define simple data types */
#include <stdint.h>

/* State definitions */
typedef enum{
   FINAL0,
   S1,
   S2,
   S3,
   __BAR_LAST_STATE__} BAR_STATES_T;






/* Predefined types required by the codegen. You can provide your own  */
/* definitions by setting the following define somewhere in your built env. */
#ifndef __PROVIDE_OWN_BAR_STATEMACHINE_TYPES__
   typedef uint8_t BAR_ENTRY_FLAG_T;
   typedef BAR_STATES_T BAR_STATEVAR_T;
   typedef uint8_t BAR_INST_ID_T;
   typedef uint8_t BAR_EV_CONSUMED_FLAG_T ;
   typedef unsigned char BAR_EVENT_T;
#endif




/* Instance data definition of state machine */
typedef struct{
   BAR_ENTRY_FLAG_T s1Entry;
   BAR_STATEVAR_T stateVar;
      BAR_INST_ID_T inst_id;
} BAR_INSTANCEDATA_T;

/* Helper to initialize the machine's instance data */
#define BAR_INSTANCEDATA_INIT {\
   1,\
   (BAR_STATEVAR_T) S1 /* set init state of top state */,\
   0 /* instance id */ } 

/* Region handler prototypes  */

/*Prototype of the state machine function */
BAR_EV_CONSUMED_FLAG_T bar(bar_context_t* userInstanceVar, BAR_EVENT_T msg);


/*Prototoye of the reset machine function */
void barResetMachine(BAR_INSTANCEDATA_T *instanceVar);

// Helper(s) to find out if the machine is in a certain state
uint8_t barIsInFINAL0(BAR_INSTANCEDATA_T *instanceVar);
uint8_t barIsInS1(BAR_INSTANCEDATA_T *instanceVar);
uint8_t barIsInS2(BAR_INSTANCEDATA_T *instanceVar);
uint8_t barIsInS3(BAR_INSTANCEDATA_T *instanceVar);


/* Macros to test if the machine is in a certain state. */
#define BAR_IS_IN_FINAL0(instance)((((&instance)->stateVar==FINAL0)) ? (1U) : (0U))
#define BAR_IS_IN_S1(instance)((((&instance)->stateVar==S1)) ? (1U) : (0U))
#define BAR_IS_IN_S2(instance)((((&instance)->stateVar==S2)) ? (1U) : (0U))
#define BAR_IS_IN_S3(instance)((((&instance)->stateVar==S3)) ? (1U) : (0U))


/* Helper returning the innermost active state.*/
BAR_STATES_T barGetInnermostActiveState(BAR_INSTANCEDATA_T *instanceVar);

#endif

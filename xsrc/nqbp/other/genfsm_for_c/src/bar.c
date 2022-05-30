/*-----------------------------------------------------------------------------
* Copyright notice Blah...
*----------------------------------------------------------------------------*/
/** @file */

/* Command line options: -p CADIFRA -o bar -l cx -Trace bar.cdd   */
/* This file is generated from bar.cdd - do not edit manually  */
/* Generated on: Sun Nov 22 22:22:52 CST 2015 / version 3.6.9 */


/* Inlcude default headers (no header definition in UML state-chart provided) */
/* Define a header comment in your statediagram.model to define own includes here. */
#include "bar_ext.h"
#include "bar_ctx.h"
#include "bar_trace.h"

/* Refere to external var (no header definition in UML state-chart provided) */
/* Define a header comment in your statediagram.model to define own definitions here. */

BAR_EV_CONSUMED_FLAG_T bar(bar_context_t* userInstanceVar, BAR_EVENT_T msg){

   BAR_EV_CONSUMED_FLAG_T evConsumed = 0U;

   BAR_INSTANCEDATA_T *instanceVar; /* ptr to instance data */
   
   /* Set instance var once */
   instanceVar=&(userInstanceVar->instanceVar);


   switch (instanceVar->stateVar) {

      case S1:
         if(msg==(BAR_EVENT_T)ev1){
            /* Transition from S1 to S2 */
            evConsumed=1U;


            /* adjust state variables  */
            instanceVar->stateVar = S2;
            barTraceEvent(0U);
         }else if(msg==(BAR_EVENT_T)ev2){
            if(isGuard1(ctx)){
               /* Transition from S1 to S1 */
               evConsumed=1U;

               /* Action code for transition  */
               actionA(ctx);



               /* adjust state variables  */
               instanceVar->stateVar = S1;
               barTraceEvent(5U);
            }else{
               /* Intentionally left blank */
            } /*end of event selection */
         }else{
            /* Intentionally left blank */
         } /*end of event selection */
      break; /* end of case S1  */

      case S2:
         if(msg==(BAR_EVENT_T)ev1){
            /* Transition from S2 to S3 */
            evConsumed=1U;

            /* Action code for transition  */
            actionA(ctx);
            actionC(ctx);



            /* adjust state variables  */
            instanceVar->stateVar = S3;
            barTraceEvent(0U);
         }else if(msg==(BAR_EVENT_T)ev2){
            /* Transition from S2 to FINAL0 */
            evConsumed=1U;


            /* adjust state variables  */
            instanceVar->stateVar = FINAL0;
            barTraceEvent(1U);
         }else{
            /* Intentionally left blank */
         } /*end of event selection */
      break; /* end of case S2  */

      case S3:
         if(msg==(BAR_EVENT_T)ev1){
            if(isGuard2(ctx)){
               /* Transition from S3 to S2 */
               evConsumed=1U;

               /* Action code for transition  */
               
               actionA(ctx);



               /* adjust state variables  */
               instanceVar->stateVar = S2;
               barTraceEvent(3U);
            }else{
               /* Transition from S3 to S1 */
               evConsumed=1U;

               /* Action code for transition  */
               
               actionB(ctx);



               /* adjust state variables  */
               instanceVar->stateVar = S1;
               barTraceEvent(4U);
            } /*end of event selection */
         }else if(msg==(BAR_EVENT_T)ev2){
            /* Transition from S3 to S3 */
            evConsumed=1U;

            /* Action code for transition  */
            exitAndLogError(ctx);



            /* adjust state variables  */
            instanceVar->stateVar = S3;
            barTraceEvent(1U);
         }else if(msg==(BAR_EVENT_T)ev3){
            /* Transition from S3 to S1 */
            evConsumed=1U;


            /* adjust state variables  */
            instanceVar->stateVar = S1;
            barTraceEvent(2U);
         }else{
            /* Intentionally left blank */
         } /*end of event selection */
      break; /* end of case S3  */

      case FINAL0:
      break; /* end of case FINAL0  */

      default:
         /* Intentionally left blank */
      break;
   } /* end switch stateVar_root */

   /* Return flag if an event was processed */
   return evConsumed;
}

/* Implementation of the reset machine function */
void barResetMachine(BAR_INSTANCEDATA_T *instanceVar){
   instanceVar->s1Entry = 1U;
   instanceVar->stateVar = S1;
   
}

// Helper(s) to find out if the machine is in a certain state

uint8_t barIsInFINAL0(BAR_INSTANCEDATA_T *instanceVar){
   return(((instanceVar->stateVar== FINAL0)) ? (1U) : (0U));
}

uint8_t barIsInS1(BAR_INSTANCEDATA_T *instanceVar){
   return(((instanceVar->stateVar== S1)) ? (1U) : (0U));
}

uint8_t barIsInS2(BAR_INSTANCEDATA_T *instanceVar){
   return(((instanceVar->stateVar== S2)) ? (1U) : (0U));
}

uint8_t barIsInS3(BAR_INSTANCEDATA_T *instanceVar){
   return(((instanceVar->stateVar== S3)) ? (1U) : (0U));
}


// Helper to get id of innermost active state
BAR_STATES_T barGetInnermostActiveState(BAR_INSTANCEDATA_T *instanceVar){
   if(barIsInS3(instanceVar)){
      return S3;
   }else if(barIsInS2(instanceVar)){
      return S2;
   }else if(barIsInS1(instanceVar)){
      return S1;
   }else if(barIsInFINAL0(instanceVar)){
      return FINAL0;
   }else{
      return __BAR_LAST_STATE__;
   }
}


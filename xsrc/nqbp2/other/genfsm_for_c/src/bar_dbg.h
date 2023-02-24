/*-----------------------------------------------------------------------------
* Copyright notice Blah...
*----------------------------------------------------------------------------*/
/** @file */

/* Command line options: -p CADIFRA -o bar -l cx -Trace bar.cdd   */
/* This file is generated from bar.cdd - do not edit manually */
/* Generated on: Sun Nov 22 22:22:52 CST 2015 / version 3.6.9 */




#ifndef __BAR_DBG_H__
#define __BAR_DBG_H__

/* State names */
const char bar_states[]=
   "FINAL0\0S1\0S2\0S3\0__BAR_LAST_STATE__\0";


/* State values */
const unsigned short bar_state_values[]={
   FINAL0,S1,S2,S3,__BAR_LAST_STATE__
};


const unsigned short bar_state_idx[]={
   0,7,10,13,16,35};

#define BAR_EVENT_START 0


/* Event names */
const char bar_events[]=
   "ev1\0ev2\0ev3\0NO_MSG\0";


/* Event values */
const unsigned short bar_event_values[]={
   ev1,ev2,ev3,BAR_NO_MSG
};


/* Helper to map event enum value to an index starting from 0 */
/* Needed if the event enum value is not just 0,1,2,... */
unsigned short bar_map_event_enum(unsigned short evt){
   unsigned short i;
   for(i=0; i<3;i++){
      if(bar_event_values[i]==evt) return i;
   }
   return 3; // should never be reached
}


/* Helper to map state enum value to an index starting from 0 */
/* Needed if the event enum value is not just 0,1,2,... */
unsigned short bar_map_state_enum(unsigned short state){
   unsigned short i;
   for(i=0; i<4;i++){
      if(bar_state_values[i]==state) return i;
   }
   return 4; // should never be reached
}



const unsigned short bar_evt_idx[]={
   0,4,8,12
};

const char* barGetNameByState(unsigned short state){
   unsigned short idx = bar_map_state_enum(state);
   return bar_states+bar_state_idx[idx];
}

const char* barGetNameByEvent(BAR_EVENT_T evt){
   unsigned short idx = bar_map_event_enum(evt);
   return bar_events+bar_evt_idx[idx];
}


#endif

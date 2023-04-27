/*-----------------------------------------------------------------------------
* Copyright notice Blah...
*----------------------------------------------------------------------------*/
/** @file */

/* Command line options: -p CADIFRA -o bar -l cx -Trace bar.cdd   */
/* This file is generated from bar_trace - do not edit manually*/
/* Generated on: version 3.6.9 */


#ifndef __BAR_TRACE_H__
#define __BAR_TRACE_H__

/* Simulation which can be sent to the state-machine statediagram.simulator */
#define barTraceEventLen 6

#ifndef MY_ENABLE_TRACE_SWITCH
#define barTraceEvent(e)
#else
void barTraceEvent(int evt);
#endif

const char* const barTraceEvents[] = {
   "ev1",
   "ev2",
   "ev3",
   "ev1[isGuard2(ctx)]",
   "ev1[else]",
   "ev2[isGuard1(ctx)]"
};


#endif

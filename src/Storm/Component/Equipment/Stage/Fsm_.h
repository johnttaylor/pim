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


#ifndef __FSM_STORM_COMPONENT_EQUIPMENT_STAGE_H__
#define __FSM_STORM_COMPONENT_EQUIPMENT_STAGE_H__

/** \class Storm::Component::Equipment::Stage::Fsm

Here is the graph that shows the state machine this class implements

\dot
digraph G {
rankdir=TB;
bgcolor=transparent;
node[fontsize=8 shape=box];
edge[fontsize=8, arrowsize=0.5];
compound=true;
remincross=true;
nodesep=0.3;
ranksep=.4;
"StartingOn"->"OnTime"[label=<evStartingOnTimeExpired / <br ALIGN="LEFT"/>stageOn();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //StartingOn OnTime
"defaultOnCycle"->"defaultOffCycle"[label=<evOnTimeExpired<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black, lhead=clusterOffCycle, ltail=clusterOnCycle];//OnCycle OffCycle
"StartingOff"->"OffTime"[label=<evStartingOffTimeExpired / <br ALIGN="LEFT"/>stageOff();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //StartingOff OffTime
"defaultOffCycle"->"defaultOnCycle"[label=<evOffTimeExpired<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black, lhead=clusterOnCycle, ltail=clusterOffCycle];//OffCycle OnCycle
"__C0"->"defaultOffCycle"[labeldistance = 2.0, taillabel=<[isStartInOffCycle()] / <br ALIGN="LEFT"/>startCyclingInOffCycle();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black lhead=clusterOffCycle];//__C0 OffCycle
"__C0"->"defaultOnCycle"[labeldistance = 2.0, taillabel=<[else] / <br ALIGN="LEFT"/>startCyclingInOnCycle();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black lhead=clusterOnCycle];//__C0 OnCycle
"defaultOffCycle"->"StartingOff"[label=< <br ALIGN="LEFT"/>startOffTime();<br ALIGN="LEFT"/>startingStageOff...<br ALIGN="LEFT"/> > fontname=Arial, style=dotted];
"defaultOnCycle"->"StartingOn"[label=< <br ALIGN="LEFT"/>startOnTime();<br ALIGN="LEFT"/>startingStageOn()...<br ALIGN="LEFT"/> > fontname=Arial, style=dotted];
"__C2"->"TransitioningFromLowerStage"[label=<[isBeingSupplemented()] / initializeFromTransition();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //__C2 TransitioningFromLowerStage
"TransitioningFromLowerStage"->"defaultCycling"[labeldistance = 2.0, taillabel=<evFromTransitionCompleted<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black lhead=clusterCycling];//TransitioningFromLowerStage Cycling
"defaultCycling"->"__C1"[labeldistance=2.0, headlabel=<evExcessCapacity<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black ltail=clusterCycling];//Cycling __C1
"__C1"->"Off"[label=<[else] / shutdownStage();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //__C1 Off
"__C1"->"TransitioningBackToLowerStage"[label=<[isBeingSupplemented()] / initializeBackTransition();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //__C1 TransitioningBackToLowerStage
"TransitioningBackToLowerStage"->"Off"[label=<evBackTransitionCompleted / <br ALIGN="LEFT"/>notifyLowerStage();<br ALIGN="LEFT"/>shutdownStag...<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //TransitioningBackToLowerStage Off
"defaultActive"->"Off"[labeldistance=2.0, headlabel=<evOffModeRequest / <br ALIGN="LEFT"/><br ALIGN="LEFT"/><br ALIGN="LEFT"/>initializeStage();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black ltail=clusterActive];//Active Off
"SupplementingNextStage"->"Off"[label=<evOffModeRequest / <br ALIGN="LEFT"/><br ALIGN="LEFT"/>initializeStage();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //SupplementingNextStage Off
"defaultActive"->"defaultCycling"[label=< <br ALIGN="LEFT"/>initializeActive();<br ALIGN="LEFT"/> > style=dotted, lhead=clusterCycling];
"Off"->"__C2"[label=<evOnRequest<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black]; //Off __C2
"__C2"->"defaultActive"[labeldistance = 2.0, taillabel=<[else]>  color=black, fontname=Arial, fontcolor=black lhead=clusterActive];//__C2 Active
"defaultroot"->"Off"[label=<  > fontname=Arial, style=dotted];
"defaultCycling"->"SupplementingNextStage"[labeldistance=2.0, headlabel=<evNeedMoreCapacity / <br ALIGN="LEFT"/><br ALIGN="LEFT"/>enterSupplementing();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black ltail=clusterCycling];//Cycling SupplementingNextStage
"SupplementingNextStage"->"defaultCycling"[labeldistance = 2.0, taillabel=<evLessCapacityNeeded / <br ALIGN="LEFT"/><br ALIGN="LEFT"/>exitSupplementing();<br ALIGN="LEFT"/>>  color=black, fontname=Arial, fontcolor=black lhead=clusterCycling];//SupplementingNextStage Cycling
"defaultCycling"->"__C0"[label=<  > fontname=Arial, style=dotted];
"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C2" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
subgraph "clusterActive"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Active<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterCycling"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Cycling<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterOnCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OnCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"OnTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OnTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"StartingOn"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOn</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOnCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
subgraph "clusterOffCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OffCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"StartingOff"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOff</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"OffTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OffTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOffCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultCycling"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"TransitioningFromLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningFromLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkFromTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"TransitioningBackToLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningBackToLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkBackTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultActive"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultroot"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
"SupplementingNextStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>SupplementingNextStage</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C2" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
subgraph "clusterActive"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Active<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterCycling"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Cycling<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterOnCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OnCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"OnTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OnTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"StartingOn"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOn</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOnCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
subgraph "clusterOffCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OffCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"StartingOff"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOff</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"OffTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OffTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOffCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultCycling"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"TransitioningFromLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningFromLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkFromTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"TransitioningBackToLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningBackToLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkBackTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultActive"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultroot"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
"SupplementingNextStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>SupplementingNextStage</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C0" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C2" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
subgraph "clusterActive"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Active<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterCycling"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Cycling<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterOnCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OnCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"OnTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OnTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"StartingOn"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOn</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOnCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
subgraph "clusterOffCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OffCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"StartingOff"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOff</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"OffTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OffTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOffCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultCycling"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"TransitioningFromLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningFromLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkFromTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"TransitioningBackToLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningBackToLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkBackTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultActive"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultroot"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
"SupplementingNextStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>SupplementingNextStage</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C2" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
subgraph "clusterActive"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Active<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterCycling"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Cycling<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterOnCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OnCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"OnTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OnTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"StartingOn"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOn</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOnCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
subgraph "clusterOffCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OffCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"StartingOff"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOff</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"OffTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OffTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOffCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultCycling"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"TransitioningFromLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningFromLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkFromTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"TransitioningBackToLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningBackToLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkBackTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultActive"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultroot"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
"SupplementingNextStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>SupplementingNextStage</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C0" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
"__C1" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C2" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
subgraph "clusterActive"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Active<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterCycling"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Cycling<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterOnCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OnCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"OnTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OnTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"StartingOn"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOn</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOnCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
subgraph "clusterOffCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OffCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"StartingOff"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOff</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"OffTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OffTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOffCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultCycling"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"TransitioningFromLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningFromLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkFromTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"TransitioningBackToLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningBackToLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkBackTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultActive"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultroot"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
"SupplementingNextStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>SupplementingNextStage</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C2" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
subgraph "clusterActive"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Active<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterCycling"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Cycling<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterOnCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OnCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"OnTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OnTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"StartingOn"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOn</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOnCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
subgraph "clusterOffCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OffCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"StartingOff"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOff</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"OffTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OffTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOffCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultCycling"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"TransitioningFromLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningFromLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkFromTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"TransitioningBackToLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningBackToLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkBackTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultActive"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultroot"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
"SupplementingNextStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>SupplementingNextStage</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C0" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C2" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
subgraph "clusterActive"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Active<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterCycling"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Cycling<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterOnCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OnCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"OnTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OnTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"StartingOn"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOn</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOnCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
subgraph "clusterOffCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OffCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"StartingOff"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOff</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"OffTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OffTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOffCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultCycling"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"TransitioningFromLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningFromLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkFromTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"TransitioningBackToLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningBackToLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkBackTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultActive"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultroot"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
"SupplementingNextStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>SupplementingNextStage</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"Off"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>Off</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C2" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
subgraph "clusterActive"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Active<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterCycling"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<Cycling<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
subgraph "clusterOnCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OnCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"OnTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OnTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"StartingOn"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOn</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOnTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOnCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
subgraph "clusterOffCycle"{fontname=Arial; fontsize=8
color=black; style="rounded";
label=<OffCycle<br ALIGN="LEFT"/><br ALIGN="LEFT"/>>;
"StartingOff"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>StartingOff</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkStartingOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"OffTime"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>OffTime</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkOffTime();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultOffCycle"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultCycling"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"TransitioningFromLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningFromLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkFromTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"TransitioningBackToLowerStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>TransitioningBackToLowerStage</B><br ALIGN="LEFT"/>| <br ALIGN="LEFT"/><u>Do:</u><br ALIGN="LEFT"/>checkBackTransition();<br ALIGN="LEFT"/><br ALIGN="LEFT"/>}>];"defaultActive"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
};
"defaultroot"[label=< >,shape=circle, fontsize=8, fixedsize=true, height=0.2, width=0.2, fillcolor=black, style=filled];
"SupplementingNextStage"[shape=record, color=black, fontname=Arial, style=rounded, label=<{<B>SupplementingNextStage</B><br ALIGN="LEFT"/>|<br ALIGN="LEFT"/>}>];"__C0" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
"__C1" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
"__C2" [label=<  > shape=diamond, fixedsize=true, height=0.2, width=0.2, fontname=arial color=black];
}
\enddot
*/

#include "Storm/Component/Equipment/Stage/Fsm_ext_.h"
#include "Storm/Component/Equipment/Stage/FsmContext_.h"

#ifndef DOXYGEN_WILL_SKIP_THIS

namespace Storm { namespace Component { namespace Equipment { namespace Stage  {

    class Fsm: public FsmContext_
    {
        public:
            explicit Fsm(void);


            int processEvent(const FSM_EVENT_T msg);

            void initialize();

            // Helper(s) to reset history
            void resetHistoryActive(void);
            void resetHistoryCycling(void);
            void resetHistoryOnCycle(void);
            void resetHistoryOffCycle(void);

            // Helper(s) to find out if the machine is in a certain state
            bool isInOffTime(void) const;
            bool isInActive(void) const;
            bool isInSupplementingNextStage(void) const;
            bool isInOnTime(void) const;
            bool isInOff(void) const;
            bool isInCycling(void) const;
            bool isInOnCycle(void) const;
            bool isInStartingOn(void) const;
            bool isInTransitioningFromLowerStage(void) const;
            bool isInOffCycle(void) const;
            bool isInStartingOff(void) const;
            bool isInTransitioningBackToLowerStage(void) const;


            // Helper returning the innermost active state id.
            enum States{
                OffTime,
                Active,
                SupplementingNextStage,
                OnTime,
                Off,
                Cycling,
                OnCycle,
                StartingOn,
                TransitioningFromLowerStage,
                OffCycle,
                StartingOff,
                TransitioningBackToLowerStage,
                NUM_STATES  // number of states in the machine
            };

            States  getInnermostActiveState(void) const;


            // Returns the state name as string
            const char* getNameByState(const unsigned short state) const;

            // Returns the event as string
            const char* getNameByEvent(const FSM_EVENT_T evt) const;

        protected:
            int m_initialized;



        protected:
            // State variables
            struct stateVarsT{
                States stateVar;
                States stateVarActive;
                States stateVarCycling;
                States stateVarOnCycle;
                States stateVarOffCycle;
            };

            stateVarsT stateVars;
            stateVarsT stateVarsCopy;



            /* Region handler prototypes  */
    };

}}}} //namespace Storm { namespace Component { namespace Equipment { namespace Stage 


#endif // !DOXYGEN_WILL_SKIP_THIS

#endif // __FSM_STORM_COMPONENT_EQUIPMENT_STAGE_H__

#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Enable trace
#define USE_CPL_SYSTEM_TRACE

// Sim time
#define USE_CPL_SYSTEM_SIM_TICK
#define OPTION_CPL_SYSTEM_SIM_TICK_NO_ACTIVITY_LIMIT	    500 // .5sec wait
#define OPTION_CPL_SYSTEM_SIM_TICK_MIN_TICKS_FOR_ADVANCE    1000 // Advance time in .5s increments -->nothing in the algorithm has 'faster' time than 2seconds
#define OPTION_CPL_SYSTEM_SIM_TICK_YEILD_SLEEP_TIME         10

// CLI
#define OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE      (8*1024)
#define OPTION_CPL_TSHELL_PROCESSOR_OUTPUT_SIZE     (8*1024)
#define OPTION_CPL_TSHELL_PROCESSOR_PROMPT          "$$$ "

// Data Model
#define OPTION_CPL_DM_MODEL_DATABASE_MAX_CAPACITY_JSON_DOC  (8*1024)
#define OPTION_CPL_DM_MODEL_DATABASE_TEMP_STORAGE_SIZE      (8*1024)


#endif

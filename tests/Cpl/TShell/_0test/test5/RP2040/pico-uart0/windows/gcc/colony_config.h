#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Turn off STDIO - using a Cpl::Io UART stream in place of
#define USE_BSP_NO_STDIO

// Enable security
#define USE_CPL_TSHELL_PROCESSOR_SILENT_WHEN_PUBLIC
#define OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL Cpl::TShell::Security::eSYSTEM

// Enable Trace
#define USE_CPL_SYSTEM_TRACE


#endif

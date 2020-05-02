#ifndef Storm_Thermostat_filelogger_ModelPoints_h_
#define Storm_Thermostat_filelogger_ModelPoints_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file This file defines test/simulation specific Model Points for the Thermostat 
          Application.  
          
          Note: the Model Points (and Database) are purposely placed in the 
                standard/global namespace to simplify their usage (i.e. less 
                verbose naming).
 */
#include "colony_config.h"
#include "Cpl/Dm/Mp/Double.h"
#include "Cpl/Dm/Mp/Bool.h"
#include "Cpl/Dm/Mp/String.h"

/** Maximum file name length (in bytes not including the null terminator)
 */
#ifndef OPTION_LOGGER_MAX_FILE_NAME
#define OPTION_LOGGER_MAX_FILE_NAME     64
#endif

/// Logger: Name of the CSV log file.  On the transition from invalid to a 'name', logging is started.  The transition to invalid stops logging
extern Cpl::Dm::Mp::String              mp_loggingFileName;

#endif  // end header latch

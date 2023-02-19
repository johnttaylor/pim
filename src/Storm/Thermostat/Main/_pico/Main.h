#ifndef Storm_Thermostat_Main_Pico_Main_h
#define Storm_Thermostat_Main_Pico_Main_h
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
/** @file 

    This file defines the PICO specific interfaces for constructing the 
    Storm::Thermostat application
 */

#include "Cpl/Dm/PeriodicScheduler.h"

/*-------------------------------------------------------*/
/// Expose the runnable object that the Algorithm executes in
extern Cpl::Dm::MailboxServer*      g_algoRunnablePtr;

/*-------------------------------------------------------*/
/// Expose the runnable object for the UI thread to allow the creation of static UI class
extern Cpl::Dm::PeriodicScheduler*  g_uiRunnablePtr;

#endif  // end header latch

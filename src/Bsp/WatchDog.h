#ifndef Bsp_WatchDog_h_
#define Bsp_WatchDog_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

	This file defines a common/generic interface for managing the application's
	and/or platform's watchdog.  The watchdog is responsible for restarted the
	application/platform if watchdog is not 'kicked' within the pre-configured
	time intervals.

	ALL Colony.* BSPs are ENCOURAGED to support this interface. The default
	implementation DOES NOTHING. Use the config preprocessor symbols
	HAVE_BSP_WATCH_DOG_xxx to disable and/or turn off the default implementation.
 */


#include "Bsp/Api.h"


// Start C++
#ifdef __cplusplus
extern "C" {
#endif


/////////////////////////////////////////////////////////////////////////////
//

/** This method starts the watchdog mechanism.  Once started, the watchdog
	mechanism can not be disabled/turned off.  This method is also responsible
	for configuring/setting the time interval/frequency that watchdog
	must be kicked to keep the application/platform from resetting.

	\b Prototype:
		void Bsp_WatchDog_start(void);
 */
#define Bsp_WatchDog_start                      Bsp_WatchDog_start_MAP


/** This method 'kicks' the watchdog, i.e. restart's the watchdog count
	down timer.

	\b Prototype:
		void Bsp_WatchDog_kick(void);
 */
#define Bsp_WatchDog_kick                       Bsp_WatchDog_kick_MAP






/////////////////////////////////////////////////////////////////////////////
//                          DEFAULT IMPLEMENATION
/////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_WILL_SKIP_THIS

#ifndef HAVE_BSP_WATCH_DOG_START
#define Bsp_WatchDog_start_MAP()                    
#endif

#ifndef HAVE_BSP_WATCH_DOG_KICK
#define Bsp_WatchDog_kick_MAP()     
#endif

#endif // end DOXYGEN_WILL_SKIP_THIS

#ifdef __cplusplus
};
#endif


#endif  // end header latch

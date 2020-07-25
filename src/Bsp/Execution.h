#ifndef Bsp_Execution_h_
#define Bsp_Execution_h_
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

	This file defines a common/generic interface for restarting, rebooting,
	etc. and application and/or a physical device.  The actual implementation
	is obviously platform dependent and as such the specific behavior is NOT
	guarantied on ALL platforms.  However, what is GUARENTIED for ALL
	platforms is that the execution of the application will be stopped - at
	least momentarily.

	ALL Colony.* BSPs are ENCOURAGED to support this interface. The default
	implementation maps to a forever loop with interrupts disabled. Use the
	config preprocessor symbols HAVE_BSP_EXECUTION_xxx to disable and/or turn
	off the default implementation.
 */


#include "Bsp/Api.h"


// Start C++
#ifdef __cplusplus
extern "C" {
#endif


/////////////////////////////////////////////////////////////////////////////
//

/// Reset type unknown or not supported
#define BSP_EXECUTION_RESET_UNKNOWN             BSP_EXECUTION_RESET_UNKNOWN_MAP

/// Cold boot
#define BSP_EXECUTION_RESET_COLD                BSP_EXECUTION_RESET_COLD_MAP

/// Warm boot
#define BSP_EXECUTION_RESET_WARM                BSP_EXECUTION_RESET_WARM_MAP

/// Reset due to drop in Voltage
#define BSP_EXECUTION_RESET_BROWN_OUT           BSP_EXECUTION_RESET_BROWN_OUT_MAP

/// Reset due to an unhandled CPU exception/error
#define BSP_EXECUTION_RESET_EXECPTION           BSP_EXECUTION_RESET_EXECPTION_MAP


/** This method returns information about the how the application's platform
	was last came out of reset.  See the above BSP_EXECUTION_RESET_xxxx
	macros for the possible return values.  The application/platform is free
	define additional reason codes


	\b Prototype:
		uint32_t Bsp_Execution_getResetType(void);
 */
#define Bsp_Execution_getResetType                  Bsp_Execution_getResetType_MAP



/////////////////////////////////////////////////////////////////////////////
//
/** This method reboots the application's platform. There are no guaranties on
	how gracefully or safe the reboot is.

	\b Prototype:
		void Bsp_Execution_reboot( void );
 */
#define Bsp_Execution_reboot                        Bsp_Execution_reboot_MAP


/** This method shuts down the application's platform. There are no guaranties on
	how gracefully or safe the shutdown is.

	\b Prototype:
		void Bsp_Execution_shutdown( void );
 */
#define Bsp_Execution_shutdown                      Bsp_Execution_shutdown_MAP




/////////////////////////////////////////////////////////////////////////////
//                          DEFAULT IMPLEMENATION
/////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_WILL_SKIP_THIS

#ifndef HAVE_BSP_EXECUTION_REBOOT
#define Bsp_Execution_reboot_MAP()                  {Bsp_Api_disableIrqs();for(;;);}
#endif

#ifndef HAVE_BSP_EXECUTION_SHUTDOWN
#define Bsp_Execution_shutdown_MAP()                {Bsp_Api_disableIrqs();for(;;);}
#endif


#ifndef HAVE_BSP_EXECUTION_GET_RESET_TYPE
#define BSP_EXECUTION_RESET_UNKNOWN_MAP             0
#define BSP_EXECUTION_RESET_COLD_MAP                1
#define BSP_EXECUTION_RESET_WARM_MAP                2
#define BSP_EXECUTION_RESET_BROWN_OUT_MAP           3
#define BSP_EXECUTION_RESET_EXECPTION_MAP           4

#define Bsp_Execution_getResetType_MAP()            BSP_EXECUTION_RESET_UNKNOWN
#endif


#endif // end DOXYGEN_WILL_SKIP_THIS

#ifdef __cplusplus
};
#endif


#endif  // end header latch

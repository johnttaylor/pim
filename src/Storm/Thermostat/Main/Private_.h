#ifndef Storm_Thermostat_0test_Log_h
#define Storm_Thermostat_0test_Log_h
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

    This file contains the interface(s) that are platform specific to running
    the Storm::Thermostat application
 */

/** This method is used to initialize 'stuff' immediately before an application
    initialization 
 */
void initializePlatform0();

/** This method is used to 'open' any platform specific objects.  It is called
    prior to the application 'open' calls
 */
void openPlatform0();

/** This method is used to 'close' any platform specific objects.  It is called
    after to the application 'close' calls
 */
void closePlatform0();

/** This method is used to 'exit' the application'
 */
int exitPlatform( int exitCode );


#endif  // end header latch

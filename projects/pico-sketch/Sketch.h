#ifndef Sketch_h
#define Sketch_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

    This file provide the logic/UI for etch-o-sketch portion of the application.
 */

#include "Cpl/System/ElapsedTime.h"


 /*-------------------------------------------------------*/
 /** This method is used to initialize the UI/Sketch logic.
     The application is required to call this method in the Periodic Scheduler's
     'beginThreading' call back function.
  */
void intializeUI();

/** The application is required to call this method from its
    periodic scheduler. The frequency of the interval determines
    the responsiveness of the UI.  Recommendation is to
    call this method at 10Hz
 */
void processUI( Cpl::System::ElapsedTime::Precision_T currentTick,
                Cpl::System::ElapsedTime::Precision_T currentInterval );


/*-------------------------------------------------------*/
#endif // end header latch

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

#include "ModelPoints.h"


// Helper macros to simplify the instantiation of the MP instances
#define ALLOC_INVALID( n, t )           t mp::n(  mp::g_modelDatabase, (#n) );  


/// Create the Model Point database
Cpl::Dm::ModelDatabase mp::g_modelDatabase( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );


////////////////////////////////////////////////////////////////////////////////

ALLOC_INVALID( signalIn, Cpl::Dm::Mp::Uint32 )
ALLOC_INVALID( hiAlarm, MpAlarm )
ALLOC_INVALID( loAlarm, MpAlarm )
ALLOC_INVALID( bootCounter, Cpl::Dm::Mp::Uint32 )
ALLOC_INVALID( metrics, MpMetrics )
ALLOC_INVALID( hiAlarmCounts, Cpl::Dm::Mp::Uint32 )
ALLOC_INVALID( loAlarmCounts, Cpl::Dm::Mp::Uint32 )

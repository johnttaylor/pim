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
#define ALLOC_INVALID( n, t )           t mp::n(  mp::modelDatabase, #n );  

Cpl::Dm::ModelDatabase mp::modelDatabase( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );


///////////////////////////////

ALLOC_INVALID( buttonEventAlt, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( buttonEventRight, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( buttonEventLeft, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( buttonEventUp, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( buttonEventDown, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( buttonEventSize, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( buttonEventColor, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( buttonEventClear, Cpl::Dm::Mp::Bool )

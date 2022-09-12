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

#include "ModelPoints.h"
#include "Storm/Thermostat/ModelPoints.h"

// Help macros because I am OCD. The model point variable 
// name is prefixed with 'mp_'. For example:
// MP_INVALID(Cpl::Dm::Mp::Float, bob) == static Cpl::Dm::Mp::Float mp_bob;

// Creates model point in the invalid state. 
#define MP_INVALID(t, n)            t mp_##n(g_modelDatabase, #n ) 

////////////////////////////////////////////////////////////////////////////////

// Allocate my Model Points
MP_INVALID( Cpl::Dm::Mp::Bool, houseSimEnabled );

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

#include "Storm/Thermostat/ModelPoints.h" 

// Help macros because I am OCD. The model point variable 
// name is prefixed with 'mp_'. For example:
// MP_INVALID(Cpl::Dm::Mp::Float, bob) == static Cpl::Dm::Mp::Float mp_bob;

// Creates model point in the invalid state. 
// Where t:=model Point type, n:=model point 'look-up' name, and 'mp_'+<n> is the model point variable name.
#define MP_INVALID(t, n)            static Cpl::Dm::StaticInfo info_mp_##n(#n); t mp_##n(g_modelDatabase, info_mp_##n ) 
#define MP_INVALID_STR(t, n, l)     static Cpl::Dm::StaticInfo info_mp_##n(#n); t mp_##n(g_modelDatabase, info_mp_##n, l ) 

////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
// NOTE: For the MickySoft compiler I must 'allocate' the g_modelDatabase before any
//       model points get instantiated.  By placing the allocation in the _0test 
//       directory AND by using nqbp's 'firstObjects' feature (and setting the _0test
//       directory to be a 'firstObjects') it seems to appease the MS gods.
//
//       Updated: The issue appears that the g_modelDatabases class's vtable
//       pointer was zero when static MP instances self registered with the
//       database.  As to why the vtable pointer is/was zero - is still a mystery
//       (the issue also appeared with the gcc/mingw compiler). The work-around 
//       was to make the insert_() method a non-virtual method.
Cpl::Dm::ModelDatabase   g_modelDatabase( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

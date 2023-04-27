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

#include "InOutModelPoints.h"


// Helper macros to simplify the instantiation of the MP instances
#define ALLOC_INVALID( n, t )           t mp::n(  mp::g_modelDatabase, (#n) );  

////////////////////////////////////////////////////////////////////////////////
Cpl::Dm::Mp::Bool*    mp::simDioInOut_inputs[DRIVER_DIO_SIMULATED_INOUT_MAX_INPUTS] =
{
    &mp::simDioInOut_in0,
    &mp::simDioInOut_in1,
    &mp::simDioInOut_in2,
    &mp::simDioInOut_in3,
    &mp::simDioInOut_in4,
    &mp::simDioInOut_in5,
    &mp::simDioInOut_in6,
    &mp::simDioInOut_in7
};

Cpl::Dm::Mp::Bool*    mp::simDioInOut_outputs[DRIVER_DIO_SIMULATED_INOUT_MAX_OUTPUTS] =
{
    &mp::simDioInOut_out0,
    &mp::simDioInOut_out1,
    &mp::simDioInOut_out2,
    &mp::simDioInOut_out3,
    &mp::simDioInOut_out4,
    &mp::simDioInOut_out5,
    &mp::simDioInOut_out6,
    &mp::simDioInOut_out7
};


ALLOC_INVALID( simDioInOut_in0, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_in1, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_in2, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_in3, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_in4, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_in5, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_in6, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_in7, Cpl::Dm::Mp::Bool )

ALLOC_INVALID( simDioInOut_out0, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_out1, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_out2, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_out3, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_out4, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_out5, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_out6, Cpl::Dm::Mp::Bool )
ALLOC_INVALID( simDioInOut_out7, Cpl::Dm::Mp::Bool )

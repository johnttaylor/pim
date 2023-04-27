#ifndef Driver_DIO_Simulated_InOut_ModelPoints_h_
#define Driver_DIO_Simulated_InOut_ModelPoints_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file 

    This file defines all of the Model Points for the Simulated DIO InOut
    driver.  
              
    All of the model points are placed into the 'mp' namespace.  The model
    point names map 1-to-1 with the instance names.

    The Application/Client is RESPONSIBLE for ensuring input values are with
    the defined range for the model point instance.
 */

#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Mp/Bool.h"

/// Maximum number of supported inputs
#define DRIVER_DIO_SIMULATED_INOUT_MAX_INPUTS       8

/// Maximum number of supported output
#define DRIVER_DIO_SIMULATED_INOUT_MAX_OUTPUTS      8


/** Encapsulate all Model Points in the 'mp' namespace to prevent polluting
    the global name space
 */
namespace mp {

/// Simulated Input 
extern Cpl::Dm::Mp::Bool     simDioInOut_in0;

/// Simulated Input 
extern Cpl::Dm::Mp::Bool     simDioInOut_in1;

/// Simulated Input 
extern Cpl::Dm::Mp::Bool     simDioInOut_in2;

/// Simulated Input
extern Cpl::Dm::Mp::Bool     simDioInOut_in3;

/// Simulated Input 
extern Cpl::Dm::Mp::Bool     simDioInOut_in4;

/// Simulated Input 
extern Cpl::Dm::Mp::Bool     simDioInOut_in5;

/// Simulated Input 
extern Cpl::Dm::Mp::Bool     simDioInOut_in6;

/// Simulated Input 
extern Cpl::Dm::Mp::Bool     simDioInOut_in7;

/// Array of Inputs
extern Cpl::Dm::Mp::Bool*    simDioInOut_inputs[DRIVER_DIO_SIMULATED_INOUT_MAX_INPUTS];


/// Simulated Output 
extern Cpl::Dm::Mp::Bool     simDioInOut_out0;

/// Simulated Output 
extern Cpl::Dm::Mp::Bool     simDioInOut_out1;

/// Simulated Output 
extern Cpl::Dm::Mp::Bool     simDioInOut_out2;

/// Simulated Output
extern Cpl::Dm::Mp::Bool     simDioInOut_out3;

/// Simulated Output 
extern Cpl::Dm::Mp::Bool     simDioInOut_out4;

/// Simulated Output 
extern Cpl::Dm::Mp::Bool     simDioInOut_out5;

/// Simulated Output 
extern Cpl::Dm::Mp::Bool     simDioInOut_out6;

/// Simulated Output 
extern Cpl::Dm::Mp::Bool     simDioInOut_out7;

/// Array of Output
extern Cpl::Dm::Mp::Bool*    simDioInOut_outputs[DRIVER_DIO_SIMULATED_INOUT_MAX_OUTPUTS];


/*---------------------------------------------------------------------------*/
/// The Application's Model Point Database
extern Cpl::Dm::ModelDatabase   g_modelDatabase;


}       // end namespace
#endif  // end header latch

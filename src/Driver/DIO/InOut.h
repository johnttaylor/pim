#ifndef Driver_DIO_InOut_h_
#define Driver_DIO_InOut_h_
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
/** @file */

#include <stdint.h>
#include <stdlib.h>

///
namespace Driver {
///
namespace DIO {

/** This class defines a generic interface for controlling N Digital inputs
    and M Digital outputs.  This driver is useful when the Application wants to 
    access Inputs and Output using a zero based index.

    The driver interface is a Singleton interface (i.e. there is only one 
    'instance' of the driver).

    The maximum 'N' and 'M' values are defined by the concrete driver
    instance.  

    The 'asserted' Pin state maps to the signal being physical high.
 */
class InOut
{
public:
    /// Configuration Options
    struct Config_T
    {
        size_t pin;         //!< GPIO pin identifier
        size_t blob;        //!< Additional Platform specific options
    };

public:
    /** Starts the driver actively sampling and outputting signals. 

        The 'inputCfg[]' and 'outputCfg[]' array must stay in the scope while
        the driver is started, i.e. can NOT go out of scope till stop() has
        been called.
         
        Returns false if an error was encountered (e.g. exceeded max number of 
        Inputs/Outputs signals)
     */
    static bool start( uint8_t          numInputs,
                       const Config_T   inputCfg[],
                       uint8_t          numOutputs,
                       const Config_T   outputCfg[] );

    /** Stops the driver from sampling inputs, and places all of the output
        into their de-asserted state
     */
    static void stop();

public:
    /** Returns (via the 'assertedOut' argument) the current commanded state of 
        the specified DO signal.  A true value indicates that the signal
        is asserted state
        
        'outputIndex' is a zero-based index.

        The method returns false if there is error or the 'outputIndex' argument
        is out of range.
     */
    static bool getOutput( uint8_t outputIndex, bool& assertedOut );

    /** Sets the state of the specified DO signal

        The method returns false if there is error or the 'outputIndex' argument
        is out of range.
     */
    static bool setOutput( uint8_t outputIndex, bool asserted );

    /// Convenience method
    static inline bool assertOutput( uint8_t outputIndex )
    {
        return setOutput( outputIndex, true );
    }

    /// Convenience method
    static inline bool deassertOutput( uint8_t outputIndex )
    {
        return setOutput( outputIndex, false );
    }

public:
    /** Returns (via the 'assertedOut' argument) the current DI signal state. A 
        true values indicates that the signal is asserted. 

        'inputIndex' is a zero-based index.

        The method returns false if there is error or the 'outputIndex' argument
        is out of range.
     */
    static bool getInput( uint8_t inputIndex, bool& assertedOut );
};

} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch

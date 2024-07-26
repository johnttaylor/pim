#ifndef Cpl_Json_ArduinoHelpers_h_
#define Cpl_Json_ArduinoHelpers_h_
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

    Contains a collection of 'helper' function (mostly for debugging) when
    using JsonArduino tool.
*/

#include "Cpl/Json/Arduino.h"


///
namespace Cpl {
///
namespace Json {

/** This method dumps the contents of the JSON object/array to the trace output.
    
    Note: This method uses the heap to temporarily allocate memory
 */
void dumpToTrace( const char* traceSection, const JsonVariant& srcObjToDump ) noexcept;



};      // end namespaces
};
#endif  // end header latch

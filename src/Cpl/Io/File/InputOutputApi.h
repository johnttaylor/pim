#ifndef Cpl_Io_File_InputOutputApi_h_
#define Cpl_Io_File_InputOutputApi_h_
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

#include "Cpl/Io/File/InputApi.h"
#include "Cpl/Io/File/OutputApi.h"
#include "Cpl/Io/InputOutput.h"

///
namespace Cpl {
///
namespace Io {
///
namespace File {


/** This abstract class defines the interface for a Random Access Input
    Output File.

    NOTE: All the read/write operations return 'false' if an error occurred,
          this INCLUDES the end-of-file condition (which is error when dealing
          with streams). To differentiate between a true error and EOF, the
          client must call isEof().
 */
class InputOutputApi : public Cpl::Io::File::InputApi,
    public Cpl::Io::File::OutputApi

{
public:
    /** Returns a 'stream proxy' for the File instance that is of type: Cpl::Io::InputOutput. 
        This method is needed because an Cpl::Io::File::InputOutputApi does NOT 
        inherit from Cpl::Io::InputOutput.
    */
    virtual Cpl::Io::InputOutput& getStream() = 0;

};


};      // end namespaces
};
};
#endif  // end header latch

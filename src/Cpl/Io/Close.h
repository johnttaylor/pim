#ifndef Cpl_Io_Close_h_
#define Cpl_Io_Close_h_
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

#include "Cpl/Container/Item.h"


///
namespace Cpl {
///
namespace Io {

/** This abstract class defines a close operation that is intended to be used
    Input and Output streams/files.  Since InputOutput streams/files are
    supported we end up with 2 close() method when the InputOutput class
    inherits from Input and Output interfaces.  This causes basically a
    'diamond' problem.  By making the close() it owns interface and a parent
    class - we can use the 'virtual mechanism' in C++ to ensure that for
    InputOutput classes there is one and only one close() method.
 */
class Close : public Cpl::Container::Item
{
public:
    /** This method will close the stream.  The result of closing the
        stream is dependent on the actual concrete stream/platform.
     */
    virtual void close() = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~Close() {}

};

};      // end namespaces
};
#endif  // end header latch

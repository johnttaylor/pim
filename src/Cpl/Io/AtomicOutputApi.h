#ifndef Cpl_Io_AtomicOutputApi_h_
#define Cpl_Io_AtomicOutputApi_h_
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

#include "Cpl/Io/Output.h"


///
namespace Cpl {
///
namespace Io {


/** This abstract template class defines a interface for an Atomic outputs to a
    stream.  The AtomicOutputApi interface is intended to be used in an multi
    threaded environment and provides a mechanism for many clients to write to
    a single output stream in manner where each client's output is atomic with
    respect to other clients.

    The interface provides two paradigms:

        1) An atomic output of a single a 'chunk' of data, i.e. each write()
           call on the Output is an atomic operation.
        2) A Callback mechanism that allows outputting many 'chunks'
           a single atomic operation.

    The template arg - CONTEXT - is the class that implements the callback
    when requestOutputs() is called.
 */
template <class CONTEXT>
class AtomicOutputApi : public Output
{
public:
    /** Definition of the call-back method that is used by the client/context
        to atomically publish many output operations.  The callback is required
        to return the OR'd return codes from all of the operations called on
        the Output stream.
     */
    typedef bool (CONTEXT::* OutputsFunction_T)(Output&);


public:
    /** This method is used to output a series of output operations to the
        stream as a single atomic operation. A callback mechanism is used and
        all output operations performed in the callback are atomic with respect
        to the Output Stream. This method returns false if a error occur on the
        Output Stream; else true is returned.

        NOTE: The callback client should keep the amount of work/duration of
              callback to a MINIMUM. All other clients attempting to publish
              to the same Output stream will be BLOCKED until this method
              returns.
     */
    virtual bool requestOutputs( CONTEXT& client, OutputsFunction_T clientsMethod ) = 0;
};

};      // end namespaces
};
#endif  // end header latch

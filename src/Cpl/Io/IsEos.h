#ifndef Cpl_Io_IsEos_h_
#define Cpl_Io_IsEos_h_
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
/** @file */



///
namespace Cpl {
///
namespace Io {

/** This abstract class defines a isEos() operation that is intended to be used
    Input and Output streams.  Since InputOutput streams are supported we end up 
	with 2 isEos() methods when the InputOutput class inherits from Input and 
	Output interfaces.  This causes basically a 'diamond' problem.  By making 
	the isEof() it owns interface and a parent class - we can use the 'virtual 
	mechanism' in C++ to ensure that for InputOutput classes there is one and 
	only one isEos() method.
 */
class IsEos 
{
public:
    /** This method returns true if End-of-Stream was encountered on the stream.
     */
    virtual bool isEos() = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~IsEos() {}

};

};      // end namespaces
};
#endif  // end header latch

#ifndef Cpl_Io_Null_h_
#define Cpl_Io_Null_h_
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

#include "Cpl/Io/InputOutput.h"


///
namespace Cpl {
///
namespace Io {


/** This concrete class implements a NULL InputOutput stream - that all of its
    input operations return 'End-of-Stream' (i.e. return false) and all output
    is dropped in the proverbial 'bit-bucket' and goes no where!  Note: The
    write() methods always return true (i.e. no error).
 */
class Null : public InputOutput
{
public:
    /// Constructor
    Null();

    /// Destructor
    ~Null();


public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::InputOutput::read;

    /// See Cpl::Io::Input
    bool read( void* buffer, int numBytes, int& bytesRead );

    /// See Cpl::Io::Input
    bool available();


public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::InputOutput::write;

    /// See Cpl::Io::Output
    bool write( const void* buffer, int maxBytes, int& bytesWritten );

    /// See Cpl::Io::Output
    void flush();

	/// See Cpl::Io::IsEos
	bool isEos();

	/// See Cpl::Io::Close. Note: Once closed() has been called, all of the write() method will return false;
    void close();


protected:
    /// Track my opened/closed state
    bool m_opened;

};

};      // end namespaces
};
#endif  // end header latch

#ifndef Cpl_Io_LineReaderApi_h_
#define Cpl_Io_LineReaderApi_h_
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

#include "Cpl/Text/String.h"
#include "Cpl/Container/Item.h"


///
namespace Cpl {
///
namespace Io {


/** This abstract class defines a interface for a stream line reader.  A
    line reader allows the client to read one line at time from the stream.
    A line is consider a stream of ASCII character terminated by '\n' a.k.a
    newline. The actual newline character(s) are handled by the interface
    and the client is not aware of the actual newline implementation.
 */
class LineReaderApi : public Cpl::Container::Item
{
public:
    /** Reads a single line from the stream.  If the line is greater than
        the will fit in 'destString', the line content is truncated.  The
        next subsequent readln() reads the next line (NOT the truncated
        characters!).  The '\n' character is NOT copied into 'destString'
        but is replaced with '\0'.  Returns true if successful, or false
        if End-of-Stream was encountered.

        NOTE: This call will not return until a newline character is encounter!
     */
    virtual bool readln( Cpl::Text::String& destString ) = 0;

    /** Returns true if there is data available to be read from the stream.

        NOTE: The implementation of this method is VERY PLATFORM dependent! If
              your code uses it - it may not be portable to all platforms.
              If a platform does not/can not support this method it is
              guaranteed to return 'true'
     */
    virtual bool available() = 0;

    /** Closes the reader and the underlying input stream.
     */
    virtual void close() = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~LineReaderApi() {}

};

};      // end namespaces
};
#endif  // end header latch

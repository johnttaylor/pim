#ifndef Cpl_Io_Input_h_
#define Cpl_Io_Input_h_
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

#include "Cpl/Text/String.h"
#include "Cpl/Io/Close.h"
#include "Cpl/Io/IsEos.h"



///
namespace Cpl {
///
namespace Io {

/** This partially abstract class defines a interface for operating on an
    input stream (example of a stream is 'stdin' or a socket connection).
    All Read calls on the stream are 'blocking' - i.e. the calls do not
    return until one or more bytes have been read from the stream.

    Note: There is really only just one read() method (the one the returns
          'bytesRead').  All of the other read() are convenience methods
          and as such a default implementation is provided for these methods.

 */
class Input : virtual public Close, virtual public IsEos
{
public:
    /** Reads a single byte from the stream.  Returns true if successful,
        or false if End-of-Stream was encountered.
     */
    virtual bool read( char& c );

    /** Reads N bytes into the String's internal buffer.  The number of
        bytes read will be less or equal to the String's max length.  The
        String is guaranteed to be terminated by a '\0'.  The placement
        of the '\0' is determined by the number of bytes read from the
        stream (i.e. buffer[bytesRead] = '\0').   Returns true if successful,
        or false if End-of-Stream was encountered.

        NOTE: NO FILTERING of 'non-printable' characters is done!  Therefore
              it is up to the client application to deal with the problem!
     */
    virtual bool read( Cpl::Text::String& destString );

    /** Attempts to read the specified number of bytes from the stream in the
        supplied buffer.  The actual number of bytes read is returned via
        'bytesRead'. Returns true if successful, or false if End-of-Stream
        was encountered.
     */
    virtual bool read( void* buffer, int numBytes, int& bytesRead ) = 0;

    /** Returns true if there data available to be read from the stream.

        NOTE: The implementation of this method is VERY PLATFORM dependent! If
              your code uses it - it may not be portable to all platforms.
              If a platform does not/can not support this method it is
              guaranteed to return 'true'

     */
    virtual bool available() = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~Input() {}

};


};      // end namespaces
};
#endif  // end header latch

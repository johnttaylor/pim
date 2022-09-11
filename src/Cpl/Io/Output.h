#ifndef Cpl_Io_Output_h_
#define Cpl_Io_Output_h_
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
#include <stdarg.h>


///
namespace Cpl {
///
namespace Io {

/** This partially abstract class defines a interface for operating on an
    output stream (example of a stream is 'stdout' or a socket connection).

    Note: There is really only just one write() method (the one the returns
          'bytesWritten').  All of the other write() are convenience methods
          and as such a default implementation is provided for these methods.
 */
class Output : virtual public Close, virtual public IsEos
{
public:
    /** Writes a single byte to the stream.  Returns true if successful,
        or false if End-of-Stream was encountered.
     */
    virtual bool write( char c );

    /** Writes a string to the stream.  The string's trailing '\0' is stripped
        off and not outputted to the stream. Returns true if successful, or false
        if End-of-Stream was encountered.  The method does not return until all
        characters in the string have been written to the Output stream.
     */
    virtual bool write( const char* string );

    /** Writes a string to the stream.  The string's trailing '\0' is stripped
        off and not outputted to the stream. Returns true if successful, or false
        if End-of-Stream was encountered. The method does not return until all
        characters in the string have been written to the Output stream.
     */
    virtual bool write( const Cpl::Text::String& string );

    /** Formatted write to the stream.  The formatting syntax/semantics is the
        same as printf(). The string's trailing '\0' is stripped off and not
        outputted to the stream.  The client is required to provide storage for
        a temporary buffer used to format the outgoing data.  The contents of
        'formatBuffer' will match what was written to the stream. Returns true
        if successful, or false if End-of-Stream was encountered. The method
        does not return until the all of the characters in the 'formatBuffer'
        (after the contents have been 'formated') is written to the Output
        stream.
     */
    virtual bool write( Cpl::Text::String& formatBuffer, const char* format, ... );

    /** Same as write( String& formatBuffer, const char* format,...), except that
        it is called with a va_list instead of a variable number of arguments.
     */
    virtual bool vwrite( Cpl::Text::String& formatBuffer, const char* format, va_list ap );

    /** Writes the content of the buffer to the stream. Returns true if
        successful, or false if End-of-Stream as encountered. The method does
        not return until 'numBytes' have been written to the Output stream.
     */
    virtual bool write( const void* buffer, int numBytes );

    /** Writes the content of the buffer to the stream. At most 'maxBytes'
        will be outputted.  The actual number of bytes written is returned
        via 'bytesWritten'. Returns true if successful, or false if End-of-Stream
        was encountered.
     */
    virtual bool write( const void* buffer, int maxBytes, int& bytesWritten ) = 0;

    /** Forces all buffered data (if any) to be written to the stream
        media.
     */
    virtual void flush() = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~Output() {}

};

};      // end namespaces
};
#endif  // end header latch

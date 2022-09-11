#ifndef Cpl_Io_LineWriterApi_h_
#define Cpl_Io_LineWriterApi_h_
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
#include "Cpl/Container/Item.h"
#include <stdarg.h>



///
namespace Cpl {
///
namespace Io {

/** This abstract class defines a interface for a stream line writer.  A
    line writer allows the client to write lines to a stream.  The interface
    also handles the output of the 'new line' character(s).
 */
class LineWriterApi : public Cpl::Container::Item
{
public:
    /** Outputs the specified string to the stream.  No newline character(s)
        are written. Returns true if successful, or false if End-of-Stream was
        encountered.
     */
    virtual bool print( const char* srcstring ) = 0;

    /** Outputs the specified string to the stream and then appends the
        newline character(s) to the stream. Returns true if successful, or
        false if End-of-Stream was encountered.
     */
    virtual bool println( const char* srcstring ) = 0;

    /** Outputs the first 'numbytes' bytes of the specified string to the
        stream.  No newline character(s) are written. Returns true if
        successful, or false if End-of-Stream was encountered.
     */
    virtual bool print( const char* srcstring, int numbytes ) = 0;

    /** Outputs the first 'numbytes' bytes of the specified string to the
        stream and then appends the newline character(s) to the stream.
        Returns true if successful, or false if End-of-Stream was
        encountered.
     */
    virtual bool println( const char* srcstring, int numbytes ) = 0;


    /** Outputs the newline character(s) to the stream.
     */
    virtual bool println() = 0;

    /** Formatted output to the stream.  The formatting syntax/semantics is the
        same as printf(). The number of characters actually outputted to
        stream is limited by the size of 'formatBuffer'.  No newline character(s)
        are written to the stream.  Returns true if successful, or false if
        End-of-Stream was encountered.
     */
    virtual bool print( Cpl::Text::String& formatBuffer, const char* format, ... ) = 0;

    /** Same as above, except newline character(s) are appended to the end of
        the formatted output.
     */
    virtual bool println( Cpl::Text::String& formatBuffer, const char* format, ... ) = 0;

    /** Same as print( String& formatBuffer, const char* format,...), except that
        it is called with a va_list instead of a variable number of arguments.
     */
    virtual bool vprint( Cpl::Text::String& formatBuffer, const char* format, va_list ap ) = 0;

    /** Same as println( String& formatBuffer, const char* format,...), except that
        it is called with a va_list instead of a variable number of arguments.
     */
    virtual bool vprintln( Cpl::Text::String& formatBuffer, const char* format, va_list ap ) = 0;

    /** Forces all buffered data (if any) to be written to the stream
        media.
     */
    virtual void flush() = 0;

    /** Closes the writer and the underlying output stream.
     */
    virtual void close() = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~LineWriterApi() {}

};


};      // end namespaces
};
#endif  // end header latch

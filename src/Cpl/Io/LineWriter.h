#ifndef Cpl_Io_LineWriter_h_
#define Cpl_Io_LineWriter_h_
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

#include "Cpl/Io/LineWriterApi.h"
#include "Cpl/Io/Output.h"
#include "Cpl/Io/NewLine.h"



///
namespace Cpl {
///
namespace Io {


/** This concrete class implements a Line Writer stream using a previously
    opened Output stream.

    NOTE: The LineWriter class does NOT provide any multi-thread mechanisms
          and/or protections.  In addition, using an instance of the
          AtomicOutputApi interface as the underlying Output stream will NOT
          make output of the println() calls atomic! The intended way to
          have an 'Atomic' LineWriter is to use the AtomicOutput callback
          mechanism (i.e. requestOutputs()) and within the callback create
          the desired LineWriter using the supplied Output stream.
 */
class LineWriter : public LineWriterApi
{
protected:
    /// Data source
    Output & m_stream;

    /// Newline
    const char* m_newline;


public:
    /** Constructor.
     */
    LineWriter( Output& stream, const char* newline=NewLine::standard() );


public:
    /// See LineWriterApi
    bool print( const char* srcstring );

    /// See LineWriterApi
    bool println( const char* srcstring );

    /// See LineWriterApi
    bool println();

    /// See LineWriterApi
    bool print( const char* srcstring, int numbytes );

    /// See LineWriterApi
    bool println( const char* srcstring, int numbytes );

    /// See LineWriterApi
    bool print( Cpl::Text::String& formatBuffer, const char* format, ... );

    /// See LineWriterApi
    bool println( Cpl::Text::String& formatBuffer, const char* format, ... );

    /// See LineWriterApi
    bool vprint( Cpl::Text::String& formatBuffer, const char* format, va_list ap );

    /// See LineWriterApi
    bool vprintln( Cpl::Text::String& formatBuffer, const char* format, va_list ap );


    /// See LineWriterApi
    void flush();

    /// See LineWriterApi
    void close();
};

};      // end namespaces
};
#endif  // end header latch

#ifndef Cpl_Io_LineReader_h_
#define Cpl_Io_LineReader_h_
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

#include "Cpl/Io/LineReaderApi.h"
#include "Cpl/Io/Input.h"
#include "Cpl/Io/NewLine.h"
#include "Cpl/Text/FString.h"



///
namespace Cpl {
///
namespace Io {


/** This concrete class implements a Line Reader stream using a previously
    opened Input stream.
 */
class LineReader : public LineReaderApi
{
protected:
    /// Data source
    Input & m_stream;

    /// Newline
    const char* m_newline;


public:
    /** Constructor.
     */
    LineReader( Input& stream, const char* newline=NewLine::standard() );


public:
    /// See LineReaderApi
    bool readln( Cpl::Text::String& destString );

    /// See LineReaderApi
    bool available();

    /// See LineReaderApi
    void close();

};

};      // end namespaces
};
#endif  // end header latch

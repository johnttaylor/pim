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

#include "LineWriter.h"

//
using namespace Cpl::Io;


///////////////////////////////
LineWriter::LineWriter( Output& stream, const char* newline )
    :m_stream( stream )
    , m_newline( newline )
{
}


///////////////////////////////
bool LineWriter::print( const char* srcstring )
{
    return m_stream.write( srcstring );
}


bool LineWriter::println( const char* srcstring )
{
    bool io = m_stream.write( srcstring );
    return io && m_stream.write( m_newline );
}


bool LineWriter::println()
{
    return m_stream.write( m_newline );
}


bool LineWriter::print( const char* srcstring, int numbytes )
{
    return m_stream.write( srcstring, numbytes );
}


bool LineWriter::println( const char* srcstring, int numbytes )
{
    bool io = m_stream.write( srcstring, numbytes );
    return io && m_stream.write( m_newline );
}


bool LineWriter::print( Cpl::Text::String& formatBuffer, const char* format, ... )
{
    va_list ap;
    va_start( ap, format );
    bool io = m_stream.vwrite( formatBuffer, format, ap );
    va_end( ap );
    return io;
}


bool LineWriter::println( Cpl::Text::String& formatBuffer, const char* format, ... )
{
    va_list ap;
    va_start( ap, format );
    bool io = m_stream.vwrite( formatBuffer, format, ap );
    va_end( ap );
    return io && m_stream.write( m_newline );
}


bool LineWriter::vprint( Cpl::Text::String& formatBuffer, const char* format, va_list ap )
{
    return m_stream.vwrite( formatBuffer, format, ap );
}


bool LineWriter::vprintln( Cpl::Text::String& formatBuffer, const char* format, va_list ap )
{
    bool io = m_stream.vwrite( formatBuffer, format, ap );
    return io && m_stream.write( m_newline );
}


void LineWriter::flush()
{
    m_stream.flush();
}


void LineWriter::close()
{
    m_stream.close();
}

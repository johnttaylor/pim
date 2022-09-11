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

#include "Output.h"
#include "Common_.h"
#include <stdarg.h>

//
using namespace Cpl::Io::File;


//////////////////////////
Output::Output( const char* fileName, bool forceCreate, bool forceEmptyFile )
    :m_stream( Common_::open( fileName, false, forceCreate, forceEmptyFile ) )
{
}

Output::Output( Cpl::Io::Descriptor streamfd )
    : m_stream( streamfd )
{
}

Output::~Output()
{
    m_stream.close();
}


bool Output::isOpened()
{
    return m_stream.isOpened();
}


//////////////////////////
bool Output::write( char c )
{
    m_stream.m_outEos = false;
    return m_stream.write( c );
}

bool Output::write( const char* string )
{
    m_stream.m_outEos = false;
    return m_stream.write( string );
}

bool Output::write( const Cpl::Text::String& string )
{
    m_stream.m_outEos = false;
    return m_stream.write( string );
}

bool Output::write( Cpl::Text::String& formatBuffer, const char* format, ... )
{
    va_list ap;
    va_start( ap, format );
    m_stream.m_outEos = false;
    bool result  = m_stream.vwrite( formatBuffer, format, ap );
    va_end( ap );
    return result;
}

bool Output::vwrite( Cpl::Text::String& formatBuffer, const char* format, va_list ap )
{
    m_stream.m_outEos = false;
    return m_stream.vwrite( formatBuffer, format, ap );
}

bool Output::write( const void* buffer, int maxBytes )
{
    m_stream.m_outEos = false;
    return m_stream.write( buffer, maxBytes );
}

bool Output::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    m_stream.m_outEos = false;
    return m_stream.write( buffer, maxBytes, bytesWritten );
}

void Output::flush()
{
    m_stream.flush();
}

bool Output::isEos()
{
	return isEof();
}

void Output::close()
{
    m_stream.close();
}


//////////////////////////
bool Output::currentPos( unsigned long& curPos )
{
    return Common_::currentPos( m_stream.m_outFd, curPos );
}

bool Output::setRelativePos( long deltaOffset )
{
    return Common_::setRelativePos( m_stream.m_outFd, deltaOffset );
}

bool Output::setToEof()
{
    return Common_::setToEof( m_stream.m_outFd );
}

bool Output::setAbsolutePos( unsigned long newoffset )
{
    return Common_::setAbsolutePos( m_stream.m_outFd, newoffset );
}

bool Output::isEof()
{
    return isOpened() == false || m_stream.m_outEos;
}

bool Output::length( unsigned long& len )
{
    return Common_::length( m_stream.m_outFd, len );
}

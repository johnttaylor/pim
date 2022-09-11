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

#include "InputOutput.h"
#include "Common_.h"


//
using namespace Cpl::Io::File;


//////////////////////////
InputOutput::InputOutput( const char* fileName, bool forceCreate, bool forceEmptyFile, bool readOnly )
    :m_stream( Common_::open( fileName, readOnly, forceCreate, forceEmptyFile ) )
{
}

InputOutput::InputOutput( Cpl::Io::Descriptor streamfd )
    : m_stream( streamfd )
{
}


InputOutput::~InputOutput()
{
    m_stream.close();
}


bool InputOutput::isOpened()
{
    return m_stream.isOpened();
}


//////////////////////////
bool InputOutput::read( char& c )
{
    m_stream.m_in.m_inEos = m_stream.m_out.m_outEos = false;
    return m_stream.read( c );
}

bool InputOutput::read( Cpl::Text::String& destString )
{
    m_stream.m_in.m_inEos = m_stream.m_out.m_outEos = false;
    return m_stream.read( destString );
}

bool InputOutput::read( void* buffer, int numBytes, int& bytesRead )
{
    m_stream.m_in.m_inEos = m_stream.m_out.m_outEos = false;
    return m_stream.read( buffer, numBytes, bytesRead );
}

bool InputOutput::available()
{
    return m_stream.available();
}


//////////////////////////
bool InputOutput::write( char c )
{
    m_stream.m_in.m_inEos = m_stream.m_out.m_outEos = false;
    return m_stream.write( c );
}

bool InputOutput::write( const char* string )
{
    m_stream.m_in.m_inEos = m_stream.m_out.m_outEos = false;
    return m_stream.write( string );
}

bool InputOutput::write( const Cpl::Text::String& string )
{
    m_stream.m_in.m_inEos = m_stream.m_out.m_outEos = false;
    return m_stream.write( string );
}

bool InputOutput::write( Cpl::Text::String& formatBuffer, const char* format, ... )
{
    va_list ap;
    va_start( ap, format );
    m_stream.m_in.m_inEos = m_stream.m_out.m_outEos = false;
    bool result           = m_stream.vwrite( formatBuffer, format, ap );
    va_end( ap );
    return result;
}

bool InputOutput::vwrite( Cpl::Text::String& formatBuffer, const char* format, va_list ap )
{
    m_stream.m_in.m_inEos = m_stream.m_out.m_outEos = false;
    return m_stream.vwrite( formatBuffer, format, ap );
}

bool InputOutput::write( const void* buffer, int maxBytes )
{
    m_stream.m_in.m_inEos = m_stream.m_out.m_outEos = false;
    return m_stream.write( buffer, maxBytes );
}

bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    m_stream.m_in.m_inEos = m_stream.m_out.m_outEos = false;
    return m_stream.write( buffer, maxBytes, bytesWritten );
}

void InputOutput::flush()
{
    m_stream.flush();
}

bool InputOutput::isEos()
{
	return isEof();
}

void InputOutput::close()
{
    m_stream.close();
}


//////////////////////////
bool InputOutput::currentPos( unsigned long& curPos )
{
    return Common_::currentPos( m_stream.m_out.m_outFd, curPos );
}

bool InputOutput::setRelativePos( long deltaOffset )
{
    return Common_::setRelativePos( m_stream.m_out.m_outFd, deltaOffset );
}

bool InputOutput::setToEof()
{
    return Common_::setToEof( m_stream.m_out.m_outFd );
}

bool InputOutput::setAbsolutePos( unsigned long newoffset )
{
    return Common_::setAbsolutePos( m_stream.m_out.m_outFd, newoffset );
}

bool InputOutput::isEof()
{
    return m_stream.isOpened() == false || m_stream.m_out.m_outEos || m_stream.m_in.m_inEos;
}

bool InputOutput::length( unsigned long& len)
{
    return Common_::length( m_stream.m_out.m_outFd, len );
}

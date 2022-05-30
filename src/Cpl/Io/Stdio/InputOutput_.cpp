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

#include "Cpl/Io/Stdio/InputOutput_.h"




//
using namespace Cpl::Io::Stdio;

/////////////////////
InputOutput_::InputOutput_( int fd )
    :m_in( fd )
    , m_out( fd )
    , m_single( true )
{
}

InputOutput_::InputOutput_( void* handle )
    :m_in( handle )
    , m_out( handle )
    , m_single( true )
{
}

InputOutput_::InputOutput_( Cpl::Io::Descriptor streamfd )
    :m_in( streamfd )
    , m_out( streamfd )
    , m_single( true )
{
}

InputOutput_::InputOutput_( int infd, int outfd )
    :m_in( infd )
    , m_out( outfd )
    , m_single( false )
{
}

InputOutput_::InputOutput_( void* inhandle, void* outhandle )
    :m_in( inhandle )
    , m_out( outhandle )
    , m_single( false )
{
}

InputOutput_::InputOutput_( Cpl::Io::Descriptor instreamfd, Cpl::Io::Descriptor outstreamfd )
    :m_in( instreamfd )
    , m_out( outstreamfd )
    , m_single( false )
{
}

InputOutput_::InputOutput_( void )
    :m_in()
    , m_out()
    , m_single( true )
{
}


bool InputOutput_::isOpened()
{
    return m_in.isOpened() && m_out.isOpened();
}

bool InputOutput_::isInputOpened()
{
    return m_in.isOpened();
}

bool InputOutput_::isOutputOpened()
{
    return m_out.isOpened();
}


/////////////////////
InputOutput_::~InputOutput_( void )
{
}


/////////////////////
void InputOutput_::activate( int fd )
{
    m_in.activate( fd );
    m_out.activate( fd );
    m_single = true;
}

void InputOutput_::activate( void* handle )
{
    m_in.activate( handle );
    m_out.activate( handle );
    m_single = true;
}

void InputOutput_::activate( Cpl::Io::Descriptor streamfd )
{
    m_in.activate( streamfd );
    m_out.activate( streamfd );
    m_single = true;
}


/////////////////////
void InputOutput_::activate( int infd, int outfd )
{
    m_in.activate( infd );
    m_out.activate( outfd );
    m_single = false;
}

void InputOutput_::activate( void* inhandle, void* outhandle )
{
    m_in.activate( inhandle );
    m_out.activate( outhandle );
    m_single = false;
}

void InputOutput_::activate( Cpl::Io::Descriptor instreamfd, Cpl::Io::Descriptor outstreamfd )
{
    m_in.activate( instreamfd );
    m_out.activate( outstreamfd );
    m_single = false;
}


/////////////////////
bool InputOutput_::read( char& c )
{
    return m_in.read( c );
}

bool InputOutput_::read( Cpl::Text::String& destString )
{
    return m_in.read( destString );
}

bool InputOutput_::read( void* buffer, int numBytes, int& bytesRead )
{
    return m_in.read( buffer, numBytes, bytesRead );
}

bool InputOutput_::available()
{
    return m_in.available();
}


/////////////////////
bool InputOutput_::write( char c )
{
    return m_out.write( c );
}

bool InputOutput_::write( const char* string )
{
    return m_out.write( string );
}

bool InputOutput_::write( const Cpl::Text::String& string )
{
    return m_out.write( string );
}

bool InputOutput_::write( Cpl::Text::String& formatBuffer, const char* format, ... )
{
    va_list ap;
    va_start( ap, format );
    bool result = m_out.vwrite( formatBuffer, format, ap );
    va_end( ap );
    return result;
}

bool InputOutput_::vwrite( Cpl::Text::String& formatBuffer, const char* format, va_list ap )
{
    return m_out.vwrite( formatBuffer, format, ap );
}

bool InputOutput_::write( const void* buffer, int maxBytes )
{
    return m_out.write( buffer, maxBytes );
}

bool InputOutput_::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    return m_out.write( buffer, maxBytes, bytesWritten );
}

void InputOutput_::flush()
{
    m_out.flush();
}

bool InputOutput_::isEos()
{
	return m_in.m_inEos || m_out.m_outEos;
}

void InputOutput_::close()
{
    if ( m_single )
    {
        m_out.close();
        m_in.m_inFd = m_out.m_outFd;
    }
    else
    {
        m_in.close();
        m_out.close();
    }
}

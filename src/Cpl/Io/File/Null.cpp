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

#include "Null.h"

//
using namespace Cpl::Io::File;



//////////////////////
Null::Null()
    :m_opened( true )
{
}

Null::~Null()
{
}


//////////////////////
bool Null::read( void* buffer, int numBytes, int& bytesRead )
{
    bytesRead = 0;
    return false;
}

bool Null::available()
{
    return false;
}


//////////////////////
bool Null::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    bytesWritten = maxBytes;
    return m_opened;
}


void Null::flush()
{
}

bool Null::isEos()
{
	return isEof();
}

void Null::close()
{
    m_opened = false;
}


//////////////////////
bool Null::isEof()
{
    return false;
}

bool Null::length( unsigned long& len )
{
    len = 0L;
    return m_opened;
}

bool Null::currentPos( unsigned long& curPos )
{
    curPos = 0L;
    return m_opened;
}

bool Null::setRelativePos( long deltaOffset )
{
    return m_opened;
}

bool Null::setAbsolutePos( unsigned long newoffset )
{
    return m_opened;
}

bool Null::setToEof()
{
    return m_opened;
}


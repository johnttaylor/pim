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


#include "StreamDecoder.h"
#include "Cpl/System/FatalError.h"



///
using namespace Cpl::Text::Frame;



///////////////////////////////////
StreamDecoder::StreamDecoder( char            rawInputBuffer[], 
							  size_t          sizeOfRawInputBuffer, 
							  Cpl::Io::Input* inputSource,
							  bool            blocking )
	:Decoder_( rawInputBuffer, sizeOfRawInputBuffer )
	, m_srcPtr( inputSource )
	, m_blocking( blocking )
{
}


///////////////////////////////////
void StreamDecoder::setInput( Cpl::Io::Input& newInFd ) noexcept
{
	m_srcPtr = &newInFd;
}


bool StreamDecoder::read( void* buffer, int numBytes, int& bytesRead )
{
	if ( !m_srcPtr )
	{
		Cpl::System::FatalError::logf( "Cpl::Text::Frame::StreamDecoder::read() - Input source has NOT be set!" );
		return false;  // Should never get here! -->but needed for unittests
	}

	// Do not allow the read() call to block if there is no data available (and non-blocking has been enabled!)
	if ( !m_blocking && !m_srcPtr->available() )
	{
		bytesRead = 0;
		return true;
	}
	
	return m_srcPtr->read( buffer, numBytes, bytesRead );
}


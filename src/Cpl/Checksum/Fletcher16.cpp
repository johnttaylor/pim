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

#include "Fletcher16.h"


using namespace Cpl::Checksum;

///////////////////////////////////////
Fletcher16::Fletcher16() noexcept
{
	reset();
}


void Fletcher16::reset( void ) noexcept
{
	m_sum1 = 0;
	m_sum2 = 0;
}


///////////////////////////////////////
void Fletcher16::accumulate( const void* bytes, unsigned numbytes ) noexcept
{
	unsigned i;
	uint8_t* ptr = (uint8_t*) bytes;
	for ( i=0; i < numbytes; i++, ptr++ )
	{
		m_sum1 = ( (uint16_t) m_sum1 + (uint16_t) ( *ptr ) ) % 255;
		m_sum2 = ( (uint16_t) m_sum2 + (uint16_t) m_sum1 ) % 255;
	}
}

uint16_t Fletcher16::finalize( void* destBuffer ) noexcept
{
	uint8_t sumlo = 255 - ( ( (uint16_t) m_sum1 + (uint16_t) m_sum2 ) % 255 );
	uint8_t sumhi = 255 - ( ( (uint16_t) m_sum1 + (uint16_t) sumlo ) % 255 );

	uint8_t* ptr = (uint8_t*) destBuffer;
	if ( ptr )
	{
		*ptr++ = sumlo;
		*ptr   = sumhi;
	}

	return ( (uint16_t) sumlo ) + ( ( (uint16_t) sumhi ) << 8 );
}

bool Fletcher16::isOkay( void ) noexcept
{
	return m_sum1 == 0 && m_sum2 == 0;
}



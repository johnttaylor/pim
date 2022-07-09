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

#include "SeqNumber.h"


/// 
using namespace Cpl::Type;

//////////////////////////////////////////
SeqNumber::SeqNumber( int32_t initialValue )
	:m_counter( initialValue )
{
}


//////////////////////////////////////////
void SeqNumber::set( int32_t newValue )
{
	if ( newValue > 0 )
	{
		m_counter = newValue;
	}
}


void SeqNumber::increment( void )
{
	// When incrementing - don't let the value go negative 
	if ( ++m_counter <= 0 )
	{
		m_counter = 1; // By design, a valid sequence number is never zero
	}
}


SeqNumber& SeqNumber::operator=( const SeqNumber rvalue )
{
	m_counter = rvalue.m_counter;
	return *this;
}

void SeqNumber::invalidate( void )
{
	m_counter = -1;
}


bool SeqNumber::compare( const SeqNumber& other ) const
{
	// ALWAYS return false if one of the sequence number is invalid
	if ( m_counter <= 0 || other.m_counter <= 0 )
	{
		return false;
	}

	return m_counter == other.m_counter;
}

bool SeqNumber::compare( int32_t other ) const
{
    SeqNumber t( other );
    return compare( t );
}

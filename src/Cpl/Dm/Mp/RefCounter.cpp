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


#include "RefCounter.h"
#include "Cpl/Text/atob.h"
#include <limits.h>

///
using namespace Cpl::Dm::Mp;

///////////////////////////////////////////////////////////////////////////////
RefCounter::RefCounter( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
	:Basic<uint32_t>( myModelBase, staticInfo )
{
}

RefCounter::RefCounter( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, uint32_t initialValue )
	: Basic<uint32_t>( myModelBase, staticInfo, initialValue )
{
}


///////////////////////////////////////////////////////////////////////////////
uint16_t RefCounter::reset( uint32_t newValue, LockRequest_T lockRequest ) noexcept
{
	m_modelDatabase.lock_();
	if ( testAndUpdateLock( lockRequest ) )
	{
		// Increment the counter and prevent overflow
		uint32_t previous = m_data;
		m_data            = newValue;

		// Generate change notices on transition to valid OR zero-to-not-zero OR not-zero-to-zero
		if ( !IS_VALID( m_validState ) || ( previous == 0 && m_data != 0 ) || ( m_data == 0 && previous != 0 ) )
		{
			processDataUpdated();
		}
	}
	uint16_t result = m_seqNum;
	m_modelDatabase.unlock_();

	return result;
}

uint16_t RefCounter::increment( uint32_t incrementAmount, LockRequest_T lockRequest ) noexcept
{
	m_modelDatabase.lock_();
	if ( testAndUpdateLock( lockRequest ) )
	{
		// Increment the counter and prevent overflow
		uint32_t previous = m_data;
		m_data           += incrementAmount;
		if ( m_data < previous )
		{
			m_data = (uint32_t) -1;
		}

		// Generate change notices on transition to valid OR zero-to-not-zero
		if ( !IS_VALID( m_validState ) || ( previous == 0 && m_data != 0 ) )
		{
			processDataUpdated();
		}
	}
	uint16_t result = m_seqNum;
	m_modelDatabase.unlock_();

	return result;
}

uint16_t RefCounter::decrement( uint32_t decrementAmount, LockRequest_T lockRequest ) noexcept
{
	m_modelDatabase.lock_();
	if ( testAndUpdateLock( lockRequest ) )
	{
		// Decrement the counter and prevent underflow
		uint32_t previous = m_data;
		m_data           -= decrementAmount;
		if ( m_data > previous )
		{
			m_data = 0;
		}

		// Generate change notices on transition to valid OR not-zero-to-zero
		if ( !IS_VALID( m_validState ) || ( m_data == 0 && previous != 0 ) )
		{
			processDataUpdated();
		}
	}
	uint16_t result = m_seqNum;
	m_modelDatabase.unlock_();

	return result;
}

uint16_t RefCounter::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
	m_modelDatabase.lock_();
	m_data          = 0;
	uint16_t result = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );
	m_modelDatabase.unlock_();
	return result;
}

void RefCounter::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
	ModelPointCommon_::attach( observer, initialSeqNumber );
}

void RefCounter::detach( Observer& observer ) noexcept
{
	ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* RefCounter::getTypeAsText() const noexcept
{
	return "Cpl::Dm::Mp::RefCounter";
}

bool RefCounter::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
{
	// Get a snapshot of the my data and state
	m_modelDatabase.lock_();
	uint32_t value  = m_data;
	uint16_t seqnum = m_seqNum;
	int8_t   valid  = m_validState;
	bool     locked = m_locked;
	m_modelDatabase.unlock_();

	// Start the conversion
	JsonDocument& doc = beginJSON( valid, locked, seqnum, verbose );

	// Construct the 'val' key/value pair (as a string)
	if ( IS_VALID( valid ) )
	{
		Cpl::Text::FString<10> tmp = value;
		doc["val"] = (char*) tmp.getString();
	}

	// End the conversion
	Cpl::Dm::ModelPointCommon_::endJSON( dst, dstSize, truncated, verbose );
	return true;
}

bool RefCounter::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
	// Attempt to parse the value key/value pair
	const char* jsonValue = src;
	if ( jsonValue == nullptr )
	{
		if ( errorMsg )
		{
			*errorMsg = "Missing 'val' key/value pair";
		}
		return false;
	}

	// Increment action
	if ( jsonValue[0] == '+' )
	{
		unsigned long numValue;
		if ( Cpl::Text::a2ul( numValue, jsonValue + 1 ) == false )
		{
			if ( errorMsg )
			{
				*errorMsg = "Invalid/bad syntax for the 'val' key/value pair";
			}
			return false;
		}
		retSequenceNumber = increment( (uint32_t) numValue, lockRequest );
	}

	// Decrement action
	else if ( jsonValue[0] == '-' )
	{
		unsigned long numValue;
		if ( Cpl::Text::a2ul( numValue, jsonValue + 1 ) == false )
		{
			if ( errorMsg )
			{
				*errorMsg = "Invalid/bad syntax for the 'val' key/value pair";
			}
			return false;
		}
		retSequenceNumber = decrement( (uint32_t) numValue, lockRequest );
	}

	// Set/Reset action
	else
	{
		unsigned long numValue;
		if ( Cpl::Text::a2ul( numValue, jsonValue ) == false )
		{
			if ( errorMsg )
			{
				*errorMsg = "Invalid/bad syntax for the 'val' key/value pair";
			}
			return false;
		}
		retSequenceNumber = reset( (uint32_t) numValue, lockRequest );
	}

	return true;
}



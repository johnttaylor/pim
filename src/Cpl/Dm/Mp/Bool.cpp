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


#include "Bool.h"

///
using namespace Cpl::Dm::Mp;

///////////////////////////////////////////////////////////////////////////////
Bool::Bool( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
	:Basic<bool>( myModelBase, staticInfo )
{
}


Bool::Bool( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, bool initialValue )
	: Basic<bool>( myModelBase, staticInfo, initialValue )
{
}


///////////////////////////////////////////////////////////////////////////////
int8_t Bool::read( bool& dstData, uint16_t* seqNumPtr ) const noexcept
{
	return ModelPointCommon_::read( &dstData, sizeof( bool ), seqNumPtr );
}

uint16_t Bool::write( bool newValue, LockRequest_T lockRequest ) noexcept
{
	return ModelPointCommon_::write( &newValue, sizeof( bool ), lockRequest );
}

uint16_t Bool::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
	return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void Bool::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
	ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Bool::detach( Observer& observer ) noexcept
{
	ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* Bool::getTypeAsText() const noexcept
{
	return "Cpl::Dm::Mp::Bool";
}

bool Bool::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
{
	// Get a snapshot of the my data and state
	m_modelDatabase.lock_();
	bool     value  = m_data;
	uint16_t seqnum = m_seqNum;
	int8_t   valid  = m_validState;
	bool     locked = m_locked;
	m_modelDatabase.unlock_();

	// Start the conversion
	JsonDocument& doc = beginJSON( valid, locked, seqnum, verbose );

	// Construct the 'val' key/value pair (as a simple numeric)
	if ( IS_VALID( valid ) )
	{
		doc["val"] = value;
	}

	// End the conversion
	endJSON( dst, dstSize, truncated, verbose );
	return true;
}

bool Bool::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
	// Attempt to parse the value key/value pair
	bool checkForError = src | false;
	bool newValue      = src | true;
	if ( newValue == true && checkForError == false )
	{
		if ( errorMsg )
		{
			*errorMsg = "Invalid syntax for the 'val' key/value pair";
		}
		return false;
	}

	retSequenceNumber = write( newValue, lockRequest );
	return true;
}

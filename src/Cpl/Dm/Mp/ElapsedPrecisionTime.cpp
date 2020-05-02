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


#include "ElapsedPrecisionTime.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/atob.h"

///
using namespace Cpl::Dm::Mp;

///////////////////////////////////////////////////////////////////////////////
ElapsedPrecisionTime::ElapsedPrecisionTime( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
	:Basic<Cpl::System::ElapsedTime::Precision_T>( myModelBase, staticInfo )
{
}

ElapsedPrecisionTime::ElapsedPrecisionTime( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, Cpl::System::ElapsedTime::Precision_T initialValue )
	: Basic<Cpl::System::ElapsedTime::Precision_T>( myModelBase, staticInfo, initialValue )
{
}


///////////////////////////////////////////////////////////////////////////////
uint16_t ElapsedPrecisionTime::readModifyWrite( Client& callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void ElapsedPrecisionTime::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

/// Type safe un-register observer
void ElapsedPrecisionTime::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

const char* ElapsedPrecisionTime::getTypeAsText() const noexcept
{
    return "Cpl::Dm::Mp::ElapsedPrecisionTime";
}



///////////////////////////////////////////////////////////////////////////////
bool ElapsedPrecisionTime::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
{
	// Get a snapshot of the my data and state
	m_modelDatabase.lock_();
	Cpl::System::ElapsedTime::Precision_T value  = m_data;
	uint16_t seqnum = m_seqNum;
	int8_t   valid  = m_validState;
	bool     locked = m_locked;
	m_modelDatabase.unlock_();

	// Start the conversion
	JsonDocument& doc = beginJSON( valid, locked, seqnum, verbose );

	// Construct the 'val' key/value pair (as a string)
	if ( IS_VALID( valid ) )
	{
		Cpl::Text::FString<20> tmp;
        Cpl::Text::formatPrecisionTimeStamp( tmp, value, true );
		doc["val"] = (char*) tmp.getString();
	}

	// End the conversion
	Cpl::Dm::ModelPointCommon_::endJSON( dst, dstSize, truncated, verbose );
	return true;
}

bool ElapsedPrecisionTime::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    // Does the value key/value pair exist?
    const char* newValue = src;
    if ( newValue == nullptr )
    {
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }

    // Parse the time string, format is: [DD ]HH:MM:SS.sss]
    Cpl::System::ElapsedTime::Precision_T newTime;
    if ( Cpl::Text::parsePrecisionTimeStamp( newValue, newTime ) == false )
    {
        if ( errorMsg )
        {
            errorMsg->format( "Failed to parse time stamp (%s)", newValue );
        }
        return false;

    }

    retSequenceNumber = write( newTime, lockRequest );
    return true;
}



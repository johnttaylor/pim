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
/** @file */


#include "ElapsedPrecisionTime.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"

///
using namespace Cpl::Dm::Mp;


///////////////////////////////////////////////////////////////////////////////
void ElapsedPrecisionTime::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    attachSubscriber( observer, initialSeqNumber );
}

/// Type safe un-register observer
void ElapsedPrecisionTime::detach( Observer& observer ) noexcept
{
    detachSubscriber( observer );
}

const char* ElapsedPrecisionTime::getTypeAsText() const noexcept
{
    return "Cpl::Dm::Mp::ElapsedPrecisionTime";
}


///////////////////////////////////////////////////////////////////////////////
void ElapsedPrecisionTime::setJSONVal( JsonDocument& doc ) noexcept
{
    Cpl::Text::FString<20> tmp;
    Cpl::Text::formatPrecisionTimeStamp( tmp, m_data, true );
    doc["val"] = (char*) tmp.getString();
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



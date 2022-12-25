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


#include "MpAlarm.h"
#include <memory.h>



///////////////////////////////////////////////////////////////////////////////
MpAlarm::MpAlarm( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    :ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
{
    memset( (void*) &m_data, 0, sizeof( m_data ) );
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpAlarm::acknowledge( LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Alarm_T newData      = m_data;
    newData.acknowledged = true;
    uint16_t result      = writeData( &newData, sizeof( Alarm_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

///////////////////////////////////////////////////////////////////////////////
const char* MpAlarm::getTypeAsText() const noexcept
{
    return "MpAlarm";
}

///////////////////////////////////////////////////////////////////////////////
void MpAlarm::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj = doc.createNestedObject( "val" );
    valObj["signal"]  = m_data.signal;
    valObj["ack"]     = m_data.acknowledged;
}


bool MpAlarm::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Alarm_T updatedData = m_data;

    // Parse Fields.  Any fields not explicitly set in the JSON are 'left as is'
    if ( src["signal"].is<size_t>() )
    {
        updatedData.signal = src["signal"].as<size_t>();
    }
    if ( src["ack"].is<bool>() )
    {
        updatedData.acknowledged = src["ack"].as<bool>();
    }
    
    // Update the MP
    retSequenceNumber = write( updatedData, lockRequest );
    return true;
}


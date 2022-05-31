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


#include "MpIdtAlarm.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include <string.h>

///
using namespace Storm::Dm;

static bool getBooleanValue( JsonObject& src, const char* key, bool& newValue );

///////////////////////////////////////////////////////////////////////////////
MpIdtAlarm::MpIdtAlarm( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    :ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
{
    memset( &m_data, 0, sizeof( m_data ) );
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpIdtAlarm::setAlarm( bool primaryAlarmState, bool secondaryAlarmState, bool isCritical, LockRequest_T lockRequest ) noexcept
{
    Data newData;
    m_modelDatabase.lock_();

    newData                = m_data;
    newData.primaryAlarm   = primaryAlarmState;
    newData.secondaryAlarm = secondaryAlarmState;
    newData.critical       = isCritical;

    // Clear ACK flags on transition to Active Alarm
    if ( primaryAlarmState && m_data.primaryAlarm == false )
    {
        newData.primaryAck = false;
    }
    if ( secondaryAlarmState && m_data.secondaryAlarm == false )
    {
        newData.secondaryAck = false;
    }
    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Data ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpIdtAlarm::acknowledgePrimaryAlarm( LockRequest_T lockRequest ) noexcept
{
    Data newData;
    m_modelDatabase.lock_();

    newData            = m_data;
    newData.primaryAck = true;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Data ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpIdtAlarm::acknowledgeSecondaryAlarm( LockRequest_T lockRequest ) noexcept
{
    Data newData;
    m_modelDatabase.lock_();

    newData              = m_data;
    newData.secondaryAck = true;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Data ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

void MpIdtAlarm::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpIdtAlarm::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}



///////////////////////////////////////////////////////////////////////////////
const char* MpIdtAlarm::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpIdtAlarm";
}

void MpIdtAlarm::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj  = doc.createNestedObject( "val" );
    valObj["priAlarm"] = m_data.primaryAlarm;
    valObj["priAck"]   = m_data.primaryAck;
    valObj["secAlarm"] = m_data.secondaryAlarm;
    valObj["secAck"]   = m_data.secondaryAck;
    valObj["critical"] = m_data.critical;
}

bool MpIdtAlarm::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Data updatedData = m_data;

    if ( src.is<JsonObject>() )
    {
        // Parse Fields
        JsonObject valObj = src;
        getBooleanValue( valObj, "priAlarm", updatedData.primaryAlarm );
        getBooleanValue( valObj, "priAck", updatedData.primaryAck );
        getBooleanValue( valObj, "secAlarm", updatedData.secondaryAlarm );
        getBooleanValue( valObj, "secAck", updatedData.secondaryAck );
        getBooleanValue( valObj, "critical", updatedData.critical );
        retSequenceNumber = write( updatedData, lockRequest );
        return true;
    }

    if ( errorMsg )
    {
        errorMsg->format( "Invalid key/value for 'val'" );
    }
    return false;
}

bool getBooleanValue( JsonObject & src, const char* key, bool& newValue )
{
    // Attempt to parse the value key/value pair
    if ( src[key].is<bool>() )
    {
        newValue = src[key];
        return true;
    }

    return false;
}

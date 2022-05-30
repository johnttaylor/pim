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


#include "MpSimpleAlarm.h"

///
using namespace Storm::Dm;


///////////////////////////////////////////////////////////////////////////////
MpSimpleAlarm::MpSimpleAlarm( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    : ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
    , m_data( { false, false, false } )
{
}


///////////////////////////////////////////////////////////////////////////////
void MpSimpleAlarm::hookSetInvalid() noexcept
{
    m_data ={ false, false, false };
}

uint16_t MpSimpleAlarm::setAlarm( bool active, bool isCritical, LockRequest_T lockRequest ) noexcept
{
    Data newData;
    m_modelDatabase.lock_();

    newData          = m_data;
    newData.active   = active;
    newData.critical = isCritical;

    // ALWAYS clear ACK flag when the active status changes
    if ( active != m_data.active )
    {
        newData.acked = false;
    }
    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Data ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpSimpleAlarm::acknowledgeAlarm( LockRequest_T lockRequest ) noexcept
{
    Data newData;
    m_modelDatabase.lock_();

    newData       = m_data;
    newData.acked = true;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Data ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

///////////////////////////////////////////////////////////////////////////////
void MpSimpleAlarm::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpSimpleAlarm::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

const char* MpSimpleAlarm::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpSimpleAlarm";
}

void MpSimpleAlarm::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj  = doc.createNestedObject( "val" );
    valObj["active"]   = m_data.active;
    valObj["ack"]      = m_data.acked;
    valObj["critical"] = m_data.critical;
}

bool MpSimpleAlarm::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    if ( src.is<JsonObject>() )
    {
        Data updatedData = m_data;

        // Parse Fields
        if ( src["active"].is<bool>() )
        {
            updatedData.active = src["active"];
        }
        if ( src["ack"].is<bool>() )
        {
            updatedData.acked = src["ack"];
        }
        if ( src["critical"].is<bool>() )
        {
            updatedData.critical = src["critical"];
        }

        retSequenceNumber = write( updatedData, lockRequest );
        return true;
    }

    if ( errorMsg )
    {
        errorMsg->format( "Invalid key/value for 'val'" );
    }
    return false;
}

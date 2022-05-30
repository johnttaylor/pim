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


#include "MpHvacRelayOutputs.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include <string.h>

#define MAX_OUTPUT      100

#define SECT_   "Storm::Dm"


///
using namespace Storm::Dm;

///////////////////////////////////////////////////////////////////////////////
MpHvacRelayOutputs::MpHvacRelayOutputs( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    :ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof(m_data), true )
{
    memset( &m_data, 0, sizeof( m_data ) );
}


///////////////////////////////////////////////////////////////////////////////

void MpHvacRelayOutputs::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpHvacRelayOutputs::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}


///////////////////////////////////////////////////////////////////////////////
const char* MpHvacRelayOutputs::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpHvacRelayOutputs";
}

void MpHvacRelayOutputs::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj  = doc.createNestedObject( "val" );
    valObj["g"]        = m_data.g;
    valObj["bk"]       = m_data.bk;
    valObj["w1"]       = m_data.w1;
    valObj["w2"]       = m_data.w2;
    valObj["w3"]       = m_data.w3;
    valObj["y1"]       = m_data.y1;
    valObj["y2"]       = m_data.y2;
    valObj["o"]        = m_data.o ? "COOL" : "HEAT";
}


bool MpHvacRelayOutputs::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Storm::Type::HvacRelayOutputs_T updatedData = m_data;

    if ( src.is<JsonObject>() )
    {
        // Relay outputs
        updatedData.bk = src["bk"] | updatedData.bk;
        updatedData.g  = src["g"] | updatedData.g;
        updatedData.w1 = src["w1"] | updatedData.w1;
        updatedData.w2 = src["w2"] | updatedData.w2;
        updatedData.w3 = src["w3"] | updatedData.w3;
        updatedData.y1 = src["y1"] | updatedData.y1;
        updatedData.y2 = src["y2"] | updatedData.y2;

        // SOV state
        const char* sov = src["o"];
        if ( sov )
        {
            if ( strcmp( sov, "COOL" ) == 0 )
            {
                updatedData.o = true;
            }
            else if ( strcmp( sov, "HEAT" ) == 0 )
            {
                updatedData.o = false;
            }
        }

        validate( updatedData );
        retSequenceNumber = write( updatedData, lockRequest );
        return true;
    }

    if ( errorMsg )
    {
        errorMsg->format( "Invalid key/value for 'val'" );
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
void MpHvacRelayOutputs::validate( Storm::Type::HvacRelayOutputs_T& newValues ) const noexcept
{
    if ( newValues.bk > MAX_OUTPUT )
    {
        newValues.bk = MAX_OUTPUT;
    }
}

void MpHvacRelayOutputs::setSafeAllOff( Storm::Type::HvacRelayOutputs_T& outputs )
{
    outputs.bk = 0;
    outputs.g  = false;
    outputs.w1 = false;
    outputs.w2 = false;
    outputs.w3 = false;
    outputs.y1 = false;
    outputs.y2 = false;
}

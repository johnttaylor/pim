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


#include "MpMetrics.h"
#include <memory.h>



///////////////////////////////////////////////////////////////////////////////
MpMetrics::MpMetrics( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    :ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
{
    memset( (void*) &m_data, 0, sizeof( m_data ) );
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpMetrics::newSample( uint32_t sampleValue, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Metrics_T newData;
    newData           = m_data;
    newData.sumTotal += sampleValue;
    newData.numSamples++;

    uint16_t result = writeData( &newData, sizeof( Metrics_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpMetrics::clearAll( LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Metrics_T newData;  // Note: The default constructor initialize everything to zero

    uint16_t result = writeData( &newData, sizeof( Metrics_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

///////////////////////////////////////////////////////////////////////////////
const char* MpMetrics::getTypeAsText() const noexcept
{
    return "MpMetrics";
}

///////////////////////////////////////////////////////////////////////////////
void MpMetrics::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj    = doc.createNestedObject( "val" );
    valObj["numSamples"] = m_data.numSamples;
    valObj["sumTotal"]   = m_data.sumTotal;
    valObj["min"]        = m_data.minValue;
    valObj["max"]        = m_data.maxValue;
}


bool MpMetrics::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Metrics_T updatedData = m_data;

    // Parse Numeric Fields.  Any fields not explicitly set in the JSON are 'left as is'
    if ( src["numSamples"].is<size_t>() )
    {
        updatedData.numSamples = src["numSamples"].as<size_t>();
    }
    if ( src["sumTotal"].is<size_t>() )
    {
        updatedData.sumTotal = src["sumTotal"].as<size_t>();
    }
    if ( src["min"].is<uint8_t>() )
    {
        updatedData.minValue = src["min"].as<uint8_t>();
    }
    if ( src["max"].is<uint8_t>() )
    {
        updatedData.minValue = src["max"].as<uint8_t>();
    }

    // Update the MP
    retSequenceNumber = write( updatedData, lockRequest );
    return true;
}


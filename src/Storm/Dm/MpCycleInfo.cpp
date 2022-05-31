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


#include "MpCycleInfo.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Math/real.h"
#include <string.h>

///
using namespace Storm::Dm;


///////////////////////////////////////////////////////////////////////////////
MpCycleInfo::MpCycleInfo( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    :ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
{
    memset( (void*) &m_data, 0, sizeof( m_data ) );
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpCycleInfo::setOnTime( uint32_t newOnCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::CycleInfo_T newData;
    m_modelDatabase.lock_();

    newData          = m_data;
    newData.onTime   = newOnCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::CycleInfo_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpCycleInfo::setOffTime( uint32_t newOffCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::CycleInfo_T newData;
    m_modelDatabase.lock_();

    newData          = m_data;
    newData.offTime  = newOffCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::CycleInfo_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}
uint16_t MpCycleInfo::setBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::CycleInfo_T newData;
    m_modelDatabase.lock_();

    newData             = m_data;
    newData.beginOnTime = newBeginOnCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::CycleInfo_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}
uint16_t MpCycleInfo::setBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::CycleInfo_T newData;
    m_modelDatabase.lock_();

    newData              = m_data;
    newData.beginOffTime = newBeginOffCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::CycleInfo_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpCycleInfo::setMode( Storm::Type::CycleStatus newMode, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::CycleInfo_T newData;
    m_modelDatabase.lock_();

    newData      = m_data;
    newData.mode = newMode;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::CycleInfo_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

void MpCycleInfo::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpCycleInfo::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

///////////////////////////////////////////////////////////////////////////////
const char* MpCycleInfo::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpCycleInfo";
}

void MpCycleInfo::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj         = doc.createNestedObject( "val" );
    valObj["onTimeMsec"]      = m_data.onTime;
    valObj["offTimeMsec"]     = m_data.offTime;
    valObj["beginOnTimeSec"]  = (double) (m_data.beginOnTime.m_seconds) + (m_data.beginOnTime.m_thousandths / 1000.0);
    valObj["beginOffTimeSec"] = (double) (m_data.beginOffTime.m_seconds) + (m_data.beginOffTime.m_thousandths / 1000.0);
    valObj["mode"]            = Storm::Type::CycleStatus::_from_integral_unchecked( m_data.mode )._to_string();
}


bool MpCycleInfo::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Storm::Type::CycleInfo_T updatedData = m_data;

    // Parse Numeric Fields
    if ( src["onTimeMsec"].is<uint32_t>() )
    {
        updatedData.onTime = src["onTimeMsec"].as<uint32_t>();
    }
    if ( src["offTimeMsec"].is<uint32_t>() )
    {
        updatedData.offTime = src["offTimeMsec"].as<uint32_t>();
    }
    if ( src["beginOnTimeSec"].is<double>() )
    {
        double beginTime                      = src["beginOnTimeSec"].as<double>();
        updatedData.beginOnTime.m_seconds     = (unsigned long) beginTime;
        updatedData.beginOnTime.m_thousandths = (uint16_t) ((beginTime - updatedData.beginOnTime.m_seconds) * 1000.0 + 0.5);
    }
    if ( src["beginOffTimeSec"].is<double>() )
    {
        double beginTime                       = src["beginOffTimeSec"].as<double>();
        updatedData.beginOffTime.m_seconds     = (unsigned long) beginTime;
        updatedData.beginOffTime.m_thousandths = (uint16_t) ((beginTime - updatedData.beginOffTime.m_seconds) * 1000.0 + 0.5);
    }

    const char* enumVal = src["mode"];
    if ( enumVal )
    {
        // Convert the text to an enum value
        auto maybeValue = Storm::Type::CycleStatus::_from_string_nothrow( enumVal );
        if ( !maybeValue )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Invalid enum value (%s)", enumVal );
            }
            return false;
        }

        updatedData.mode = *maybeValue;
    }

    retSequenceNumber = write( updatedData, lockRequest );
    return true;
}


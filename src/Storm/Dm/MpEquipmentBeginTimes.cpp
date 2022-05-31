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


#include "MpEquipmentBeginTimes.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include <string.h>

///
using namespace Storm::Dm;


///////////////////////////////////////////////////////////////////////////////
MpEquipmentBeginTimes::MpEquipmentBeginTimes( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    :ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
{
    hookSetInvalid();
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpEquipmentBeginTimes::setIndoorUnitBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                       = m_data;
    newData.indoorUnitBeginOnTime = newBeginOnCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}
uint16_t MpEquipmentBeginTimes::setIndoorUnitBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                        = m_data;
    newData.indoorUnitBeginOffTime = newBeginOffCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpEquipmentBeginTimes::setOutdoorUnitBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                        = m_data;
    newData.outdoorUnitBeginOnTime = newBeginOnCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}
uint16_t MpEquipmentBeginTimes::setOutdoorUnitBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                         = m_data;
    newData.outdoorUnitBeginOffTime = newBeginOffCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}

uint16_t MpEquipmentBeginTimes::setSystemBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                   = m_data;
    newData.systemBeginOnTime = newBeginOnCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}
uint16_t MpEquipmentBeginTimes::setSystemBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest ) noexcept
{
    Storm::Type::EquipmentTimes_T newData;
    m_modelDatabase.lock_();

    newData                    = m_data;
    newData.systemBeginOffTime = newBeginOffCycleTime;

    uint16_t result = ModelPointCommon_::write( &newData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    m_modelDatabase.unlock_();

    return result;
}


///////////////////////////////////////////////////////////////////////////////
void MpEquipmentBeginTimes::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpEquipmentBeginTimes::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

const char* MpEquipmentBeginTimes::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpEquipmentBeginTimes";
}

void MpEquipmentBeginTimes::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj                = doc.createNestedObject( "val" );
    valObj["beginIndoorOnTimeSec"]   = (double) (m_data.indoorUnitBeginOnTime.m_seconds) + (m_data.indoorUnitBeginOnTime.m_thousandths / 1000.0);
    valObj["beginIndoorOffTimeSec"]  = (double) (m_data.indoorUnitBeginOffTime.m_seconds) + (m_data.indoorUnitBeginOffTime.m_thousandths / 1000.0);
    valObj["beginOutdoorOnTimeSec"]  = (double) (m_data.outdoorUnitBeginOnTime.m_seconds) + (m_data.outdoorUnitBeginOnTime.m_thousandths / 1000.0);
    valObj["beginOutdoorOffTimeSec"] = (double) (m_data.outdoorUnitBeginOffTime.m_seconds) + (m_data.outdoorUnitBeginOffTime.m_thousandths / 1000.0);
    valObj["beginSystemOnTimeSec"]   = (double) (m_data.systemBeginOnTime.m_seconds) + (m_data.systemBeginOnTime.m_thousandths / 1000.0);
    valObj["beginSystemOffTimeSec"]  = (double) (m_data.systemBeginOffTime.m_seconds) + (m_data.systemBeginOffTime.m_thousandths / 1000.0);
}

bool MpEquipmentBeginTimes::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    Storm::Type::EquipmentTimes_T updatedData = m_data;

    // Parse Fields
    if ( src["beginIndoorOnTimeSec"].is<double>() )
    {
        double beginTime = src["beginIndoorOnTimeSec"].as<double>();
        updatedData.indoorUnitBeginOnTime.m_seconds     = (unsigned long) beginTime;
        updatedData.indoorUnitBeginOnTime.m_thousandths = (uint16_t) ((beginTime - updatedData.indoorUnitBeginOnTime.m_seconds) * 1000.0 + 0.5);
    }
    if ( src["beginIndoorOffTimeSec"].is<double>() )
    {
        double beginTime = src["beginIndoorOffTimeSec"].as<double>();
        updatedData.indoorUnitBeginOffTime.m_seconds     = (unsigned long) beginTime;
        updatedData.indoorUnitBeginOffTime.m_thousandths = (uint16_t) ((beginTime - updatedData.indoorUnitBeginOffTime.m_seconds) * 1000.0 + 0.5);
    }
    if ( src["beginOutdoorOnTimeSec"].is<double>() )
    {
        double beginTime = src["beginOutdoorOnTimeSec"].as<double>();
        updatedData.outdoorUnitBeginOnTime.m_seconds     = (unsigned long) beginTime;
        updatedData.outdoorUnitBeginOnTime.m_thousandths = (uint16_t) ((beginTime - updatedData.outdoorUnitBeginOnTime.m_seconds) * 1000.0 + 0.5);
    }
    if ( src["beginOutdoorOffTimeSec"].is<double>() )
    {
        double beginTime = src["beginOutdoorOffTimeSec"].as<double>();
        updatedData.outdoorUnitBeginOffTime.m_seconds     = (unsigned long) beginTime;
        updatedData.outdoorUnitBeginOffTime.m_thousandths = (uint16_t) ((beginTime - updatedData.outdoorUnitBeginOffTime.m_seconds) * 1000.0 + 0.5);
    }
    if ( src["beginSystemOnTimeSec"].is<double>() )
    {
        double beginTime = src["beginSystemOnTimeSec"].as<double>();
        updatedData.systemBeginOnTime.m_seconds     = (unsigned long) beginTime;
        updatedData.systemBeginOnTime.m_thousandths = (uint16_t) ((beginTime - updatedData.systemBeginOnTime.m_seconds) * 1000.0 + 0.5);
    }
    if ( src["beginSystemOffTimeSec"].is<double>() )
    {
        double beginTime = src["beginSystemOffTimeSec"].as<double>();
        updatedData.systemBeginOffTime.m_seconds     = (unsigned long) beginTime;
        updatedData.systemBeginOffTime.m_thousandths = (uint16_t) ((beginTime - updatedData.systemBeginOffTime.m_seconds) * 1000.0 + 0.5);
    }

    retSequenceNumber = write( updatedData, lockRequest );
    return true;
}


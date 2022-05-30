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


#include "MpWhiteBox.h"

///
using namespace Storm::Dm;


///////////////////////////////////////////////////////////////////////////////
MpWhiteBox::MpWhiteBox( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    : ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
{
    hookSetInvalid();   // Default everything to 'safe/non-active' values
}


uint16_t MpWhiteBox::resetPulseSettings( LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Storm::Type::WhiteBox_T src = m_data;
    src.abortOnOffCycle         = false;
    uint16_t result             = ModelPointCommon_::write( &src, sizeof( Storm::Type::WhiteBox_T ), lockRequest );

    m_modelDatabase.unlock_();
    return result;
}


///////////////////////////////////////////////////////////////////////////////
void MpWhiteBox::attach( Observer & observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpWhiteBox::detach( Observer & observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

const char* MpWhiteBox::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpWhiteBox";
}

void MpWhiteBox::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj               = doc.createNestedObject( "val" );
    valObj["defeatEquipMinOffTime"] = m_data.defeatEquipMinOffTime;
    valObj["abortOnOffCycle"]       = m_data.abortOnOffCycle;
}

bool MpWhiteBox::fromJSON_( JsonVariant & src, LockRequest_T lockRequest, uint16_t & retSequenceNumber, Cpl::Text::String * errorMsg ) noexcept
{
    if ( src.is<JsonObject>() )
    {
        // Parse Fields
        Storm::Type::WhiteBox_T newData = m_data;
        newData.defeatEquipMinOffTime   = src["defeatEquipMinOffTime"] | m_data.defeatEquipMinOffTime;
        newData.abortOnOffCycle         = src["abortOnOffCycle"] | m_data.abortOnOffCycle;

        // Update the Model Point
        retSequenceNumber = write( newData, lockRequest );
        return true;
    }

    if ( errorMsg )
    {
        errorMsg->format( "Invalid key/value pair" );
    }
    return false;
}


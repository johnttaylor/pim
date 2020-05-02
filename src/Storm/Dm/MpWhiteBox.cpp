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
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include <string.h>

///
using namespace Storm::Dm;


///////////////////////////////////////////////////////////////////////////////
MpWhiteBox::MpWhiteBox( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    : ModelPointCommon_( myModelBase, &m_data, staticInfo, MODEL_POINT_STATE_VALID )
{
    memset( &m_data, 0, sizeof( m_data ) );
}


///////////////////////////////////////////////////////////////////////////////
uint16_t MpWhiteBox::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    // Reset all settings to their 'safe' values when being
    // invalidated. This ensure proper behavior when just write ONE setting
    memset( &m_data, 0, sizeof( m_data ) );

    uint16_t result  = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

int8_t MpWhiteBox::read( Storm::Type::WhiteBox_T& dst, uint16_t * seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &dst, sizeof( Storm::Type::WhiteBox_T ), seqNumPtr );
}


uint16_t MpWhiteBox::write( const Storm::Type::WhiteBox_T& srcSettings, LockRequest_T lockRequest ) noexcept
{
    return ModelPointCommon_::write( &srcSettings, sizeof( Storm::Type::WhiteBox_T ), lockRequest );
}

uint16_t MpWhiteBox::writeDefeatEquipMinOffTime( bool newValue, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Storm::Type::WhiteBox_T src = m_data;
    src.defeatEquipMinOffTime   = newValue;
    uint16_t result             = ModelPointCommon_::write( &src, sizeof( Storm::Type::WhiteBox_T ), lockRequest );

    m_modelDatabase.unlock_();
    return result;
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

uint16_t MpWhiteBox::readModifyWrite( Client & callbackClient, LockRequest_T lockRequest )
{
    return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
}

void MpWhiteBox::attach( Observer & observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpWhiteBox::detach( Observer & observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

bool MpWhiteBox::isDataEqual_( const void* otherData ) const noexcept
{
    Storm::Type::WhiteBox_T* otherDataPtr = ( Storm::Type::WhiteBox_T* ) otherData;

    return m_data.defeatEquipMinOffTime == otherDataPtr->defeatEquipMinOffTime &&
        m_data.abortOnOffCycle == otherDataPtr->abortOnOffCycle;
}

void MpWhiteBox::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Storm::Type::WhiteBox_T ) );
    Storm::Type::WhiteBox_T* dstDataPtr = ( Storm::Type::WhiteBox_T* ) dstData;
    *dstDataPtr                         = m_data;
}

void MpWhiteBox::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Storm::Type::WhiteBox_T ) );
    Storm::Type::WhiteBox_T* dataSrcPtr = ( Storm::Type::WhiteBox_T* ) srcData;
    m_data                              = *dataSrcPtr;
}


///////////////////////////////////////////////////////////////////////////////
const char* MpWhiteBox::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpWhiteBox";
}

size_t MpWhiteBox::getSize() const noexcept
{
    return sizeof( Storm::Type::WhiteBox_T );
}

size_t MpWhiteBox::getInternalDataSize_() const noexcept
{
    return sizeof( Storm::Type::WhiteBox_T );
}


const void* MpWhiteBox::getImportExportDataPointer_() const noexcept
{
    return &m_data;
}

bool MpWhiteBox::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
{
    // Get my state
    m_modelDatabase.lock_();
    uint16_t seqnum = m_seqNum;
    int8_t   valid  = m_validState;
    bool     locked = m_locked;

    // Start the conversion
    JsonDocument& doc = beginJSON( valid, locked, seqnum, verbose );

    // Construct the 'val' key/value pair 
    if ( IS_VALID( valid ) )
    {
        JsonObject valObj               = doc.createNestedObject( "val" );
        valObj["defeatEquipMinOffTime"] = m_data.defeatEquipMinOffTime;
        valObj["abortOnOffCycle"]       = m_data.abortOnOffCycle;

    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose );

    m_modelDatabase.unlock_();
    return true;
}

bool MpWhiteBox::fromJSON_( JsonVariant & src, LockRequest_T lockRequest, uint16_t & retSequenceNumber, Cpl::Text::String * errorMsg ) noexcept
{
    // Parse Fields
    Storm::Type::WhiteBox_T newData = m_data;
    newData.defeatEquipMinOffTime   = src["defeatEquipMinOffTime"] | m_data.defeatEquipMinOffTime;
    newData.abortOnOffCycle         = src["abortOnOffCycle"] | m_data.abortOnOffCycle;

    // Update the Model Point
    retSequenceNumber = write( newData, lockRequest );
    return true;
}


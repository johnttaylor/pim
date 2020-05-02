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


#include "MpEquipmentConfig.h"
#include "Storm/Constants.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Math/real.h"
#include <string.h>

///
using namespace Storm::Dm;


///////////////////////////////////////////////////////////////////////////////

MpEquipmentConfig::MpEquipmentConfig( Cpl::Dm::ModelDatabase&  myModelBase,
                                      Cpl::Dm::StaticInfo&     staticInfo,
                                      Storm::Type::IduType     iduUnitType,
                                      bool                     hasVspBlower,
                                      uint16_t                 numIduHeatingStages,
                                      Storm::Type::OduType     oduUnitType,
                                      uint16_t                 numCompressorStages )
    : ModelPointCommon_( myModelBase, &m_data, staticInfo, MODEL_POINT_STATE_VALID )
    , m_data( { iduUnitType, oduUnitType, numCompressorStages, numIduHeatingStages, hasVspBlower } )
{
    validate( m_data );
}

///////////////////////////////////////////////////////////////////////////////
uint16_t MpEquipmentConfig::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    // Reset the default values when being invalidated. This ensure proper 
    // behavior when just updating a single field.
    m_data.iduType             = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDUTYPE;
    m_data.oduType             = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_ODUTYPE;
    m_data.hasVspBlower        = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_VSPBLOWER;
    m_data.numIduHeatingStages = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDU_NUM_STAGES;
    m_data.numCompStages       = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_COMP_NUM_STAGES;

    // Return the sequence number
    uint16_t result  = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );
    m_modelDatabase.unlock_();
    return result;
}

int8_t MpEquipmentConfig::read( Data& configuration, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &configuration, sizeof( Data ), seqNumPtr );
}

uint16_t MpEquipmentConfig::write( Data& newConfiguration, LockRequest_T lockRequest ) noexcept
{
    // Enforce limits
    validate( newConfiguration );
    return ModelPointCommon_::write( &newConfiguration, sizeof( Data ), lockRequest );
}

uint16_t MpEquipmentConfig::writeIndoorType( Storm::Type::IduType newUnitType, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Data src        = m_data;
    src.iduType     = newUnitType;
    validate( src );
    uint16_t result = ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

uint16_t MpEquipmentConfig::writeIndoorFanMotor( bool hasVspBlower, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Data src         = m_data;
    src.hasVspBlower = hasVspBlower;
    validate( src );
    uint16_t result  = ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

uint16_t MpEquipmentConfig::writeIndoorHeatingStages( uint16_t numIduHeatingStages, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Data src                = m_data;
    src.numIduHeatingStages = numIduHeatingStages;
    validate( src );
    uint16_t result         = ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );

    m_modelDatabase.unlock_();
    return result;
}
uint16_t MpEquipmentConfig::writeOutdoorType( Storm::Type::OduType newUnitType, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Data src        = m_data;
    src.oduType     = newUnitType;
    validate( src );
    uint16_t result = ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

uint16_t MpEquipmentConfig::writeCompressorStages( uint16_t numStages, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Data src          = m_data;
    src.numCompStages = numStages;
    validate( src );
    uint16_t result   = ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

uint16_t MpEquipmentConfig::readModifyWrite( Client & callbackClient, LockRequest_T lockRequest )
{
    m_modelDatabase.lock_();
    uint16_t result = ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );

    // Ensure the limits are enforced
    // NOTE: In theory this method only needs to be called when there was a
    //       operation.  However, if the data was just 'read' or invalidated,
    //       enforcing the range limits is either a NOP or a don't care
    //       respectively.
    validate( m_data );

    m_modelDatabase.unlock_();
    return result;
}

void MpEquipmentConfig::attach( Observer & observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpEquipmentConfig::detach( Observer & observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

bool MpEquipmentConfig::isDataEqual_( const void* otherData ) const noexcept
{
    return memcmp( &m_data, otherData, sizeof( Data ) ) == 0;;
}

void MpEquipmentConfig::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Data ) );
    Data* dstDataPtr   = ( Data*) dstData;
    *dstDataPtr        = m_data;
}

void MpEquipmentConfig::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Data ) );
    Data* dataSrcPtr   = ( Data*) srcData;
    m_data             = *dataSrcPtr;
}


///////////////////////////////////////////////////////////////////////////////
const char* MpEquipmentConfig::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpEquipmentConfig";
}

size_t MpEquipmentConfig::getSize() const noexcept
{
    return sizeof( Data );
}

size_t MpEquipmentConfig::getInternalDataSize_() const noexcept
{
    return sizeof( Data );
}


const void* MpEquipmentConfig::getImportExportDataPointer_() const noexcept
{
    return &m_data;
}

bool MpEquipmentConfig::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
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
        JsonObject valObj            = doc.createNestedObject( "val" );
        Storm::Type::IduType iduType = Storm::Type::IduType::_from_integral_unchecked( m_data.iduType );
        valObj["iduType"]            = iduType._to_string();
        valObj["vspBlower"]          = m_data.hasVspBlower;
        valObj["numIduHeat"]         = m_data.numIduHeatingStages;
        Storm::Type::OduType oduType = Storm::Type::OduType::_from_integral_unchecked( m_data.oduType );
        valObj["oduType"]            = oduType._to_string();
        valObj["numCompStages"]      = m_data.numCompStages;
    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose );

    m_modelDatabase.unlock_();
    return true;
}

bool MpEquipmentConfig::fromJSON_( JsonVariant & src, LockRequest_T lockRequest, uint16_t & retSequenceNumber, Cpl::Text::String * errorMsg ) noexcept
{
    Data newVal = m_data;

    // Parse Type field
    const char* enumVal = src["iduType"];
    if ( enumVal )
    {
        // Convert the text to an enum value
        auto maybeValue = Storm::Type::IduType::_from_string_nothrow( enumVal );
        if ( !maybeValue )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Invalid enum value (%s)", enumVal );
            }
            return false;
        }

        newVal.iduType = *maybeValue;
    }

    // Parse Indoor Fan Motor option
    JsonVariant vspBlower = src["vspBlower"];
    if ( vspBlower.isNull() == false  )
    {
        newVal.hasVspBlower = vspBlower.as<bool>();
    }

    // Parse number of Indoor stages
    int stages = src["numIduHeat"] | -1;
    if ( stages >= 0 )
    {
        newVal.numIduHeatingStages = ( uint16_t) stages;
    }

    // Parse Type field
    enumVal = src["odutype"];
    if ( enumVal )
    {
        // Convert the text to an enum value
        auto maybeValue = Storm::Type::OduType::_from_string_nothrow( enumVal );
        if ( !maybeValue )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Invalid enum value (%s)", enumVal );
            }
            return false;
        }

        newVal.oduType = *maybeValue;
    }

    // Parse number of Compressor stages
    stages = src["numCompStages"] | -1;
    if ( stages >= 0 )
    {
        newVal.numCompStages = ( uint16_t) stages;
    }

    retSequenceNumber = write( newVal, lockRequest );
    return true;
}

bool MpEquipmentConfig::validate( Data& values ) const noexcept
{
    bool modified = false;

    switch ( values.iduType )
    {
    case Storm::Type::IduType::eAIR_HANDLER:
        if ( values.numIduHeatingStages > OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES )
        {
            values.numIduHeatingStages = OPTION_STORM_MAX_ELECTRIC_HEATING_STAGES;
            modified                   = true;
        }
        break;

    case Storm::Type::IduType::eFURNACE:
        if ( values.numIduHeatingStages > OPTION_STORM_MAX_FURNACE_HEATING_STAGES )
        {
            values.numIduHeatingStages = OPTION_STORM_MAX_FURNACE_HEATING_STAGES;
            modified                   = true;
        }
        break;

        // Unsupported Indoor unit type
    default:
        values.iduType             = Storm::Type::IduType::eFURNACE;
        values.numIduHeatingStages = 0;
        break;
    }

    switch ( values.oduType )
    {
    case Storm::Type::OduType::eAC:
        if ( values.numCompStages > OPTION_STORM_MAX_COMPRESSOR_COOLING_STAGES )
        {
            values.numCompStages = OPTION_STORM_MAX_COMPRESSOR_COOLING_STAGES;
            modified             = true;
        }
        break;

    case Storm::Type::OduType::eHP:
        if ( values.numCompStages > OPTION_STORM_MAX_COMPRESSOR_HEATING_STAGES )
        {
            values.numCompStages = OPTION_STORM_MAX_COMPRESSOR_HEATING_STAGES;
            modified             = true;
        }
        break;

          // Unsupported Outdoor unit type
    default:
        values.oduType       = Storm::Type::OduType::eAC;
        values.numCompStages = 0;
        break;
    }

    return modified;
}

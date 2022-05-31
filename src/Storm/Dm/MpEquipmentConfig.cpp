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
                                      const char*              symbolicName,
                                      Storm::Type::IduType     iduUnitType,
                                      bool                     hasVspBlower,
                                      uint16_t                 numIduHeatingStages,
                                      Storm::Type::OduType     oduUnitType,
                                      uint16_t                 numCompressorStages )
    : ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
    , m_data( { iduUnitType, oduUnitType, numCompressorStages, numIduHeatingStages, hasVspBlower } )
{
    validate( m_data );
}

///////////////////////////////////////////////////////////////////////////////
void MpEquipmentConfig::hookSetInvalid() noexcept
{
    // Reset the default values when being invalidated. This ensure proper 
    // behavior when just updating a single field.
    m_data.iduType             = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDUTYPE;
    m_data.oduType             = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_ODUTYPE;
    m_data.hasVspBlower        = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_VSPBLOWER;
    m_data.numIduHeatingStages = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDU_NUM_STAGES;
    m_data.numCompStages       = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_COMP_NUM_STAGES;
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


void MpEquipmentConfig::attach( Observer & observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpEquipmentConfig::detach( Observer & observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

///////////////////////////////////////////////////////////////////////////////
const char* MpEquipmentConfig::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpEquipmentConfig";
}

void MpEquipmentConfig::setJSONVal( JsonDocument& doc ) noexcept
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


bool MpEquipmentConfig::fromJSON_( JsonVariant & src, LockRequest_T lockRequest, uint16_t & retSequenceNumber, Cpl::Text::String * errorMsg ) noexcept
{
    Data newVal = m_data;

    // Parse Type field
    const char* enumVal = src["iduType"];
    if ( enumVal )
    {
        // Convert the text to an enum value
        auto maybeValue = Storm::Type::IduType::_from_string_nothrow( enumVal );
        if ( maybeValue )
        {

            newVal.iduType = *maybeValue;
        }
    }

    // Parse Indoor Fan Motor option
    JsonVariant vspBlower = src["vspBlower"];
    if ( vspBlower.isNull() == false )
    {
        newVal.hasVspBlower = vspBlower.as<bool>();
    }

    // Parse number of Indoor stages
    if ( src["numIduHeat"].is<int>() )
    {
        int stages = src["numIduHeat"] | -1;
        if ( stages >= 0 )
        {
            newVal.numIduHeatingStages = (uint16_t) stages;
        }
    }

    // Parse Type field
    enumVal = src["oduType"];
    if ( enumVal )
    {
        // Convert the text to an enum value
        auto maybeValue = Storm::Type::OduType::_from_string_nothrow( enumVal );
        if ( maybeValue )
        {

            newVal.oduType = *maybeValue;
        }
    }

    // Parse number of Compressor stages
    if ( src["numCompStages"].is<int>() )
    {
        int stages = src["numCompStages"] | -1;
        if ( stages >= 0 )
        {
            newVal.numCompStages = (uint16_t) stages;
        }
    }
    
    validate( newVal );
    retSequenceNumber = write( newVal, lockRequest );
    return true;
}

bool MpEquipmentConfig::validate( Data & values ) const noexcept
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

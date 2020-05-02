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


#include "MpComfortConfig.h"
#include "Storm/Utils/DutyCycle.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include "Storm/Constants.h"
#include <string.h>

#define SECT_   "Storm::Dm"

///
using namespace Storm::Dm;

static void setDefaults( Storm::Type::ComfortStageParameters_T& parms )
{
    parms.cph        = OPTION_STORM_DEFAULT_CPH;
    parms.minOffTime = OPTION_STORM_DEFAULT_MIN_OFF_CYCLE_TIME;
    parms.minOnTime  = OPTION_STORM_DEFAULT_MIN_ON_CYCLE_TIME;
}

///////////////////////////////////////////////////////////////////////////////

MpComfortConfig::MpComfortConfig( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
    : ModelPointCommon_( myModelBase, &m_data, staticInfo, MODEL_POINT_STATE_VALID )
{
    memset( &m_data, 0, sizeof( m_data ) ); // Set all potential 'pad bytes' to zero so memcmp() will work correctly
    setDefaults( m_data.compressorCooling );
    setDefaults( m_data.compressorHeating );
    setDefaults( m_data.indoorHeating );
}

///////////////////////////////////////////////////////////////////////////////
uint16_t MpComfortConfig::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    // Reset the default values when being invalidated. This ensure proper 
    // behavior when just updating a single field.
    setDefaults( m_data.compressorCooling );
    setDefaults( m_data.compressorHeating );
    setDefaults( m_data.indoorHeating );

    // Return the sequence number
    uint16_t result  = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );
    m_modelDatabase.unlock_();
    return result;
}

int8_t MpComfortConfig::read( Storm::Type::ComfortConfig_T& configuration, uint16_t* seqNumPtr ) const noexcept
{
    return ModelPointCommon_::read( &configuration, sizeof( Storm::Type::ComfortConfig_T ), seqNumPtr );
}

uint16_t MpComfortConfig::write( Storm::Type::ComfortConfig_T& newConfiguration, LockRequest_T lockRequest ) noexcept
{
    validate( newConfiguration );
    return ModelPointCommon_::write( &newConfiguration, sizeof( Storm::Type::ComfortConfig_T ), lockRequest );
}

uint16_t MpComfortConfig::writeCompressorCooling( Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Storm::Type::ComfortConfig_T src = m_data;
    src.compressorCooling            = newParameters;
    uint16_t result                  = write( src, lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

uint16_t MpComfortConfig::writeCompressorHeating( Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Storm::Type::ComfortConfig_T src = m_data;
    src.compressorHeating            = newParameters;
    uint16_t result                  = write( src, lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

uint16_t MpComfortConfig::writeIndoorHeating( Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    Storm::Type::ComfortConfig_T src = m_data;
    src.indoorHeating                = newParameters;
    uint16_t result                  = write( src, lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

uint16_t MpComfortConfig::readModifyWrite( Client & callbackClient, LockRequest_T lockRequest )
{
    m_modelDatabase.lock_();
    uint16_t result = ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );

    // Ensure the limits are enforced
    // NOTE: In theory this method only needs to be called when there was a
    //       write operation.  However, if the data was just 'read' or invalidated,
    //       enforcing the range limits is either a NOP or a don't care
    //       respectively.
    validate( m_data );

    m_modelDatabase.unlock_();
    return result;
}

void MpComfortConfig::attach( Observer & observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpComfortConfig::detach( Observer & observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

bool MpComfortConfig::isDataEqual_( const void* otherData ) const noexcept
{
    return memcmp( &m_data, otherData, sizeof( m_data ) ) == 0;
}

void MpComfortConfig::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Storm::Type::ComfortConfig_T ) );
    Storm::Type::ComfortConfig_T* dstDataPtr   = ( Storm::Type::ComfortConfig_T* ) dstData;
    *dstDataPtr        = m_data;
}

void MpComfortConfig::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Storm::Type::ComfortConfig_T ) );
    Storm::Type::ComfortConfig_T* dataSrcPtr   = ( Storm::Type::ComfortConfig_T* ) srcData;
    m_data             = *dataSrcPtr;
}


///////////////////////////////////////////////////////////////////////////////
const char* MpComfortConfig::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpComfortConfig";
}

size_t MpComfortConfig::getSize() const noexcept
{
    return sizeof( Storm::Type::ComfortConfig_T );
}

size_t MpComfortConfig::getInternalDataSize_() const noexcept
{
    return sizeof( Storm::Type::ComfortConfig_T );
}


const void* MpComfortConfig::getImportExportDataPointer_() const noexcept
{
    return &m_data;
}

static void buildEntry( JsonObject& obj, Storm::Type::ComfortStageParameters_T& parms )
{
    Storm::Type::Cph cph = Storm::Type::Cph::_from_integral_unchecked( parms.cph );
    obj["cph"]           = cph._to_string();
    obj["minOn"]         = parms.minOnTime;
    obj["minOff"]        = parms.minOffTime;
}

bool MpComfortConfig::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
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
        JsonObject valObj         = doc.createNestedObject( "val" );
        JsonObject coolObj        = valObj.createNestedObject( "cmpCool" );
        buildEntry( coolObj, m_data.compressorCooling );
        JsonObject heatObj        = valObj.createNestedObject( "cmpHeat" );
        buildEntry( heatObj, m_data.compressorHeating );
        JsonObject indoorObj      = valObj.createNestedObject( "indoorHeat" );
        buildEntry( indoorObj, m_data.indoorHeating );
    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose );

    m_modelDatabase.unlock_();
    return true;
}

static bool parseEntry( JsonObject& obj, Storm::Type::ComfortStageParameters_T& parms, Cpl::Text::String * errorMsg, const char* field )
{
    const char* enumVal = obj["cph"];
    if ( enumVal != 0 )
    {
        auto maybeValue = Storm::Type::Cph::_from_string_nothrow( enumVal );
        if ( !maybeValue )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Invalid %s CPH enum value (%s)", field, enumVal );
            }
            return false;
        }

        parms.cph = *maybeValue;
    }
    parms.minOnTime  = obj["minOn"] | parms.minOnTime;
    parms.minOffTime = obj["minOff"] | parms.minOffTime;
    return true;
}

bool MpComfortConfig::fromJSON_( JsonVariant & src, LockRequest_T lockRequest, uint16_t & retSequenceNumber, Cpl::Text::String * errorMsg ) noexcept
{
    Storm::Type::ComfortConfig_T newVal = m_data;

    // COOLING operation
    JsonObject cmpCooling = src["cmpCool"];
    if ( !parseEntry( cmpCooling, newVal.compressorCooling, errorMsg, "CmpCooling" ) )
    {
        return false;
    }

    // Compressor HEATING operation
    JsonObject cmpHeating = src["cmpHeat"];
    if ( !parseEntry( cmpHeating, newVal.compressorHeating, errorMsg, "CmpHeating" ) )
    {
        return false;
    }

    // Indoor HEATING operation
    JsonObject iduHeating = src["indoorHeat"];
    if ( !parseEntry( iduHeating, newVal.indoorHeating, errorMsg, "IduHeating" ) )
    {
        return false;
    }

    retSequenceNumber = write( newVal, lockRequest );
    return true;
}

bool MpComfortConfig::validate( Storm::Type::ComfortConfig_T& values ) const noexcept
{
    bool modified = false;

    modified |= validate( values.compressorCooling );
    modified |= validate( values.compressorHeating );
    modified |= validate( values.indoorHeating );

    return modified;
}

bool MpComfortConfig::validate( Storm::Type::ComfortStageParameters_T& src ) const noexcept
{
    bool modified = false;

    if ( src.cph >= Storm::Type::Cph::eNUM_OPTIONS || src.cph < Storm::Type::Cph::e2CPH )
    {
        modified = true;
        src.cph  = Storm::Type::Cph::e3CPH;
    }

    uint32_t limit = Storm::Utils::DutyCycle::getMaximumMinOffTime( Storm::Type::Cph::_from_integral_unchecked( src.cph ) );
    if ( src.minOffTime > limit )
    {
        src.minOffTime = limit;
        modified       = true;
    }

    limit = Storm::Utils::DutyCycle::getMaximumMinOnTime( Storm::Type::Cph::_from_integral_unchecked( src.cph ) );
    if ( src.minOnTime > limit )
    {
        src.minOnTime = limit;
        modified      = true;
    }

    return modified;
}



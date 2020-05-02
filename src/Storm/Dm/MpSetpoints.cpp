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


#include "MpSetpoints.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Math/real.h"
#include <string.h>

///
using namespace Storm::Dm;


///////////////////////////////////////////////////////////////////////////////
MpSetpoints::MpSetpoints( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, float coolSetpt, float heatSetpt )
    : ModelPointCommon_( myModelBase, &m_data, staticInfo, MODEL_POINT_STATE_VALID )
    , m_data( { coolSetpt, heatSetpt } )
{
    validateSetpoints( m_data.coolSetpt, m_data.heatSetpt, m_data.coolSetpt, m_data.heatSetpt );
}

void MpSetpoints::validateSetpoints( float newCooling, float newHeating, float& finalCooling, float& finalHeating )
{
    // Enforce Cooling is ALWAYS higher than the heating setpoint
    if ( newCooling < newHeating )
    {
        newHeating = newCooling;
    }

    // Enforce cooling range
    if ( newCooling > OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING )
    {
        newCooling = OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING;
    }
    else if ( newCooling < OPTION_STORM_DM_MP_SETPOINTS_MIN_COOLING )
    {
        newCooling = OPTION_STORM_DM_MP_SETPOINTS_MIN_COOLING;
    }

    // Enforce heating range
    if ( newHeating > OPTION_STORM_DM_MP_SETPOINTS_MAX_HEATING )
    {
        newHeating = OPTION_STORM_DM_MP_SETPOINTS_MAX_HEATING;
    }
    else if ( newHeating < OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING )
    {
        newHeating = OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING;
    }

    // Update results
    finalCooling = newCooling;
    finalHeating = newHeating;
}

///////////////////////////////////////////////////////////////////////////////
uint16_t MpSetpoints::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    // Reset the cooling/heating set-point to their 'safe' values when being
    // invalidated. This ensure proper behavior when just write ONE set-point.
    m_data.coolSetpt = OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING;
    m_data.heatSetpt = OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING;

    uint16_t result  = ModelPointCommon_::setInvalidState( newInvalidState, lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

int8_t MpSetpoints::read( float& currentCoolSetpoint, float& currentHeatSetpoint, uint16_t * seqNumPtr ) const noexcept
{
    Data   dst;
    int8_t valid        = ModelPointCommon_::read( &dst, sizeof( Data ), seqNumPtr );
    currentCoolSetpoint = dst.coolSetpt;
    currentHeatSetpoint = dst.heatSetpt;
    return valid;
}

int8_t MpSetpoints::readCool( float& currentCoolSetpoint, uint16_t * seqNumPtr ) const noexcept
{
    Data   dst;
    int8_t valid = ModelPointCommon_::read( &dst, sizeof( Data ), seqNumPtr );
    currentCoolSetpoint = dst.coolSetpt;
    return valid;
}

int8_t MpSetpoints::readHeat( float& currentHeatSetpoint, uint16_t * seqNumPtr ) const noexcept
{
    Data   dst;
    int8_t valid        = ModelPointCommon_::read( &dst, sizeof( Data ), seqNumPtr );
    currentHeatSetpoint = dst.heatSetpt;
    return valid;
}

uint16_t MpSetpoints::write( float newCoolingSetpoint, float newHeatingSetpoint, LockRequest_T lockRequest ) noexcept
{
    validateSetpoints( newCoolingSetpoint, newHeatingSetpoint, newCoolingSetpoint, newHeatingSetpoint );
    Data src = { newCoolingSetpoint, newHeatingSetpoint };
    return ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );
}

uint16_t MpSetpoints::writeCool( float newCoolingSetpoint, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    validateSetpoints( newCoolingSetpoint, m_data.heatSetpt, newCoolingSetpoint, m_data.heatSetpt );
    Data src = { newCoolingSetpoint, m_data.heatSetpt };
    uint16_t result = ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );

    m_modelDatabase.unlock_();
    return result;
}

uint16_t MpSetpoints::writeHeat( float newHeatingSetpoint, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();

    validateSetpoints( m_data.coolSetpt, newHeatingSetpoint, m_data.coolSetpt, newHeatingSetpoint );
    Data src = { m_data.coolSetpt, newHeatingSetpoint };
    uint16_t result = ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );

    m_modelDatabase.unlock_();
    return result;
}
uint16_t MpSetpoints::readModifyWrite( Client & callbackClient, LockRequest_T lockRequest )
{
    m_modelDatabase.lock_();
    uint16_t result = ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );

    // Ensure the set-points stay within valid ranges.
    // NOTE: In theory this method only needs to be called when there was a
    //       operation.  However, if the data was just 'read' or invalidated,
    //       enforcing the range limits is either a NOP or a don't care
    //       respectively.
    validateSetpoints( m_data.coolSetpt, m_data.heatSetpt, m_data.coolSetpt, m_data.heatSetpt );

    m_modelDatabase.unlock_();
    return result;
}

void MpSetpoints::attach( Observer & observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void MpSetpoints::detach( Observer & observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

bool MpSetpoints::isDataEqual_( const void* otherData ) const noexcept
{
    Data* otherDataPtr = ( Data*) otherData;

    return Cpl::Math::areFloatsEqual( m_data.coolSetpt, otherDataPtr->coolSetpt ) &&
        Cpl::Math::areFloatsEqual( m_data.heatSetpt, otherDataPtr->heatSetpt );

}

void MpSetpoints::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize == sizeof( Data ) );
    Data* dstDataPtr   = ( Data*) dstData;
    *dstDataPtr        = m_data;
}

void MpSetpoints::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize == sizeof( Data ) );
    Data* dataSrcPtr   = ( Data*) srcData;
    m_data             = *dataSrcPtr;
}


///////////////////////////////////////////////////////////////////////////////
const char* MpSetpoints::getTypeAsText() const noexcept
{
    return "Storm::Dm::MpSetpoints";
}

size_t MpSetpoints::getSize() const noexcept
{
    return sizeof( Data );
}

size_t MpSetpoints::getInternalDataSize_() const noexcept
{
    return sizeof( Data );
}


const void* MpSetpoints::getImportExportDataPointer_() const noexcept
{
    return &m_data;
}

bool MpSetpoints::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
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
        JsonObject valObj = doc.createNestedObject( "val" );
        valObj["cool"] = m_data.coolSetpt;
        valObj["heat"] = m_data.heatSetpt;
    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose );

    m_modelDatabase.unlock_();
    return true;
}

bool MpSetpoints::fromJSON_( JsonVariant & src, LockRequest_T lockRequest, uint16_t & retSequenceNumber, Cpl::Text::String * errorMsg ) noexcept
{
    float cool         = m_data.coolSetpt;
    float heat         = m_data.heatSetpt;
    int   missingCount = 0;

    // Parse Fields
    cool = src["cool"] | -1.0F;
    if ( Cpl::Math::areFloatsEqual( cool, -1.0F ) )
    {
        missingCount++;
        cool = m_data.coolSetpt;
    }
    heat = src["heat"] | -1.0F;
    if ( Cpl::Math::areFloatsEqual( heat, -1.0F ) )
    {
        heat = m_data.heatSetpt;
        missingCount++;
    }

    // Throw an error if NO valid key/value pairs where specified
    if ( missingCount == 2 )
    {
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }

    retSequenceNumber = write( cool, heat, lockRequest );
    return true;
}


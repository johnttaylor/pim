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


#include "RefCounter.h"
#include "Cpl/Text/atob.h"
#include <limits.h>

///
using namespace Cpl::Dm::Mp;

///////////////////////////////////////////////////////////////////////////////
RefCounter::RefCounter( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
    :Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
{
}

/// Constructor. Valid MP.  Requires an initial value
RefCounter::RefCounter( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, uint32_t initialValue )
    : Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
{
    m_data = initialValue;
}

///////////////////////////////////////////////////////////////////////////////
void RefCounter::updateAndCheckForChangeNotification( uint32_t newValue )
{
    // Generate change notices on transition to valid OR zero-to-not-zero OR not-zero-to-zero
    if ( !m_valid || (m_data == 0 && newValue != 0) || (newValue == 0 && m_data != 0) )
    {
        processDataUpdated();
    }
    m_data = newValue;
}


uint16_t RefCounter::reset( uint32_t newValue, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        // Generate change notices on transition to valid OR zero-to-not-zero
        updateAndCheckForChangeNotification( newValue );
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t RefCounter::increment( uint32_t incrementAmount, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        // Increment the counter and prevent overflow
        uint32_t newValue = m_data + incrementAmount;
        if ( newValue < m_data )
        {
            newValue = (uint32_t) -1;
        }

        // Generate change notices on transition to valid OR zero-to-not-zero
        updateAndCheckForChangeNotification( newValue );
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t RefCounter::decrement( uint32_t decrementAmount, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        // Decrement the counter and prevent underflow
        uint32_t newValue = m_data - decrementAmount;
        if ( newValue > m_data )
        {
            newValue = 0;
        }

        // Generate change notices on transition to valid OR zero-to-not-zero
        updateAndCheckForChangeNotification( newValue );
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

///////////////////////////////////////////////////////////////////////////////
void RefCounter::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void RefCounter::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

const char* RefCounter::getTypeAsText() const noexcept
{
    return "Cpl::Dm::Mp::RefCounter";
}

///////////////////////////////////////////////////////////////////////////////
void RefCounter::setJSONVal( JsonDocument& doc ) noexcept
{
    doc["val"] = m_data;
}

bool RefCounter::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    // Parse as a numeric -->i.e. 'set a explicit value'
    if ( src.is<uint32_t>() )
    {
        retSequenceNumber = reset( src.as<uint32_t>(), lockRequest );
        return true;
    }

    // Attempt to parse the 'action' -->MUST BE A STRING at this point!
    const char* jsonValue = src;
    if ( jsonValue == nullptr )
    {
        if ( errorMsg )
        {
            *errorMsg = "Missing 'val' key/value pair";
        }
        return false;
    }

    // Increment action
    if ( jsonValue[0] == '+' )
    {
        unsigned long numValue;
        if ( Cpl::Text::a2ul( numValue, jsonValue + 1 ) == false )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid/bad syntax for the 'val' key/value pair";
            }
            return false;
        }
        retSequenceNumber = increment( (uint32_t) numValue, lockRequest );
        return true;
    }

    // Decrement action
    if ( jsonValue[0] == '-' )
    {
        unsigned long numValue;
        if ( Cpl::Text::a2ul( numValue, jsonValue + 1 ) == false )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid/bad syntax for the 'val' key/value pair";
            }
            return false;
        }
        retSequenceNumber = decrement( (uint32_t) numValue, lockRequest );
        return true;
    }

    // Bad syntax
    if ( errorMsg )
    {
        *errorMsg = "Invalid/bad syntax for the 'val' key/value pair";
    }
    return false;
}



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

#include "ModelDatabase.h"
#include "ModelPoint.h"
#include "Cpl/Container/Key.h"
#include <new>

///
using namespace Cpl::Dm;

// global lock
static Cpl::System::Mutex globalMutex_;
StaticJsonDocument<OPTION_CPL_DM_MODEL_DATABASE_MAX_CAPACITY_JSON_DOC> ModelDatabase::g_doc_;

//////////////////////////////////////////////
ModelDatabase::ModelDatabase() noexcept
    : m_map()
    , m_lock( 0 )
{
    createLock();
}

ModelDatabase::ModelDatabase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
    : m_map( ignoreThisParameter_usedToCreateAUniqueConstructor )
{
}

ModelDatabase::~ModelDatabase() noexcept
{
    delete m_lock;
}

//////////////////////////////////////////////
bool ModelDatabase::createLock()
{
    m_lock = new(std::nothrow) Cpl::System::Mutex();
    if ( m_lock == 0 )
    {
        return false;
    }
    return true;
}

ModelPoint * ModelDatabase::lookupModelPoint( const char* modelPointName ) noexcept
{
    Cpl::Container::KeyLiteralString myKey( modelPointName );
    lock_();
    ModelPoint* result = m_map.find( myKey );
    unlock_();
    return result;
}

ModelPoint* ModelDatabase::getFirstByName() noexcept
{
    lock_();
    ModelPoint* result = m_map.first();
    unlock_();
    return result;
}

ModelPoint* ModelDatabase::getNextByName( ModelPoint& currentModelPoint ) noexcept
{
    lock_();
    ModelPoint* result = m_map.next( currentModelPoint );
    unlock_();
    return result;
}

void ModelDatabase::insert_( ModelPoint& mpToAdd ) noexcept
{
    lock_();
    m_map.insert( mpToAdd );
    unlock_();
}

void ModelDatabase::lock_() noexcept
{
    if ( m_lock )
    {
        m_lock->lock();
    }
    else
    {
        if ( createLock() )
        {
            m_lock->lock();
        }
    }
}

void ModelDatabase::unlock_() noexcept
{
    if ( m_lock )
    {
        m_lock->unlock();
    }
}

void ModelDatabase::globalLock_() noexcept
{
    globalMutex_.lock();
}

void ModelDatabase::globalUnlock_() noexcept
{
    globalMutex_.unlock();
}

bool ModelDatabase::fromJSON( const char* src, Cpl::Text::String* errorMsg, ModelPoint** retMp, uint16_t* retSequenceNumber ) noexcept
{
    // Get access to the Global JSON document
    ModelDatabase::globalLock_();

    // Parse the JSON payload...
    DeserializationError err = deserializeJson( ModelDatabase::g_doc_, src );
    if ( err )
    {
        if ( errorMsg )
        {
            *errorMsg = err.c_str();
        }
        return false;
    }

    //    { name="<mpname>", type="<mptypestring>", valid=nn, seqnum:nnnn, locked:true|false val:<value> }

    // Valid JSON... Parse the Model Point name
    const char* name = ModelDatabase::g_doc_["name"];
    if ( name == nullptr )
    {
        if ( errorMsg )
        {
            *errorMsg = "No valid 'name' key in the JSON input.";
        }
        return false;
    }

    // Look-up the Model Point name
    ModelPoint* mp = lookupModelPoint( name );
    if ( mp == 0 )
    {
        if ( errorMsg )
        {
            errorMsg->format( "Model Point name (%s) NOT found.", name );
        }
        return false;
    }

    // Attempt to parse the key/value pairs of interest
    int8_t                    valid      = ModelDatabase::g_doc_["invalid"] | -1;
    JsonVariant               locked     = ModelDatabase::g_doc_["locked"];
    JsonVariant               valElem    = ModelDatabase::g_doc_["val"];
    ModelPoint::LockRequest_T lockAction = ModelPoint::eNO_REQUEST;
    if ( locked.isNull() == false )
    {
        lockAction = locked.as<bool>() ? ModelPoint::eLOCK : ModelPoint::eUNLOCK;
    }

    // Request to invalidate the MP
    uint16_t seqnum = 0;
    if ( valid > 0 )
    {
        seqnum = mp->setInvalidState( valid, lockAction );
    }

    // Write a valid value to the MP
    else if ( valElem.isNull() == false )
    {
        if ( mp->fromJSON_( valElem, lockAction, seqnum, errorMsg ) == false )
        {
            return false;
        }
    }

    // Just lock/unlock the MP
    else if ( locked.isNull() == false )
    {
        seqnum = mp->setLockState( lockAction );
    }

    // Bad Syntax
    else
    {
        if ( errorMsg )
        {
            *errorMsg = "JSON syntax is not valid or invalid payload semantics";
        }
        return false;
    }

    // Release the Global JSON document
    ModelDatabase::globalUnlock_();

    // Return the sequence number (when requested)
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    // Return the model point instance (when requested)
    if ( retMp )
    {
        *retMp = mp;
    }

    return true;
}

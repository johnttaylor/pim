/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
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
    : m_list()
    , m_lock( 0 )
    , m_listSorted( false )
{
    createLock();
}

ModelDatabase::ModelDatabase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
    : m_list( ignoreThisParameter_usedToCreateAUniqueConstructor )
    , m_listSorted( false )
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
    lock_();
    ModelPoint* result = find( modelPointName );
    unlock_();
    return result;
}

ModelPoint* ModelDatabase::getFirstByName() noexcept
{
    lock_();
    
    // Only sort the list ONCE!
    if ( !m_listSorted )
    {
        sortList();
        m_listSorted = true;
    }
    ModelPoint* result = m_list.first();
    unlock_();
    return result;
}

ModelPoint* ModelDatabase::getNextByName( ModelPoint& currentModelPoint ) noexcept
{
    lock_();
    ModelPoint* result = m_list.next( currentModelPoint );
    unlock_();
    return result;
}

void ModelDatabase::insert_( ModelPoint& mpToAdd ) noexcept
{
    lock_();
    m_list.putFirst( mpToAdd );
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
    JsonVariant               validElem  = ModelDatabase::g_doc_["valid"];
    JsonVariant               lockedElem = ModelDatabase::g_doc_["locked"];
    JsonVariant               valElem    = ModelDatabase::g_doc_["val"];
    uint16_t                  seqnum     = 0;
    ModelPoint::LockRequest_T lockAction = ModelPoint::eNO_REQUEST;
    bool                      parsed     = false;

    // Lock/Unlock the MP
    if ( !lockedElem.isNull() )
    {
        lockAction = lockedElem.as<bool>() ? ModelPoint::eLOCK : ModelPoint::eUNLOCK;
        seqnum     = mp->setLockState( lockAction ); // Note: This handles the use case of locking/unlock when the 'val' key/pair was not specified
        parsed     = true;
    }

    // Request to invalidate the MP
    if ( !validElem.isNull() && validElem.as<bool>() == false )
    {
        seqnum = mp->setInvalid( lockAction );
        parsed     = true;
    }

    // Write a valid value to the MP
    else if ( valElem.isNull() == false )
    {
        if ( mp->fromJSON_( valElem, lockAction, seqnum, errorMsg ) == false )
        {
            return false;
        }
        parsed = true;
    }

    // Bad Syntax
    if ( !parsed )
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

void ModelDatabase::sortList() noexcept
{
    // This a BRUTE force sort, because I am lazy (well just in hurry for change)

    // Seed the sorted list with it
    Cpl::Container::SList<ModelPoint> sortedList;
    ModelPoint* item  = m_list.getFirst();
    if ( item == nullptr )
    {
        return; // the unsorted list is empty
    }
    sortedList.putFirst( *item );


    // Walk the unsorted list
    item = m_list.getFirst();
    while ( item )
    {
        bool        moved          = false;
        ModelPoint* prevSortedItem = nullptr;
        ModelPoint* sortedItem     = sortedList.first();
        while ( sortedItem )
        {
            // Insert before if new item is 'smaller' then the current sorted item
            if ( strcmp( item->getName(), sortedItem->getName() ) < 0 )
            {
                if ( prevSortedItem )
                {
                    sortedList.insertAfter( *prevSortedItem, *item );
                }
                else
                {
                    sortedList.putFirst( *item );
                }
                moved = true;
                break;
            }
            prevSortedItem = sortedItem;
            sortedItem     = m_list.next( *sortedItem );
        }

        // new item is 'larger' than all items in the sorted listed
        if ( !moved )
        {
            sortedList.putLast( *item );
        }

        item = m_list.getFirst();
    }

    // Make the unsorted list THE list
    sortedList.move( m_list );
}

ModelPoint* ModelDatabase::find( const char* name ) noexcept
{
    ModelPoint* item  = m_list.first();
    while ( item )
    {
        if ( strcmp( item->getName(), name ) == 0 )
        {
            return item;
        }
        item = m_list.next( *item );
    }

    return nullptr;
}
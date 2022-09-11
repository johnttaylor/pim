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

#include "ModelPointCommon_.h"
#include "MailboxServer.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/atob.h"
#include "Cpl/System/Assert.h"

///
using namespace Cpl::Dm;

/// SubscriberApi States
enum State_T
{
    eSTATE_UNSUBSCRIBED = 0,          // Not subscribed to any model point
    eSTATE_IDLE,                      // Subscribed and waiting for a change
    eSTATE_NOTIFY_PENDING,            // Subscribed and waiting for next change notification dispatch cycle
    eSTATE_NOTIFY_NOTIFYING,          // The Client change notification callback is being executed
    eSTATE_NOTIFY_PENDING_DETACH,     // The subscription was requested to be cancelled during the change notification callback 
};


////////////////////////
ModelPointCommon_::ModelPointCommon_( ModelDatabase& myModelBase, 
                                      const char*    symbolicName, 
                                      void*          myDataPtr, 
                                      size_t         dataSizeInBytes, 
                                      bool           isValid )
    : m_name( symbolicName )
    , m_modelDatabase( myModelBase )
    , m_dataPtr( myDataPtr )
    , m_dataSize( dataSizeInBytes )
    , m_seqNum( SEQUENCE_NUMBER_UNKNOWN + 1          )
    , m_locked( false )
    , m_valid( isValid )
{
    // Automagically add myself to the Model Database
    myModelBase.insert_( *this );


    // Make sure that I process the 'transition' to the invalid state
    if ( !m_valid )
    {
        hookSetInvalid();  
    }
}

/////////////////
const char* ModelPointCommon_::getName() const noexcept
{
    return m_name;
}

size_t ModelPointCommon_::getSize() const noexcept
{
    return m_dataSize;
}

uint16_t ModelPointCommon_::getSequenceNumber() const noexcept
{
    m_modelDatabase.lock_();
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();
    return result;
}

bool ModelPointCommon_::isNotValid( void ) const noexcept
{
    m_modelDatabase.lock_();
    bool result = m_valid;
    m_modelDatabase.unlock_();
    return !result;
}

uint16_t ModelPointCommon_::setInvalid( LockRequest_T lockRequest ) noexcept
{

    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        if ( m_valid )
        {
            m_valid = false;
            hookSetInvalid();
            processChangeNotifications();
        }
    }

    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();
    return result;
}

void ModelPointCommon_::hookSetInvalid() noexcept
{
    // Set the data to a known state so that transition from the invalid to the 
    // valid state when using read-modify-write operation is consistent in its behavior
    memset( m_dataPtr, 0, m_dataSize );
}

bool ModelPointCommon_::readData( void* dstData, size_t dstSize, uint16_t* seqNumPtr ) const noexcept
{
    m_modelDatabase.lock_();
    bool valid = m_valid;
    if ( dstData && valid )
    {
        copyDataTo_( dstData, dstSize );
    }
    if ( seqNumPtr )
    {
        *seqNumPtr = m_seqNum;
    }
    m_modelDatabase.unlock_();

    return valid;
}

uint16_t ModelPointCommon_::writeData( const void* srcData, size_t srcSize, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();
    if ( srcData && testAndUpdateLock( lockRequest ) )
    {
        if ( !m_valid || isDataEqual_( srcData ) == false )
        {
            copyDataFrom_( srcData, srcSize );
            processDataUpdated();
        }
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t ModelPointCommon_::copyDataAndStateFrom( const ModelPointCommon_& src, LockRequest_T lockRequest ) noexcept
{
    // Handle the src.invalid case
    if ( src.isNotValid() )
    {
        return setInvalid();
    }

    m_modelDatabase.lock_();
    uint16_t seqNum = writeData( src.m_dataPtr, src.m_dataSize, lockRequest );
    m_modelDatabase.unlock_();
    return seqNum;
}

uint16_t ModelPointCommon_::touch() noexcept
{
    m_modelDatabase.lock_();
    processChangeNotifications();
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();
    return result;
}

void ModelPointCommon_::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    CPL_SYSTEM_ASSERT( dstSize <= m_dataSize);
    memcpy( dstData, m_dataPtr, dstSize );
}

void ModelPointCommon_::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    CPL_SYSTEM_ASSERT( srcSize <= m_dataSize );
    memcpy( m_dataPtr, srcData, m_dataSize );
}

bool ModelPointCommon_::isDataEqual_( const void* otherData ) const noexcept
{
    return memcmp( m_dataPtr, otherData, m_dataSize ) == 0;
}

const void* ModelPointCommon_::getImportExportDataPointer_() const noexcept
{
    return m_dataPtr;
}

size_t ModelPointCommon_::getInternalDataSize_() const noexcept
{
    return getSize();
}

/////////////////
bool ModelPointCommon_::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose, bool pretty ) noexcept
{
    // Get a snapshot of the my data and state
    m_modelDatabase.lock_();

    // Start the conversion
    JsonDocument& doc = beginJSON( m_valid, m_locked, m_seqNum, verbose );

    // Construct the 'val' key/value pair (as a simple numeric)
    if ( m_valid )
    {
        setJSONVal( doc );
    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose, pretty );
    m_modelDatabase.unlock_();
    return true;
}

/////////////////
uint16_t ModelPointCommon_::setLockState( LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();
    if ( lockRequest == eLOCK )
    {
        m_locked = true;
    }
    else if ( lockRequest == eUNLOCK )
    {
        m_locked = false;
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();
    return result;
}

bool ModelPointCommon_::isLocked() const noexcept
{
    m_modelDatabase.lock_();
    bool result = m_locked;
    m_modelDatabase.unlock_();
    return result;
}


/////////////////
size_t ModelPointCommon_::exportData( void* dstDataStream, size_t maxDstLength, uint16_t* retSeqNum, bool includeLockedState ) const noexcept
{
    size_t result = 0;
    if ( dstDataStream )
    {
        m_modelDatabase.lock_();

        // Do nothing if there is not enough space left in the destination stream
        if ( maxDstLength >= getExternalSize() )
        {
            // Export metadata (if there is any)
            size_t bytesAdded = 0;
            if ( exportMetadata_( dstDataStream, bytesAdded ) )
            {
                // Export Data
                uint8_t* dstPtr   = ( (uint8_t*)dstDataStream ) + bytesAdded;
                size_t   dataSize = getSize();
                memcpy( dstPtr, getImportExportDataPointer_(), dataSize );

                // Export Valid State
                memcpy( dstPtr + dataSize, &m_valid, sizeof( m_valid ) );

                // Export Locked state
                if ( includeLockedState )
                {
                    memcpy( dstPtr + dataSize + sizeof( m_valid ), &m_locked, sizeof( m_locked ) );
                }

                // Return the Sequence number when requested
                if ( retSeqNum )
                {
                    *retSeqNum = m_seqNum;
                }

                result = getExternalSize( includeLockedState );
            }
        }

        m_modelDatabase.unlock_();
    }
    return result;
}

size_t ModelPointCommon_::importData( const void* srcDataStream, size_t srcLength, uint16_t* retSeqNum, bool includeLockedState ) noexcept
{
    size_t result = 0;
    if ( srcDataStream )
    {
        m_modelDatabase.lock_();

        // Fail the import when there is not enough data left in the input stream
        if ( getExternalSize() <= srcLength )
        {
            // Consume incoming metadata (if there is any)
            size_t bytesConsumed = 0;
            if ( importMetadata_( srcDataStream, bytesConsumed ) )
            {
                // Import Data
                const uint8_t* srcPtr   = ( (const uint8_t*)srcDataStream ) + bytesConsumed;
                size_t   dataSize = getSize();
                memcpy( (void*)getImportExportDataPointer_(), srcPtr, dataSize );

                // Import Valid State
                memcpy( &m_valid, srcPtr + dataSize, sizeof( m_valid ) );

                // Import Locked state
                if ( includeLockedState )
                {
                    memcpy( &m_locked, srcPtr + dataSize + sizeof( m_valid ), sizeof( m_locked ) );
                }

                // Generate change notifications and return the Sequence number when requested
                processChangeNotifications();
                if ( retSeqNum )
                {
                    *retSeqNum = m_seqNum;
                }

                result = getExternalSize( includeLockedState );
            }
        }

        m_modelDatabase.unlock_();
    }
    return result;
}

size_t ModelPointCommon_::getExternalSize( bool includeLockedState ) const noexcept
{
    size_t baseSize =  getInternalDataSize_() + sizeof( m_valid );
    return includeLockedState ? baseSize + sizeof( m_locked ) : baseSize;
}


/////////////////////////////////////
void ModelPointCommon_::processDataUpdated() noexcept
{
    // By definition - Point now has valid date
    m_valid = true;
    processChangeNotifications();
}

void ModelPointCommon_::advanceSequenceNumber() noexcept
{
    // Increment my sequence number (when rolling over -->do not allow the 'unknown' value)
    m_seqNum++;
    if ( m_seqNum == SEQUENCE_NUMBER_UNKNOWN )
    {
        m_seqNum = SEQUENCE_NUMBER_UNKNOWN + 1;
    }
}

void ModelPointCommon_::processChangeNotifications() noexcept
{
    // Increment the sequence number
    advanceSequenceNumber();

    // Generate change notifications 
    SubscriberApi* item = m_subscribers.get();
    while ( item )
    {
        processSubscriptionEvent_( *item, eDATA_CHANGED );
        item = m_subscribers.get();
    }
}

/////////////////
void ModelPointCommon_::attachSubscriber( SubscriberApi& observer, uint16_t initialSeqNumber ) noexcept
{
    m_modelDatabase.lock_();
    observer.setSequenceNumber_( initialSeqNumber );
    observer.setModelPoint_( this );
    processSubscriptionEvent_( observer, eATTACH );
    m_modelDatabase.unlock_();
}

void ModelPointCommon_::detachSubscriber( SubscriberApi& observer ) noexcept
{
    m_modelDatabase.lock_();
    processSubscriptionEvent_( observer, eDETACH );
    observer.setModelPoint_( 0 );
    m_modelDatabase.unlock_();
}

void ModelPointCommon_::genericAttach( SubscriberApi& observer, uint16_t initialSeqNumber) noexcept
{
    attachSubscriber( observer, initialSeqNumber );
}

void ModelPointCommon_::genericDetach( SubscriberApi& observer ) noexcept
{
    detachSubscriber( observer );
}


/////////////////
void ModelPointCommon_::processSubscriptionEvent_( SubscriberApi& subscriber, Event_T event ) noexcept
{
    m_modelDatabase.lock_();

    switch ( (State_T) subscriber.getState_() )
    {
    case eSTATE_UNSUBSCRIBED:
        switch ( event )
        {
        case eATTACH:
            transitionToSubscribed( subscriber );
            break;

        case eDATA_CHANGED:
            Cpl::System::FatalError::logf( "ModelPointCommon_::processSubscriptionEvent_(): Data changed received when in the eSTATE_UNSUBSCRIBED state!" );
            break;

            // Ignore all other events
        default:
            break;
        }
        break;

    case eSTATE_IDLE:
        switch ( event )
        {
        case eATTACH:
            transitionToSubscribed( subscriber );
            break;

        case eDETACH:
            m_subscribers.remove( subscriber );
            subscriber.setState_( eSTATE_UNSUBSCRIBED );
            break;

        case eDATA_CHANGED:
            // NOTE: By definition if the eDATA_CHANGED event was generated - the subscriber is NOT in the MP's subscribers list
            transitionToNotifyPending( subscriber );
            break;

            // Ignore all other events
        default:
            break;
        }
        break;

    case eSTATE_NOTIFY_PENDING:
        switch ( event )
        {
        case eATTACH:
            subscriber.getNotificationApi_()->removePendingChangingNotification_( subscriber );
            transitionToSubscribed( subscriber );
            break;

        case eDETACH:
            subscriber.getNotificationApi_()->removePendingChangingNotification_( subscriber );
            subscriber.setState_( eSTATE_UNSUBSCRIBED );
            break;

        case eNOTIFYING:
            subscriber.setSequenceNumber_( m_seqNum );
            subscriber.setState_( eSTATE_NOTIFY_NOTIFYING );
            break;

        case eDATA_CHANGED:
            Cpl::System::FatalError::logf( "ModelPointCommon_::processSubscriptionEvent_(): Data changed received when in the eSTATE_NOTIFY_PENDING state!" );
            break;

            // Ignore all other events
        default:
            break;
        }
        break;

    case eSTATE_NOTIFY_NOTIFYING:
        switch ( event )
        {
        case eDETACH:
            subscriber.setState_( eSTATE_NOTIFY_PENDING_DETACH );
            break;

        case eNOTIFY_COMPLETE:
            transitionToSubscribed( subscriber );
            break;

        case eDATA_CHANGED:
            Cpl::System::FatalError::logf( "ModelPointCommon_::processSubscriptionEvent_(): Data changed received when in the eSTATE_NOTIFY_NOTIFYING state!" );
            break;

            // Ignore all other events
        default:
            break;
        }
        break;

    case eSTATE_NOTIFY_PENDING_DETACH:
        switch ( event )
        {
        case eATTACH:
            subscriber.setState_( eSTATE_NOTIFY_NOTIFYING );
            break;

        case eNOTIFY_COMPLETE:
            subscriber.setState_( eSTATE_UNSUBSCRIBED );
            break;

        case eDATA_CHANGED:
            Cpl::System::FatalError::logf( "ModelPointCommon_::processSubscriptionEvent_(): Data changed received when in the eSTATE_NOTIFY_PENDING_DETACH state!" );
            break;

            // Ignore all other events
        default:
            break;
        }
        break;
    }

    m_modelDatabase.unlock_();
}

void ModelPointCommon_::transitionToSubscribed( SubscriberApi& subscriber ) noexcept
{
    // Ensure that I am not already in the Model Point's list of subscribers (this can happen if subscribing when I am already subscribed)
    m_subscribers.remove( subscriber );

    if ( m_seqNum == subscriber.getSequenceNumber_() )
    {
        subscriber.setState_( eSTATE_IDLE );
        m_subscribers.put( subscriber );
    }
    else
    {
        return transitionToNotifyPending( subscriber );
    }
}

void ModelPointCommon_::transitionToNotifyPending( SubscriberApi& subscriber ) noexcept
{
    subscriber.getNotificationApi_()->addPendingChangingNotification_( subscriber );
    subscriber.setState_( eSTATE_NOTIFY_PENDING );
}


/////////////////
bool ModelPointCommon_::testAndUpdateLock( LockRequest_T lockRequest ) noexcept
{
    bool result = false;
    if ( lockRequest == eUNLOCK )
    {
        m_locked = false;
        result   = true;
    }
    else if ( lockRequest == eLOCK )
    {
        m_locked = true;
        result   = true;
    }
    else
    {
        if ( m_locked == false )
        {
            result = true;
        }
    }

    return result;
}

/////////////////
JsonDocument& ModelPointCommon_::beginJSON( bool isValid, bool locked, uint16_t seqnum, bool verbose ) noexcept
{
    // Get access to the Global JSON document
    ModelDatabase::globalLock_();
    ModelDatabase::g_doc_.clear();  // Make sure the JSON document is starting "empty"

    // Construct the JSON
    ModelDatabase::g_doc_["name"]  = getName();
    ModelDatabase::g_doc_["valid"] = isValid;
    if ( verbose )
    {
        ModelDatabase::g_doc_["type"]      = getTypeAsText();
        ModelDatabase::g_doc_["seqnum"]    = seqnum;
        ModelDatabase::g_doc_["locked"]    = locked;
    }
    return ModelDatabase::g_doc_;
}

void ModelPointCommon_::endJSON( char* dst, size_t dstSize, bool& truncated, bool verbose, bool pretty ) noexcept
{
    size_t jsonLen;
    size_t outputLen;

    // Generate the actual output string 
    if ( !pretty )
    {
        jsonLen   = measureJson( ModelDatabase::g_doc_ );
        outputLen = serializeJson( ModelDatabase::g_doc_, dst, dstSize );
    }
    else
    {
        jsonLen   = measureJsonPretty( ModelDatabase::g_doc_ );
        outputLen = serializeJsonPretty( ModelDatabase::g_doc_, dst, dstSize );
    }
    truncated = outputLen == jsonLen ? false : true;

    // Release the Global JSON document
    ModelDatabase::globalUnlock_();
}

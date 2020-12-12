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

#include "ModelPointCommon_.h"
#include "MailboxServer.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/atob.h"

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
ModelPointCommon_::ModelPointCommon_( ModelDatabase& myModelBase, void* myDataPtr, StaticInfo& staticInfo, int8_t validState )
    : m_staticInfo( staticInfo )
    , m_modelDatabase( myModelBase )
    , m_dataPtr( myDataPtr )
    , m_seqNum( SEQUENCE_NUMBER_UNKNOWN + 1 )
    , m_locked( false )
    , m_validState( validState )
{
    // Automagically add myself to the Model Database
    myModelBase.insert_( *this );
}

/////////////////
const char* ModelPointCommon_::getName() const noexcept
{
    return m_staticInfo.getName();
}

uint16_t ModelPointCommon_::getSequenceNumber() const noexcept
{
    m_modelDatabase.lock_();
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();
    return result;
}

int8_t ModelPointCommon_::getValidState( void ) const noexcept
{
    m_modelDatabase.lock_();
    int8_t result = m_validState;
    m_modelDatabase.unlock_();
    return result;
}

uint16_t ModelPointCommon_::setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest ) noexcept
{
    // Force a 'valid Invalid State value
    if ( newInvalidState <= 0 )
    {
        newInvalidState = OPTION_CPL_DM_MODEL_POINT_STATE_INVALID;
    }

    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        if ( IS_VALID( m_validState ) )
        {
            m_validState = newInvalidState;
            processChangeNotifications();
        }

        // Note: Update my state even if there was NO valid-->invalid transition - since there are many 'invalid states'
        else
        {
            m_validState = newInvalidState;
        }
    }

    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();
    return result;
}

int8_t ModelPointCommon_::read( void* dstData, size_t dstSize, uint16_t* seqNumPtr ) const noexcept
{
    m_modelDatabase.lock_();
    int8_t validState = m_validState;
    if ( dstData && IS_VALID( validState ) )
    {
        copyDataTo_( dstData, dstSize );
    }
    if ( seqNumPtr )
    {
        *seqNumPtr = m_seqNum;
    }
    m_modelDatabase.unlock_();

    return validState;
}

uint16_t ModelPointCommon_::write( const void* srcData, size_t srcSize, LockRequest_T lockRequest ) noexcept
{
    m_modelDatabase.lock_();
    if ( srcData && testAndUpdateLock( lockRequest ) )
    {
        if ( !IS_VALID( m_validState ) || isDataEqual_( srcData ) == false )
        {
            copyDataFrom_( srcData, srcSize );
            processDataUpdated();
        }
    }
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

uint16_t ModelPointCommon_::readModifyWrite( GenericRmwCallback& callbackClient, LockRequest_T lockRequest )
{
    m_modelDatabase.lock_();
    if ( testAndUpdateLock( lockRequest ) )
    {
        // Invoke the client's callback function
        processRmwCallbackResult( callbackClient.genericCallback( m_dataPtr, m_validState ) );
    }

    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();

    return result;
}

void ModelPointCommon_::processRmwCallbackResult( RmwCallbackResult_T result ) noexcept
{
    // Do nothing if the callback did not change anything
    if ( result != RmwCallbackResult_T::eNO_CHANGE )
    {
        // Handle request to invalidate the MP data
        if ( result == RmwCallbackResult_T::eINVALIDATE )
        {
            if ( IS_VALID( m_validState ) )
            {
                m_validState = OPTION_CPL_DM_MODEL_POINT_STATE_INVALID;
                processChangeNotifications();
            }
        }

        // Handle the CHANGED use case
        else
        {
            processDataUpdated();
        }
    }
}

uint16_t ModelPointCommon_::touch() noexcept
{
    m_modelDatabase.lock_();
    processChangeNotifications();
    uint16_t result = m_seqNum;
    m_modelDatabase.unlock_();
    return result;
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
                memcpy( dstPtr + dataSize, &m_validState, sizeof( m_validState ) );

                // Export Locked state
                if ( includeLockedState )
                {
                    memcpy( dstPtr + dataSize + sizeof( m_validState ), &m_locked, sizeof( m_locked ) );
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
                memcpy( &m_validState, srcPtr + dataSize, sizeof( m_validState ) );

                // Import Locked state
                if ( includeLockedState )
                {
                    memcpy( &m_locked, srcPtr + dataSize + sizeof( m_validState ), sizeof( m_locked ) );
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
    size_t baseSize =  getInternalDataSize_() + sizeof( m_validState );
    return includeLockedState ? baseSize + sizeof( m_locked ) : baseSize;
}


/////////////////
const Cpl::Container::Key& ModelPointCommon_::getKey() const noexcept
{
    return m_staticInfo;
}



/////////////////
void ModelPointCommon_::processDataUpdated() noexcept
{
    // By definition - Point now has valid date
    m_validState = MODEL_POINT_STATE_VALID;
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
void ModelPointCommon_::attach( SubscriberApi& observer, uint16_t initialSeqNumber ) noexcept
{
    m_modelDatabase.lock_();
    observer.setSequenceNumber_( initialSeqNumber );
    observer.setModelPoint_( this );
    processSubscriptionEvent_( observer, eATTACH );
    m_modelDatabase.unlock_();
}

void ModelPointCommon_::detach( SubscriberApi& observer ) noexcept
{
    m_modelDatabase.lock_();
    processSubscriptionEvent_( observer, eDETACH );
    observer.setModelPoint_( 0 );
    m_modelDatabase.unlock_();
}

void ModelPointCommon_::genericAttach( SubscriberApi& observer, uint16_t initialSeqNumber) noexcept
{
    attach( observer, initialSeqNumber );
}

void ModelPointCommon_::genericDetach( SubscriberApi& observer ) noexcept
{
    detach( observer );
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
JsonDocument& ModelPointCommon_::beginJSON( int8_t validState, bool locked, uint16_t seqnum, bool verbose ) noexcept
{
    // Get access to the Global JSON document
    ModelDatabase::globalLock_();
    ModelDatabase::g_doc_.clear();  // Make sure the JSON document is starting "empty"

    // Construct the JSON
    ModelDatabase::g_doc_["name"]      = getName();
    ModelDatabase::g_doc_["invalid"]   = validState;
    if ( verbose )
    {
        ModelDatabase::g_doc_["type"]      = getTypeAsText();
        ModelDatabase::g_doc_["seqnum"]    = seqnum;
        ModelDatabase::g_doc_["locked"]    = locked;

        if (m_staticInfo.hasJSONFormatter())
        {
            JsonObject obj = ModelDatabase::g_doc_.createNestedObject("info");
            m_staticInfo.toJSON( obj );
        }
    }
    return ModelDatabase::g_doc_;
}

void ModelPointCommon_::endJSON( char* dst, size_t dstSize, bool& truncated, bool verbose ) noexcept
{
    // Generate the actual output string 
    size_t num = serializeJson( ModelDatabase::g_doc_, dst, dstSize );
    truncated = num == dstSize ? true : false;

    // Release the Global JSON document
    ModelDatabase::globalUnlock_();
}

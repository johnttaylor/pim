#ifndef Cpl_Dm_ModelPointBase_h_
#define Cpl_Dm_ModelPointBase_h_
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


#include "Cpl/Dm/ModelPoint.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/Subscriber.h"
#include "Cpl/Container/DList.h"
#include <stdint.h>


///
namespace Cpl {
///
namespace Dm {


/** This concrete class provide common infrastructure for a Model Point.
 */
class ModelPointCommon_ : public Cpl::Dm::ModelPoint
{
protected:
    /// List of Active Subscribers
    Cpl::Container::DList<SubscriberApi>    m_subscribers;

    /// Pointer to the Model Point's static information
    const StaticInfo&                       m_staticInfo;

    /// Reference to the containing Model Base
    ModelDatabase&                          m_modelDatabase;

    /// Reference to my Data
    void*                                   m_dataPtr;

    /// Sequence number used for tracking changes in the Point data
    uint16_t                                m_seqNum;

    /// Locked state
    bool                                    m_locked;

    /// Internal valid/invalid state
    int8_t                                  m_validState;


protected:
    /// Constructor
    ModelPointCommon_( ModelDatabase& myModelBase, void* myDataPtr, StaticInfo& staticInfo, int8_t validState = OPTION_CPL_DM_MODEL_POINT_STATE_INVALID );

public:
    /// See Cpl::Dm::ModelPoint
    const char* getName() const noexcept;

    /// See Cpl::Dm::ModelPoint
    uint16_t getSequenceNumber() const noexcept;

    /// See Cpl::Dm::ModelPoint
    uint16_t touch() noexcept;

    /// See Cpl::Dm::ModelPoint
    uint16_t setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// See Cpl::Dm::ModelPoint
    int8_t getValidState( void ) const noexcept;

    /// See Cpl::Dm::ModelPoint
    bool isLocked() const noexcept;

    /// See Cpl::Dm::ModelPoint
    uint16_t setLockState( LockRequest_T lockRequest ) noexcept;

    /// See Cpl::Dm::ModelPoint 
    size_t getExternalSize( bool includeLockedState = false ) const noexcept;

    /// See Cpl::Dm::ModelPoint.  Note: The implementation does NOT account for Endianess, i.e. assumes the 'platform' is the same for export/import
    size_t exportData( void* dstDataStream, size_t maxDstLength, uint16_t* retSequenceNumber = 0, bool includeLockedState = false ) const noexcept;

    /// See Cpl::Dm::ModelPoint.  Note: The implementation does NOT account for Endianess, i.e. assumes the 'platform' is the same for export/import
    size_t importData( const void* srcDataStream, size_t srcLength, uint16_t* retSequenceNumber = 0, bool includeLockedState = false ) noexcept;

    /// See Cpl::Dm::ModelPoint
    void genericAttach( SubscriberApi& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// See Cpl::Dm::ModelPoint
    void genericDetach( SubscriberApi& observer ) noexcept;

protected:
    /// See Cpl::Dm::ModelPoint
    int8_t read( void* dstData, size_t dstSize, uint16_t* seqNumPtr=0 ) const noexcept;

    /// See Cpl::Dm::ModelPoint
    uint16_t write( const void* srcData, size_t srcSize, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// See Cpl::Dm::ModelPoint
    uint16_t readModifyWrite( GenericRmwCallback& callbackClient, LockRequest_T lockRequest = eNO_REQUEST );

    /// See Cpl::Dm::ModelPoint
    void attach( SubscriberApi& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// See Cpl::Dm::ModelPoint 
    void detach( SubscriberApi& observer ) noexcept;


public:
    /// See Cpl::Container::DictItem
    const Cpl::Container::Key& getKey() const noexcept;

public:
    /// See Cpl::Dm::ModelPoint
    void processSubscriptionEvent_( SubscriberApi& subscriber, Event_T event ) noexcept;


protected:
    /** Internal helper method that completes the data update process as well
        as ensuring any change notifications get generated.  Note: This method
        ALWAYS sets the MP's state to 'valid'

        This method is NOT thread safe.
     */
    virtual void processDataUpdated() noexcept;

    /** Internal helper method that handles generating change notifications
        when the Model Point's data/state has changed.

        This method is NOT thread safe.
     */
    virtual void processChangeNotifications() noexcept;

    /** Internal helper method that advances/updates the Model Point's
        sequence number.

        This method is NOT thread safe.
     */
    virtual void advanceSequenceNumber() noexcept;

    /** Internal helper method that manages testing and updating the locked
        state.

        Rules:
        1) If 'lockRequest' is eNO_REQUEST, the method only returns true if
           the MP is in the unlocked state
        2) If 'lockRequest' is eLOCK, the method only returns if the MP is in
           the unlocked state.  In addition, when true is returned the MP is
           put into the locked state.
        3) If 'lockRequest' is eUNLOCK, the method always returns true and
           the MP is left in the unlocked state.

        This method is NOT thread safe.
     */
    virtual bool testAndUpdateLock( LockRequest_T lockRequest ) noexcept;


    /** Internal helper method that completes the data update process as well
        as ensuring any change notifications get generated AFTER a read-modify-
        write callback.

        This method is NOT thread safe.
     */
    virtual void processRmwCallbackResult( RmwCallbackResult_T result ) noexcept;


protected:
    /// Helper FSM method
    virtual void transitionToNotifyPending( SubscriberApi& subscriber ) noexcept;

    /// Helper FSM method
    virtual void transitionToSubscribed( SubscriberApi& subscriber ) noexcept;

    /// Helper method when converting MP to a JSON string
    virtual JsonDocument& beginJSON( int8_t validState, bool locked, uint16_t seqnum, bool verbose=true ) noexcept;

    /// Helper method when converting MP to a JSON string
    virtual void endJSON( char* dst, size_t dstSize, bool& truncated, bool verbose=true ) noexcept;


};

};      // end namespaces
};
#endif  // end header latch

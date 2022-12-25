#ifndef MpMetrics_h_
#define MpMetrics_h_
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

#include "Cpl/Dm/ModelPointCommon_.h"
#include "Metrics.h"



/** This class provides a concrete implementation for Model Point with a type
    of Metrics_T
    
    The toJSON() method is a read/modify operation, i.e. omitted key/value 
    fields in the 'val' object are NOT updated.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:{"numSamples":nnn, "sumTotal":nnn, "min":nnn, "max":nnn} }

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpMetrics : public Cpl::Dm::ModelPointCommon_
{
protected:
    /// Storage for the MP's data
    Metrics_T   m_data;

public:
    /// Constructor.  MP is created in the invalid state
    MpMetrics( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName );


public:
    /// Type safe read of the entire data struct. See Cpl::Dm::ModelPoint
    inline bool read( Metrics_T& dstData, uint16_t* seqNumPtr=0 ) const noexcept
    {
        return readData( &dstData, sizeof( Metrics_T ), seqNumPtr );
    }

    /// Type safe write of the entire data struct. See Cpl::Dm::ModelPoint
    inline uint16_t write( const Metrics_T& srcData, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return writeData( &srcData, sizeof( Metrics_T ), lockRequest );
    }

    /// Atomic operation to update the sample and sum total fields/values.
    uint16_t newSample( uint32_t sampleValue, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Atomic operation to clear all of the Metrics
    uint16_t clearAll( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

public:
    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MpMetrics& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return copyDataAndStateFrom( src, lockRequest );
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpMetrics> Observer;

    /** Type safe method to register an observer (i.e. register for change 
        notifications).  The client/observer only needs to register once. The 
        thread that this method executes in will be the thread that the change 
        notification callback function executes in.
     */
    inline void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept
    {
        attachSubscriber( observer, initialSeqNumber );
    }

    /// Type safe method to un-register an observer
    inline void detach( Observer& observer ) noexcept
    {
        detachSubscriber( observer );
    }

    /** This method is used to read the MP contents and synchronize
        the observer with the current MP contents.  This method should ONLY be
        used in the notification callback method and the 'observerToSync'
        argument MUST be the argument provided by the callback method
     */
    inline bool readAndSync( Metrics_T& dstData, Cpl::Dm::SubscriberApi& observerToSync )
    {
        return ModelPointCommon_::readAndSync( &dstData, sizeof( Metrics_T ), observerToSync );
    }

public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;

    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept;

protected:
    /// See Cpl::Dm::Point.  
    void setJSONVal( JsonDocument& doc ) noexcept;
};


#endif  // end header latch

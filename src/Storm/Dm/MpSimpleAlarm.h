#ifndef Storm_Dm_MpSimpleAlarm_h_
#define Storm_Dm_MpSimpleAlarm_h_
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
#include "Cpl/Text/FString.h"



///
namespace Storm {
///
namespace Dm {


/** This class provides a concrete implementation for a simple Alarm that has
    following attributes:
        - Requires some action (other than acknowledgment) to clear the Alarm's root cause
        - Tracks the 'user acknowledgeable' state
        - Provides an indication if the Alarm is critical, i.e. whether or not the Alarm cause the system be forced off
    
    The toJSON() method is a read/modify operation, i.e. omitted key/value 
    fields in the 'val' object are NOT updated.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:{"active":"true|false, "ack":true|false, "critical":true|false} }

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpSimpleAlarm : public Cpl::Dm::ModelPointCommon_
{
public:
    /** The MP's Data container.
     */
    struct Data
    {
        bool active;        //!< When set to true, the alarm is 'active'
        bool acked;         //!< When set to true, the alarm has been acknowledged.  When active is false, this field has no meaning
        bool critical;      //!< When set to true, the system is/was forced to the its 'off state'
    };

protected:
    /// Storage for the MP's data
    Data                m_data;

public:
    /// Constructor.  Valid MP - sets all fields to false (i.e. no-alarms/no-acks state)
    MpSimpleAlarm( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName );

public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    inline bool read( Data& dstData, uint16_t* seqNumPtr=0 ) const noexcept
    {
        return  ModelPointCommon_::read( &dstData, sizeof( Data ), seqNumPtr );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    inline uint16_t write( const Data& srcData, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::write( &srcData, sizeof( Data ), lockRequest );
    }

    /// Sets the Alarm state
    uint16_t setAlarm( bool active, bool isCritical=false, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Acknowledges the alarm
    uint16_t acknowledgeAlarm( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;


public:
    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MpSimpleAlarm& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::copyFrom( src, lockRequest );
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpSimpleAlarm> Observer;

    /// Type safe register observer
    void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// Type safe un-register observer
    void detach( Observer& observer ) noexcept;


public:
    /// See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;

    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept;

protected:
    /// See Cpl::Dm::Point.  
    void setJSONVal( JsonDocument& doc ) noexcept;

    /// Set valid default values when the MP is invalidated
    void hookSetInvalid() noexcept;
};



};      // end namespaces
};
#endif  // end header latch

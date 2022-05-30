#ifndef Storm_Dm_MpIdtAlarm_h_
#define Storm_Dm_MpIdtAlarm_h_
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



 ///
namespace Storm {
///
namespace Dm {


/** This class provides a concrete implementation for a Point who's data is the
    Indoor Temperature Alarm structure. 
    
    The toJSON() method is a read/modify operation, i.e. omitted key/value 
    fields in the 'val' object are NOT updated.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:{"priAlarm":true|false, "priAck":true|false, "secAlarm":true|false, "secAck":true|false, "critical":true|false} }

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpIdtAlarm : public Cpl::Dm::ModelPointCommon_
{
public:
    /** The MP's Data container.
     */
    struct Data
    {
        bool primaryAlarm;      //!< When set to true, the primary IDT source (aka onboard sensor) is "bad"
        bool secondaryAlarm;    //!< When set to true, the secondary IDT source (aka remote sensor) is "bad"
        bool primaryAck;        //!< When set to true, the primary IDT alarm has been acknowledged.  When primaryAlarm is false, this field has no meaning
        bool secondaryAck;      //!< When set to true, the secondary IDT alarm has been acknowledged.  When secondaryAlarm is false, this field has no meaning
        bool critical;          //!< When set to true, there is no valid IDT source and the system is/was forced to the its 'off state'
    };

protected:
    /// Storage for the MP's data
    Data                m_data;

public:
    /// Constructor.  Valid MP - sets all fields to false (i.e. no-alarms/no-acks state)
    MpIdtAlarm( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName );


public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    inline bool read( Data& dstData, uint16_t* seqNumPtr=0 ) const noexcept
    {
        return ModelPointCommon_::read( &dstData, sizeof( Data ), seqNumPtr );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    inline uint16_t write( const Data& srcData, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::write( &srcData, sizeof( Data ), lockRequest );
    }


    /// Sets the Alarm state
    uint16_t setAlarm( bool primaryAlarmState, bool secondaryAlarmState, bool isCritical, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Acknowledges the primary alarm
    uint16_t acknowledgePrimaryAlarm( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Acknowledges the primary alarm
    uint16_t acknowledgeSecondaryAlarm( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

public:
    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MpIdtAlarm& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::copyFrom( src, lockRequest );
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpIdtAlarm> Observer;

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
};



};      // end namespaces
};
#endif  // end header latch

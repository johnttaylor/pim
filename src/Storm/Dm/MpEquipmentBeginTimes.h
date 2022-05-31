#ifndef Storm_Dm_MpEquipmentBeginTimes_h_
#define Storm_Dm_MpEquipmentBeginTimes_h_
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
#include "Storm/Type/EquipmentTimes.h"


///
namespace Storm {
///
namespace Dm {


/** This class provides a concrete implementation for Model Point with a type
    of Storm::Type::EquipmentTimes_T

    The toJSON() method is a read/modify operation, i.e. omitted key/value
    fields in the 'val' object are NOT updated.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:{"beginIndoorOnTimeSec":mm.nn, "beginIndoorOffTimeSec":mm.nn, "beginOutdoorOnTimeSec":mm.nn, "beginOutdoorOffTimeSec":mm.nn} }

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpEquipmentBeginTimes : public Cpl::Dm::ModelPointCommon_
{
protected:
    /// Storage for the MP's data
    Storm::Type::EquipmentTimes_T    m_data;

public:
    /// Constructor.  Valid MP - sets all fields to zero
    MpEquipmentBeginTimes( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName );

public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    inline bool read( Storm::Type::EquipmentTimes_T& dstData, uint16_t* seqNumPtr=0 ) const noexcept
    {
        return ModelPointCommon_::read( &dstData, sizeof( Storm::Type::EquipmentTimes_T ), seqNumPtr );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    inline uint16_t write( const Storm::Type::EquipmentTimes_T& srcData, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::write( &srcData, sizeof( Storm::Type::EquipmentTimes_T ), lockRequest );
    }

    /// Sets the Point's Indoor Unit Begin on time.  Note: This is read-modify-write operation WRT to the Point's data
    uint16_t setIndoorUnitBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Point's Indoor Unit Begin off time.  Note: This is read-modify-write operation WRT to the Point's data
    uint16_t setIndoorUnitBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Point's Outdoor Unit Begin on time.  Note: This is read-modify-write operation WRT to the Point's data
    uint16_t setOutdoorUnitBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Point's Outdoor Unit Begin off time.  Note: This is read-modify-write operation WRT to the Point's data
    uint16_t setOutdoorUnitBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Point's System Cycle Begin on time.  Note: This is read-modify-write operation WRT to the Point's data
    uint16_t setSystemBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Point's System Cycle Begin off time.  Note: This is read-modify-write operation WRT to the Point's data
    uint16_t setSystemBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;


public:
    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MpEquipmentBeginTimes& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::copyFrom( src, lockRequest );
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpEquipmentBeginTimes> Observer;

    /// Type safe register observer
    void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// Type safe un-register observer
    void detach( Observer& observer ) noexcept;


public:
    ///  See Cpl::Dm::ModelPoint.
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

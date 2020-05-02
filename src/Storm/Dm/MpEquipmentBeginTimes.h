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

    { name:"<mpname>", type:"<mptypestring>", invalid:nn, seqnum:nnnn, locked:true|false, val:{"beginIndoorOnTimeSec":mm.nn, "beginIndoorOffTimeSec":mm.nn, "beginOutdoorOnTimeSec":mm.nn, "beginOutdoorOffTimeSec":mm.nn} }

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
    MpEquipmentBeginTimes( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo );

public:
    /// See Cpl::Dm::ModelPoint
    uint16_t setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    virtual int8_t read( Storm::Type::EquipmentTimes_T& dstData, uint16_t* seqNumPtr=0 ) const noexcept;

    /// Type safe write. See Cpl::Dm::ModelPoint
    virtual uint16_t write( const Storm::Type::EquipmentTimes_T& srcData, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Point's Indoor Unit Begin on time.  Note: This is read-modify-write operation WRT to the Point's data
    virtual uint16_t setIndoorUnitBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Point's Indoor Unit Begin off time.  Note: This is read-modify-write operation WRT to the Point's data
    virtual uint16_t setIndoorUnitBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Point's Outdoor Unit Begin on time.  Note: This is read-modify-write operation WRT to the Point's data
    virtual uint16_t setOutdoorUnitBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Point's Outdoor Unit Begin off time.  Note: This is read-modify-write operation WRT to the Point's data
    virtual uint16_t setOutdoorUnitBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Point's System Cycle Begin on time.  Note: This is read-modify-write operation WRT to the Point's data
    virtual uint16_t setSystemBeginOnTime( Cpl::System::ElapsedTime::Precision_T newBeginOnCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Point's System Cycle Begin off time.  Note: This is read-modify-write operation WRT to the Point's data
    virtual uint16_t setSystemBeginOffTime( Cpl::System::ElapsedTime::Precision_T newBeginOffCycleTime, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Type safe read-modify-write client callback interface
    typedef Cpl::Dm::ModelPointRmwCallback<Storm::Type::EquipmentTimes_T> Client;

    /** Type safe read-modify-write. See Cpl::Dm::ModelPoint

       NOTE: THE USE OF THIS METHOD IS STRONGLY DISCOURAGED because it has
             potential to lockout access to the ENTIRE Model Base for an
             indeterminate amount of time.  And alternative is to have the
             concrete Model Point leaf classes provide the application
             specific read, write, read-modify-write methods in addition or in
             lieu of the read/write methods in this interface.
     */
    virtual uint16_t readModifyWrite( Client& callbackClient, LockRequest_T lockRequest = eNO_REQUEST );

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpEquipmentBeginTimes> Observer;

    /// Type safe register observer
    virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// Type safe un-register observer
    virtual void detach( Observer& observer ) noexcept;


public:
    /// See Cpl::Dm::Point.  
    bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose=true ) noexcept;

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;

    /// See Cpl::Dm::ModelPoint.  Note: the returned sized does DOES NOT the null terminator
    size_t getSize() const noexcept;


public:
    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept;

    /// See Cpl::Dm::ModelPoint. 
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept;

    /// See Cpl::Dm::ModelPoint.  
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept;

    /// See Cpl::Dm::ModelPoint.  
    bool isDataEqual_( const void* otherData ) const noexcept;

    /// See Cpl::Dm::ModelPoint.  
    const void* getImportExportDataPointer_() const noexcept;

    /// See Cpl::Dm::ModelPoint.  
    size_t getInternalDataSize_() const noexcept;
};



};      // end namespaces
};
#endif  // end header latch

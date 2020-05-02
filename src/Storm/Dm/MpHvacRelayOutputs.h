#ifndef Storm_Dm_MpHvacRelayOutputs_h_
#define Storm_Dm_MpHvacRelayOutputs_h_
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
#include "Storm/Type/HvacRelayOutputs.h"



///
namespace Storm {
///
namespace Dm {


/** This class provides a concrete implementation for the HVAC Relay outputs for
    the System.  The following is the definition of the Relay outputs

    The toJSON() method is a read/modify operation, i.e. omitted key/value
    fields in the 'val' object are NOT updated.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", invalid:nn, seqnum:nnnn, locked:true|false, val:{ g:true|false, bk:nn, w1:true|false, w2:true|false, w3:true|flase, o:"COOL"|"HEAT", y1:true|flase, y2:true:false}}

    where 'bk' is value between 0 and 100 that describes PWM (in %) output signal.  0=off, 100 = 100% speed

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpHvacRelayOutputs : public Cpl::Dm::ModelPointCommon_
{
protected:
    /// Storage for the MP's data
    Storm::Type::HvacRelayOutputs_T  m_data;

public:
    /// Constructor.  Valid MP - sets all fields to zero (i.e. all outputs off and the SOV in cooling mode)
    MpHvacRelayOutputs( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo );

public:
    /// See Cpl::Dm::ModelPoint
    uint16_t setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    virtual int8_t read( Storm::Type::HvacRelayOutputs_T& dstData, uint16_t* seqNumPtr=0 ) const noexcept;

    /// Type safe write. See Cpl::Dm::ModelPoint
    virtual uint16_t write( const Storm::Type::HvacRelayOutputs_T& srcData, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets all outputs to its "safe/all off state".  Note: the SOV output is NOT changed by this operation
    virtual uint16_t setSafeAllOff( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Type safe read-modify-write client callback interface
    typedef Cpl::Dm::ModelPointRmwCallback<Storm::Type::HvacRelayOutputs_T> Client;

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
    /** Helper method that sets Output struct to its 'safe/all off state
     */
    static void setSafeAllOff( Storm::Type::HvacRelayOutputs_T& outputs );

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpHvacRelayOutputs> Observer;

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

protected:
    /// Validates the range of the output values
    void validate( Storm::Type::HvacRelayOutputs_T& newValues ) const noexcept;
};



};      // end namespaces
};
#endif  // end header latch

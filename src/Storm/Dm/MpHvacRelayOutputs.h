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

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:{ g:true|false, bk:nn, w1:true|false, w2:true|false, w3:true|flase, o:"COOL"|"HEAT", y1:true|false, y2:true:false}}

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
    MpHvacRelayOutputs( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName );


public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    inline bool read( Storm::Type::HvacRelayOutputs_T& dstData, uint16_t* seqNumPtr=0 ) const noexcept
    {
        return ModelPointCommon_::read( &dstData, sizeof( Storm::Type::HvacRelayOutputs_T ), seqNumPtr );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    inline uint16_t write( const Storm::Type::HvacRelayOutputs_T& srcData, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        Storm::Type::HvacRelayOutputs_T newData = srcData;
        validate( newData );
        return ModelPointCommon_::write( &newData, sizeof( Storm::Type::HvacRelayOutputs_T ), lockRequest );
    }

    /// Sets all outputs to its "safe/all off state".  Note: the SOV output is NOT changed by this operation
    inline uint16_t setSafeAllOff( LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        Storm::Type::HvacRelayOutputs_T newData;
        setSafeAllOff( newData );

        m_modelDatabase.lock_();
        newData.o = m_data.o;
        uint16_t result = write( newData, lockRequest );
        m_modelDatabase.unlock_();

        return result;
    }

public:
    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MpHvacRelayOutputs& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::copyFrom( src, lockRequest );
    }

public:
    /** Helper method that sets Output struct to its 'safe/all off state. Note: the SOV output is NOT changed by this operation
     */
    static void setSafeAllOff( Storm::Type::HvacRelayOutputs_T& outputs );

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpHvacRelayOutputs> Observer;

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

    /// Validates the range of the output values
    void validate( Storm::Type::HvacRelayOutputs_T& newValues ) const noexcept;
};



};      // end namespaces
};
#endif  // end header latch

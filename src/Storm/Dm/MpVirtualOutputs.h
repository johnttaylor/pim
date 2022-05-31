#ifndef Storm_Dm_MpVirtualOutputs_h_
#define Storm_Dm_MpVirtualOutputs_h_
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
#include "Storm/Type/VirtualOutputs.h"

/** This symbol is used to set the fan/stage to 'OFF' (e.g. relay off)
 */
#define STORM_DM_MP_VIRTUAL_OUTPUTS_OFF     0

 /** This symbol is used to set the fan/stage to 'FULL ON' (e.g. relay on)
  */
#define STORM_DM_MP_VIRTUAL_OUTPUTS_ON      1000


  ///
namespace Storm {
///
namespace Dm {


/** This class provides a concrete implementation for the virtual outputs for
    the system.  The virtual outputs are later mapped to relay/discrete
    outputs and/or digital communications to control the physical equipment.

    The toJSON() method is a read/modify operation, i.e. omitted key/value
    fields in the 'val' object are NOT updated.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:{ idFan:<out>, odFan:<out>, sovHeat:true:false, idStages:[ { stage:<num>, capacity:<out> }, ...], odStages:[ { stage:<num>, capacity:<out> }, ...]}}

    where 'out' is value between 0 and 1000 that describes speed/capacity of the output.  0=off, 500 = 50% speed/capacity, 1000=on/max-capacity

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpVirtualOutputs : public Cpl::Dm::ModelPointCommon_
{
protected:
    /// Storage for the MP's data
    Storm::Type::VirtualOutputs_T  m_data;

public:
    /// Constructor.  Valid MP - sets all fields to zero (i.e. all outputs off and the SOV in cooling mode)
    MpVirtualOutputs( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName );

public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    inline bool read( Storm::Type::VirtualOutputs_T& dstData, uint16_t* seqNumPtr=0 ) const noexcept
    {
        return ModelPointCommon_::read( &dstData, sizeof( Storm::Type::VirtualOutputs_T ), seqNumPtr );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    inline uint16_t write( const Storm::Type::VirtualOutputs_T& srcData, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        Storm::Type::VirtualOutputs_T newData = srcData;
        validate( newData );
        return ModelPointCommon_::write( &newData, sizeof( Storm::Type::VirtualOutputs_T ), lockRequest );
    }


    /// Sets the Indoor fan output.  Note: This is a read-modify-write operation WRT to the entire MP
    uint16_t setIndoorFanOutput( uint16_t fanSpeed, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Indoor fan CONTINOUS output.  Note: This is a read-modify-write operation WRT to the entire MP
    uint16_t setIndoorFanContinousOutput( uint16_t fanContSpeed, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets a single Indoor stage output. The 'stageIndex' is zero based! Note: This is a read-modify-write operation WRT to the entire MP
    uint16_t setIndoorStageOutput( uint8_t stageIndex, uint16_t stageOutput, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the Outdoor fan output.  Note: This is a read-modify-write operation WRT to the entire MP
    uint16_t setOutdoorFanOutput( uint16_t fanSpeed, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets a single Outdoor stage output. The 'stageIndex' is zero based! Note: This is a read-modify-write operation WRT to the entire MP
    uint16_t setOutdoorStageOutput( uint8_t stageIndex, uint16_t stageOutput, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the SOV state cooling operation This is a read-modify-write operation WRT to the entire MP
    uint16_t setSovToCooling( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the SOV state heating operation This is a read-modify-write operation WRT to the entire MP
    uint16_t setSovToHeating( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets the system unit outputs to its "safe/all off state".  Note: the SOV output is NOT changed by this operation
    uint16_t setSafeAllOff( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets all of the outdoor unit outputs to off (except it does not change the SOV output)
    uint16_t setOutdoorOff( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Sets all of the indoor unit outputs to off 
    uint16_t setIndoorOff( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

public:
    /** Helper method that returns true if at least stage of indoor OR outdoor 
        unit is on (i.e. is the 'system on')
     */
    static bool areStagesOn( const Storm::Type::VirtualOutputs_T& outputs );

    /** Helper method that sets Output struct to its 'safe/all off state
     */
    static void setSafeAllOff( Storm::Type::VirtualOutputs_T& outputs );

public:
    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MpVirtualOutputs& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::copyFrom( src, lockRequest );
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpVirtualOutputs> Observer;

    /// Type safe register observer
    virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// Type safe un-register observer
    virtual void detach( Observer& observer ) noexcept;


public:
    /// See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;

    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept;

protected:
    /// See Cpl::Dm::Point.  
    void setJSONVal( JsonDocument& doc ) noexcept;

    /// Validates the range of the output values
    void validate( Storm::Type::VirtualOutputs_T& newValues ) const noexcept;
};



};      // end namespaces
};
#endif  // end header latch

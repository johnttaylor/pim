#ifndef Storm_Dm_MpSystemConfig_h_
#define Storm_Dm_MpSystemConfig_h_
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
#include "Storm/Type/SystemConfig.h"
#include "Storm/Type/OperatingMode.h"
#include "Storm/Type/IduType.h"
#include "Storm/Type/OduType.h"


///
namespace Storm {
///
namespace Dm {


/** This class provides a concrete implementation for a Point who's data is the
    the System Configuration attributes.

    The toJSON() method is a read/modify operation, i.e. omitted key/value
    fields in the 'val' object are NOT updated.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:{ opMode:"<enum>", "oduType":"<enum>", "iduType":"<enum>", "numCompStages":n, "numIdStages":m, "totalStages":k, "fanCont":n, "gain":m.n, "reset":m.n, "maxPv":m.n, stages":[ {"stage":<num>, "lower":<val>, "upper":<val>, "minBlower":n, "maxBlower":m, "cph":"<enum>", "minOn":m, "minOff":n }...] }}

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpSystemConfig : public Cpl::Dm::ModelPointCommon_
{

protected:
    /// Storage for the MP's data
    Storm::Type::SystemConfig_T    m_data;

public:
    /** Constructor.  Valid MP.  Defaults all of the configuration to 'off configuration'
     */
    MpSystemConfig( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName );

public:
    /** Type safe read of the System Configuration
     */
    inline bool read( Storm::Type::SystemConfig_T& configuration, uint16_t* seqNumPtr=0 ) const noexcept
    {
        return ModelPointCommon_::read( &configuration, sizeof( Storm::Type::SystemConfig_T ), seqNumPtr );
    }

    /** Updates the entire System Configuration
     */
    inline uint16_t write( Storm::Type::SystemConfig_T& newConfiguration, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::write( &newConfiguration, sizeof( Storm::Type::SystemConfig_T ), lockRequest );
    }

    /** Sets the model to its "Off" and/or default configuration settings
     */
    inline uint16_t setToOff( LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        Storm::Type::SystemConfig_T newVal;
        setToOff( newVal );
        return ModelPointCommon_::write( &newVal, sizeof( Storm::Type::SystemConfig_T ), lockRequest );
    }

    /** Public helper method to set a instance of the SystemConfig structure
        to its "Off" state
     */
    static void setToOff( Storm::Type::SystemConfig_T& cfgToReset ) noexcept;



public:
    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MpSystemConfig& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::copyFrom( src, lockRequest );
    }



public:
    /// See Cpl::Dm::ModelPoint. Note: My data contains floating point values
    bool isDataEqual_( const void* otherData ) const noexcept;

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpSystemConfig> Observer;

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

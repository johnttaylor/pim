#ifndef Storm_Dm_MpComfortConfig_h_
#define Storm_Dm_MpComfortConfig_h_
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
#include "Storm/Type/ComfortConfig.h"


///
namespace Storm {
///
namespace Dm {


/** This class provides a concrete implementation for a Point who's data is the
    the Comfort configuration attributes.

    The toJSON() method is a read/modify operation, i.e. omitted key/value
    fields in the 'val' object are NOT updated.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:{ "cmpCooling":{"stage":<num>, "cph":<enum>, "minOn":<secs> "minOff":<secs>},
                                                                                                      "cmpHeating":{"stage":<num>, "cph":<enum>, "minOn":<secs> "minOff":<secs>},
                                                                                                      "indoorHeating":{"stage":<num>, "cph":<enum>, "minOn":<secs> "minOff":<secs>}}}

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpComfortConfig : public Cpl::Dm::ModelPointCommon_
{

protected:
    /// Storage for the MP's data
    Storm::Type::ComfortConfig_T    m_data;

public:
    /** Constructor.  Valid MP.  Defaults all stages (for both modes) to 3CPH, 5min minimum on time, 5min minimum off time
     */
    MpComfortConfig( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName );


public:
    /** Type safe read of the Comfort Configuration
     */
    inline bool read( Storm::Type::ComfortConfig_T& configuration, uint16_t* seqNumPtr=0 ) const noexcept
    {
        return ModelPointCommon_::read( &configuration, sizeof( Storm::Type::ComfortConfig_T ), seqNumPtr );
    }

    /** Updates the entire Comfort Configuration
     */
    inline uint16_t write( const Storm::Type::ComfortConfig_T& newConfiguration, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        Storm::Type::ComfortConfig_T newVale = newConfiguration;
        Storm::Type::ComfortConfig_T::validate( newVale );
        return ModelPointCommon_::write( &newConfiguration, sizeof( Storm::Type::ComfortConfig_T ), lockRequest );
    }

    /** Updates only the cooling operation (i.e. this is a read-modify-write operation)
     */
    uint16_t writeCompressorCooling( const Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /** Updates only the compressor heating operation (i.e. this is a read-modify-write operation)
     */
    uint16_t writeCompressorHeating( const Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /** Updates only the indoor heating operation (i.e. this is a read-modify-write operation)
     */
    uint16_t writeIndoorHeating( const Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;


public:
    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MpComfortConfig& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::copyFrom( src, lockRequest );
    }


public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpComfortConfig> Observer;

    /// Type safe register observer
    virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// Type safe un-register observer
    virtual void detach( Observer& observer ) noexcept;


public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;

    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept;

protected:
    /// See Cpl::Dm::Point.  
    void setJSONVal( JsonDocument& doc ) noexcept;

    /// See Cpl::Dm::ModelPoint
    void hookSetInvalid() noexcept;
};



};      // end namespaces
};
#endif  // end header latch

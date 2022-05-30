#ifndef Storm_Dm_MpWhiteBox_h_
#define Storm_Dm_MpWhiteBox_h_
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

#include "colony_config.h"
#include "Storm/Type/WhiteBox.h"
#include "Cpl/Dm/ModelPointCommon_.h"




///
namespace Storm {
///
namespace Dm {


/** This class provides a concrete implementation for a Point who's data is a
    collection of variables used to facilitate white box testing.

    The toJSON() method is a read/modify operation, i.e. omitted key/value 
    fields in the 'val' object are NOT updated.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:{"defeatEquipMinOffTime":true|false, "abortOnOffCycle":true|false} }

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpWhiteBox : public Cpl::Dm::ModelPointCommon_
{
public:

protected:
    /// Storage for the MP's data
    Storm::Type::WhiteBox_T       m_data;

public:
    /// Constructor.  Valid MP - all settings to their 'off/benign' values
    MpWhiteBox( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName );


public:
    /** Type safe read of all of the settings
     */
    inline bool read( Storm::Type::WhiteBox_T& dstSettings, uint16_t* seqNumPtr=0 ) const noexcept
    {
        return ModelPointCommon_::read( &dstSettings, sizeof( Storm::Type::WhiteBox_T ), seqNumPtr );
    }

    /** Type safe write of all of the settings
     */
    inline uint16_t write( const Storm::Type::WhiteBox_T& srcSettings, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::write( &srcSettings, sizeof( Storm::Type::WhiteBox_T ), lockRequest );
    }

    /** Type safe method to reset all 'Pulse' flags/settings
     */
    uint16_t resetPulseSettings( LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

public:
    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MpWhiteBox& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::copyFrom( src, lockRequest );
    }


public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpWhiteBox> Observer;

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

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

    { name:"<mpname>", type:"<mptypestring>", invalid:nn, seqnum:nnnn, locked:true|false, val:{ "cmpCooling":{"stage":<num>, "cph":<enum>, "minOn":<secs> "minOff":<secs>}, 
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
    MpComfortConfig( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo);

public:
    /** Method that limit/range checks the specified configuration. Invalid and/or
        out-of-range value are corrected, Returns true if one or more fields
        were corrected.
     */
    virtual bool validate( Storm::Type::ComfortConfig_T& src ) const noexcept;

    /** Method that limit/range checks the specified STAGE configuration. Invalid 
        and/or out-of-range value are corrected, Returns true if one or more 
        fields were corrected.
     */
    virtual bool validate( Storm::Type::ComfortStageParameters_T& src ) const noexcept;

public:
    /** Type safe read of the Comfort Configuration
     */
    virtual int8_t read( Storm::Type::ComfortConfig_T& configuration, uint16_t* seqNumPtr=0 ) const noexcept;

    /** Updates the entire Comfort Configuration
     */
    virtual uint16_t write( Storm::Type::ComfortConfig_T& newConfiguration, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /** Updates only the cooling operation (i.e. this is a read-modify-write operation)
     */
    virtual uint16_t writeCompressorCooling( Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /** Updates only the compressor heating operation (i.e. this is a read-modify-write operation)
     */
    virtual uint16_t writeCompressorHeating( Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;
 
    /** Updates only the indoor heating operation (i.e. this is a read-modify-write operation)
     */
    virtual uint16_t writeIndoorHeating( Storm::Type::ComfortStageParameters_T newParameters, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;


    /// Type safe read-modify-write client callback interface
    typedef Cpl::Dm::ModelPointRmwCallback<Storm::Type::ComfortConfig_T> Client;

    /** Type safe read-modify-write. See Cpl::Dm::ModelPoint.

        NOTE: The client is responsible for enforcing the min/max set-point
              ranges and minimum delta requirements for the set-point values
              (see the validateSetpoints() method).

        NOTE: THE USE OF THIS METHOD IS STRONGLY DISCOURAGED because it has
              potential to lockout access to the ENTIRE Model Base for an
              indeterminate amount of time.  And alternative is to have the
              concrete Model Point leaf classes provide the application
              specific read, write, read-modify-write methods in addition or in
              lieu of the read/write methods in this interface.
     */
    virtual uint16_t readModifyWrite( Client& callbackClient, LockRequest_T lockRequest = eNO_REQUEST );

    /// See Cpl::Dm::ModelPoint
    uint16_t setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;


public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpComfortConfig> Observer;

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

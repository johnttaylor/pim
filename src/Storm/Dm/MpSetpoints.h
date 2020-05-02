#ifndef Storm_Dm_MpSetpoints_h_
#define Storm_Dm_MpSetpoints_h_
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
#include "Cpl/Dm/ModelPointCommon_.h"


/** This symbol defines the minimum cooling set-point (in degrees 'F)
 */
#ifndef OPTION_STORM_DM_MP_SETPOINTS_MIN_COOLING
#define OPTION_STORM_DM_MP_SETPOINTS_MIN_COOLING        55.0F
#endif

/** This symbol defines the maximum cooling set-point (in degrees 'F)
*/
#ifndef OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING
#define OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING        95.0F
#endif

/** This symbol defines the minimum heating set-point (in degrees 'F)
 */
#ifndef OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING
#define OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING        50.0F
#endif

/** This symbol defines the maximum heating set-point (in degrees 'F)
 */
#ifndef OPTION_STORM_DM_MP_SETPOINTS_MAX_HEATING
#define OPTION_STORM_DM_MP_SETPOINTS_MAX_HEATING        90.0F
#endif

/// Default Cooling set-point
#ifndef OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_COOLING
#define OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_COOLING    78.0F
#endif

/// Default Heating set-point
#ifndef OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_HEATING
#define OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_HEATING    68.0F
#endif


///
namespace Storm {
///
namespace Dm {


/** This class provides a concrete implementation for a Point who's data is the
    the house/room/zone cooling and heating set-points.  The class enforces
    the min/max ranges for the set-points.  Minimum delta between the two
    set-points is NOT maintained (i.e. that is is policy decision)
    
    The toJSON() method is a read/modify operation, i.e. omitted key/value 
    fields in the 'val' object are NOT updated.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", invalid:nn, seqnum:nnnn, locked:true|false, val:{"cool":mm.m, "heat":nn.n} }

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpSetpoints : public Cpl::Dm::ModelPointCommon_
{
public:
    /** The MP's Data container.
     */
    typedef struct
    {
        float coolSetpt;        //!< Cooling set-point in degrees Fahrenheit.  The Cooling set-point must be >= heatSetpt+OPTION_STORM_DM_MP_SETPOINTS_MIN_DELTA
        float heatSetpt;        //!< Heating set-point in degrees Fahrenheit
    } Data;

protected:
    /// Storage for the MP's data
    Data                m_data;

public:
    /// Constructor.  Valid MP
    MpSetpoints( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, float coolSetpt=OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_COOLING, float heatSetpt=OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_HEATING );

public:
    /// See Cpl::Dm::ModelPoint
    uint16_t setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;


public:
    /** Type safe read of the Cooling & Heating set-point
     */
    virtual int8_t read( float& currentCoolSetpoint, float& currentHeatSetpoint, uint16_t* seqNumPtr=0 ) const noexcept;

    /** Type safe read of the Cooling set-point
     */
    virtual int8_t readCool( float& currentCoolSetpoint, uint16_t* seqNumPtr=0 ) const noexcept;

    /** Type safe read of the Heating set-point
     */
    virtual int8_t readHeat( float& currentHeatSetpoint, uint16_t* seqNumPtr=0 ) const noexcept;

    /** Sets the both the cooling and heating set-point.  If the specified
        set-points violates the minimum delta requirement, then the heating
        set-point is adjusted
     */
    virtual uint16_t write( float newCoolingSetpoint, float newHeatingSetpoint, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /** Sets the cooling set-point.  Note: If the Model is invalid at the time
        of this call, the heating set-point will be set to its minimum allowed
        value.
     */
    virtual uint16_t writeCool( float newSetpoint, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /** Sets the heating set-point.  Note: If the Model is invalid at the time
        of this call, the cooling set-point will be set to its maximum allowed
        value.
     */
    virtual uint16_t writeHeat( float newSetpoint, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Type safe read-modify-write client callback interface
    typedef Cpl::Dm::ModelPointRmwCallback<Data> Client;

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

    /** This helper method enforces the set-point value rules.  
     */
    static void validateSetpoints( float newCooling, float newHeating, float& finalCooling, float& finalHeating );

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpSetpoints> Observer;

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

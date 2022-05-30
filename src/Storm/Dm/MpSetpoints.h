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

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:{"cool":mm.m, "heat":nn.n} }

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpSetpoints : public Cpl::Dm::ModelPointCommon_
{
public:
    /** The MP's Data container.
     */
    struct Data
    {
        float coolSetpt;        //!< Cooling set-point in degrees Fahrenheit.  The Cooling set-point must be >= heatSetpt+OPTION_STORM_DM_MP_SETPOINTS_MIN_DELTA
        float heatSetpt;        //!< Heating set-point in degrees Fahrenheit
    };

protected:
    /// Storage for the MP's data
    Data                m_data;

public:
    /// Constructor.  Valid MP
    MpSetpoints( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, float coolSetpt=OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_COOLING, float heatSetpt=OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_HEATING );


public:
    /** Type safe read of the Cooling & Heating set-point
     */
    inline bool read( float& currentCoolSetpoint, float& currentHeatSetpoint, uint16_t* seqNumPtr=0 ) const noexcept
    {
        Data dst;
        bool valid          = ModelPointCommon_::read( &dst, sizeof( Data ), seqNumPtr );
        currentCoolSetpoint = dst.coolSetpt;
        currentHeatSetpoint = dst.heatSetpt;
        return valid;
    }

    /** Type safe read of the Cooling set-point
     */
    inline bool readCool( float& currentCoolSetpoint, uint16_t* seqNumPtr=0 ) const noexcept
    {
        Data   dst;
        int8_t valid = ModelPointCommon_::read( &dst, sizeof( Data ), seqNumPtr );
        currentCoolSetpoint = dst.coolSetpt;
        return valid;
    }

    /** Type safe read of the Heating set-point
     */
    inline bool readHeat( float& currentHeatSetpoint, uint16_t* seqNumPtr=0 ) const noexcept
    {
        Data   dst;
        int8_t valid        = ModelPointCommon_::read( &dst, sizeof( Data ), seqNumPtr );
        currentHeatSetpoint = dst.heatSetpt;
        return valid;
    }

    /** Sets the both the cooling and heating set-point.  If the specified
        set-points violates the minimum delta requirement, then the heating
        set-point is adjusted
     */
    inline uint16_t write( float newCoolingSetpoint, float newHeatingSetpoint, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        validateSetpoints( newCoolingSetpoint, newHeatingSetpoint, newCoolingSetpoint, newHeatingSetpoint );
        Data src ={ newCoolingSetpoint, newHeatingSetpoint };
        return ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );
    }

    /** Sets the cooling set-point.  Note: If the Model is invalid at the time
        of this call, the heating set-point will be set to its minimum allowed
        value.
     */
    inline uint16_t writeCool( float newSetpoint, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        m_modelDatabase.lock_();

        float finalHeatSetpt;
        validateSetpoints( newSetpoint, m_data.heatSetpt, newSetpoint, finalHeatSetpt );
        Data src ={ newSetpoint, finalHeatSetpt };
        uint16_t result = ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );

        m_modelDatabase.unlock_();
        return result;
    }


    /** Sets the heating set-point.  Note: If the Model is invalid at the time
        of this call, the cooling set-point will be set to its maximum allowed
        value.
     */
    inline uint16_t writeHeat( float newSetpoint, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        m_modelDatabase.lock_();

        float finalCoolSetpt;
        validateSetpoints( m_data.coolSetpt, newSetpoint, finalCoolSetpt, newSetpoint );
        Data src ={ finalCoolSetpt, newSetpoint };
        uint16_t result = ModelPointCommon_::write( &src, sizeof( Data ), lockRequest );

        m_modelDatabase.unlock_();
        return result;
    }


    /** This helper method enforces the set-point value rules.
     */
    static void validateSetpoints( float newCooling, float newHeating, float& finalCooling, float& finalHeating );

public:
    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MpSetpoints& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::copyFrom( src, lockRequest );
    }


public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpSetpoints> Observer;

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

    /// Set valid default values when the MP is invalidated
    void hookSetInvalid() noexcept;
};



};      // end namespaces
};
#endif  // end header latch

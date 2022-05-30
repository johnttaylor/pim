#ifndef Storm_Dm_MpEquipmentConfig_h_
#define Storm_Dm_MpEquipmentConfig_h_
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
#include "Storm/Type/IduType.h"
#include "Storm/Type/OduType.h"


/// Default Indoor Unit type
#ifndef OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDUTYPE
#define OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDUTYPE          Storm::Type::IduType::eFURNACE
#endif

/// Default Indoor Fan motor (fixed-speed)
#ifndef OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_VSPBLOWER
#define OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_VSPBLOWER         false
#endif

/// Default number of indoor heating stages
#ifndef OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDU_NUM_STAGES
#define OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDU_NUM_STAGES     1
#endif

/// Default Outdoor Unit type
#ifndef OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_ODUTYPE
#define OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_ODUTYPE            Storm::Type::OduType::eAC
#endif

/// Default number of compressor stages
#ifndef OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_COMP_NUM_STAGES
#define OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_COMP_NUM_STAGES    1
#endif
///
namespace Storm {
///
namespace Dm {


/** This class provides a concrete implementation for a Point who's data is the
    the Equipment Configuration attributes.

    The toJSON() method is a read/modify operation, i.e. omitted key/value
    fields in the 'val' object are NOT updated.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valide:true|false, seqnum:nnnn, locked:true|false, val:{"type":"<enum>", "vspMotor":true|false, "numHeat":n} }

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpEquipmentConfig : public Cpl::Dm::ModelPointCommon_
{
public:
    /** The MP's Data container.
     */
    struct Data
    {
        int      iduType;               //!< Type of Indoor Unit.   Actual type is Storm::Type::IduType  (Note: BETTER_ENUMs and classes/structs don't mix well - so we have to use an int in the struct)
        int      oduType;               //!< Type of Outdoor Unit.  Actual type is Storm::Type::OduType  (Note: BETTER_ENUMs and classes/structs don't mix well - so we have to use an int in the struct)
        uint16_t numCompStages;         //!< Number of Compressor stages in the Outdoor Unit 
        uint16_t numIduHeatingStages;   //!< Number of Indoor heating stages in the Indoor Unit 
        bool     hasVspBlower;          //!< Set to true when the Indoor Unit has a variable speed motor
    };

protected:
    /// Storage for the MP's data
    Data                m_data;

public:
    /// Constructor.  Valid MP
    MpEquipmentConfig( Cpl::Dm::ModelDatabase&  myModelBase,
                       const char*              symbolicName,
                       Storm::Type::IduType     iduUnitType         = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDUTYPE,
                       bool                     hasVspBlower        = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_VSPBLOWER,
                       uint16_t                 numIduHeatingStages = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDU_NUM_STAGES,
                       Storm::Type::OduType     oduUnitType         = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_ODUTYPE, 
                       uint16_t                 numCompressorStages = OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_COMP_NUM_STAGES );


public:
    /** Method that limit/range checks the specified configuration. Invalid and/or
        out-of-range value are corrected, Returns true  if one or more fields
        were corrected.
     */
    virtual bool validate( Data& src ) const noexcept;

public:
    /** Type safe read of the Indoor Unit Configuration
     */
    inline bool read( Data& configuration, uint16_t* seqNumPtr=0 ) const noexcept
    {
        return ModelPointCommon_::read( &configuration, sizeof( Data ), seqNumPtr );
    }

    /** Updates the entire Indoor Unit Configuration
     */
    inline uint16_t write( Data& newConfiguration, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        // Enforce limits
        validate( newConfiguration );
        return ModelPointCommon_::write( &newConfiguration, sizeof( Data ), lockRequest );
    }

    /** Updates the Indoor unit type
     */
    uint16_t writeIndoorType( Storm::Type::IduType newUnitType, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /** Updates the fan motor type
     */
    uint16_t writeIndoorFanMotor( bool hasVspBlower, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /** Updates the number of indoor heating stages.  Note: if 'numHeatingStages'
        is greater than the supported maximum, it is clamp at the max supported
        value.
     */
    uint16_t writeIndoorHeatingStages( uint16_t numHeatingStages, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /** Updates the Outdoor unit type
     */ 
    uint16_t writeOutdoorType( Storm::Type::OduType newUnitType, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /** Updates the number of indoor heating stages. Note: if 'numStages'
        is greater than the supported maximum, it is clamp at the max supported
        value.
     */
    uint16_t writeCompressorStages( uint16_t numStages, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

public:
    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MpEquipmentConfig& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return ModelPointCommon_::copyFrom( src, lockRequest );
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpEquipmentConfig> Observer;

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

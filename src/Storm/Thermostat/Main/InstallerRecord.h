#ifndef Storm_Thermostat_Main_Installer_Record_h_
#define Storm_Thermostat_Main_Installer_Record_h_
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

#include "colony_config.h"
#include "Storm/Constants.h"
#include "Cpl/Dm/Persistent/Record.h"
#include "Storm/Thermostat/ModelPoints.h"


/// Major Schema index for my record
#ifndef OPTION_STORM_THERMOSTAT_MAIN_INSTALLER_RECORD_MAJOR
#define OPTION_STORM_THERMOSTAT_MAIN_INSTALLER_RECORD_MAJOR  0
#endif

/// Minor Schema index for my record
#ifndef OPTION_STORM_THERMOSTAT_MAIN_INSTALLER_RECORD_MINOR
#define OPTION_STORM_THERMOSTAT_MAIN_INSTALLER_RECORD_MINOR  0
#endif


///
namespace Storm {
///
namespace Thermostat {
///
namespace Main {


/** This concrete class implements the "Record" class for storing Installer settings.
 */
class InstallerRecord : public Cpl::Dm::Persistent::Record
{
public:
    /// Constructor
    InstallerRecord( Cpl::Persistent::Chunk& chunkHandler )
        : Cpl::Dm::Persistent::Record( m_modelPoints, chunkHandler, OPTION_STORM_THERMOSTAT_MAIN_INSTALLER_RECORD_MAJOR, OPTION_STORM_THERMOSTAT_MAIN_INSTALLER_RECORD_MINOR )
    {
        m_modelPoints[0] ={ &mp_enabledSecondaryIdt, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };
        m_modelPoints[1] ={ &mp_equipmentConfig, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };
        m_modelPoints[2] ={ &mp_comfortConfig, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };
        m_modelPoints[3] ={ 0,0 };
    }

public:
    /// See Cpl::Dm::Persistent::Record
    void resetData() noexcept
    {
        mp_enabledSecondaryIdt.write( OPTION_STORM_DEFAULT_REMOTE_SENSOR_ENABLED_STATE );
        Storm::Dm::MpEquipmentConfig::Data equipment =
        {
            OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDUTYPE,
            OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_ODUTYPE,
            OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_COMP_NUM_STAGES,
            OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_IDU_NUM_STAGES,
            OPTION_STORM_DM_EQUIPMENT_CONFIG_DEFAULT_VSPBLOWER
        };
        mp_equipmentConfig.write( equipment );
        Storm::Type::ComfortConfig_T comfort =
        {
            { OPTION_STORM_DEFAULT_CPH, OPTION_STORM_DEFAULT_MIN_ON_CYCLE_TIME, OPTION_STORM_DEFAULT_MIN_OFF_CYCLE_TIME },
            { OPTION_STORM_DEFAULT_CPH, OPTION_STORM_DEFAULT_MIN_ON_CYCLE_TIME, OPTION_STORM_DEFAULT_MIN_OFF_CYCLE_TIME },
            { OPTION_STORM_DEFAULT_CPH, OPTION_STORM_DEFAULT_MIN_ON_CYCLE_TIME, OPTION_STORM_DEFAULT_MIN_OFF_CYCLE_TIME }
        };
        mp_comfortConfig.write( comfort );
    }

protected:
    /// List of Model Points for the Record
    Cpl::Dm::Persistent::Record::Item_T m_modelPoints[3 + 1];
};

};      // end namespace(s)
};
};

#endif  // end header latch

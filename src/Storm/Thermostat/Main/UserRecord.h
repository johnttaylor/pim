#ifndef Storm_Thermostat_Main_UserRecord_h_
#define Storm_Thermostat_Main_UserRecord_h_
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
#include "Cpl/Dm/Persistent/Record.h"
#include "Storm/Thermostat/ModelPoints.h"


/// Major Schema index for my record
#ifndef OPTION_STORM_THERMOSTAT_MAIN_USER_RECORD_MAJOR
#define OPTION_STORM_THERMOSTAT_MAIN_USER_RECORD_MAJOR  0
#endif

/// Minor Schema index for my record
#ifndef OPTION_STORM_THERMOSTAT_MAIN_USER_RECORD_MINOR
#define OPTION_STORM_THERMOSTAT_MAIN_USER_RECORD_MINOR  0
#endif

/// Default value for the Indoor Air Filter timer
#ifndef OPTION_STORM_THERMOSTAT_DEFAULT_AIR_FILTER_HOURS
#define OPTION_STORM_THERMOSTAT_DEFAULT_AIR_FILTER_HOURS    360
#endif

/// Default thermostat/user mode
#ifndef OPTION_STORM_THERMOSTAT_DEFAULT_USER_MODE
#define OPTION_STORM_THERMOSTAT_DEFAULT_USER_MODE           Storm::Type::ThermostatMode::eOFF
#endif

/// Default fan mode
#ifndef OPTION_STORM_THERMOSTAT_DEFAULT_FAN_MODE
#define OPTION_STORM_THERMOSTAT_DEFAULT_FAN_MODE            Storm::Type::FanMode::eAUTO
#endif


///
namespace Storm {
///
namespace Thermostat {
///
namespace Main {


/** This concrete class implements the "Record" class for storing user settings.
 */
class UserRecord : public Cpl::Dm::Persistent::Record
{
public:
    /// Constructor
    UserRecord( Cpl::Persistent::Chunk& chunkHandler )
        : Cpl::Dm::Persistent::Record( m_modelPoints, chunkHandler, OPTION_STORM_THERMOSTAT_MAIN_USER_RECORD_MAJOR, OPTION_STORM_THERMOSTAT_MAIN_USER_RECORD_MINOR )
    {
        m_modelPoints[0] ={ &mp_setpoints, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };
        m_modelPoints[1] ={ &mp_userMode, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };
        m_modelPoints[2] ={ &mp_fanMode, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };
        m_modelPoints[3] ={ &mp_maxAirFilterHours, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };
        m_modelPoints[4] ={ 0,0 };
    }

public:
    /// See Cpl::Dm::Persistent::Record
    bool resetData() noexcept
    {
        mp_setpoints.write( OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_COOLING, OPTION_STORM_DM_MP_SETPOINTS_DEFAULT_HEATING );
        mp_userMode.write( OPTION_STORM_THERMOSTAT_DEFAULT_USER_MODE );
        mp_fanMode.write( OPTION_STORM_THERMOSTAT_DEFAULT_FAN_MODE );
        mp_maxAirFilterHours.write( OPTION_STORM_THERMOSTAT_DEFAULT_AIR_FILTER_HOURS );

        return true;
    }

protected:
    /// List of Model Points for the Record
    Cpl::Dm::Persistent::Record::Item_T m_modelPoints[4 + 1];
};

};      // end namespace(s)
};
};

#endif  // end header latch

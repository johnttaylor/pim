#ifndef Storm_Thermostat_Main_Run_Time_Record_h_
#define Storm_Thermostat_Main_Run_Time_Record_h_
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
#include "Cpl/System/Timer.h"
#include "Cpl/Memory/Aligned.h"

/// Major Schema index for my record
#ifndef OPTION_STORM_THERMOSTAT_MAIN_RUN_TIME_RECORD_MAJOR
#define OPTION_STORM_THERMOSTAT_MAIN_RUN_TIME_RECORD_MAJOR  0
#endif

/// Minor Schema index for my record
#ifndef OPTION_STORM_THERMOSTAT_MAIN_RUN_TIME_RECORD_MINOR
#define OPTION_STORM_THERMOSTAT_MAIN_RUN_TIME_RECORD_MINOR  0
#endif

/// The amount of time, in milliseconds, for updating the non-change driven run time data
#ifndef OPTION_STORM_THERMOSTAT_MAIN_RUN_TIME_RECORD_PERIODIC_UPDATE_MS
#define OPTION_STORM_THERMOSTAT_MAIN_RUN_TIME_RECORD_PERIODIC_UPDATE_MS     (1000*60*15)    // every 15 minutes
#endif

///
namespace Storm {
///
namespace Thermostat {
///
namespace Main {


/** This concrete class implements the "Record" class for storing persistent
    run time alerts, status, etc.
 */
class RunTimeRecord : public Cpl::Dm::Persistent::Record, public Cpl::System::Timer
{
public:
    /// Constructor
    RunTimeRecord( Cpl::Persistent::Chunk& chunkHandler )
        : Cpl::Dm::Persistent::Record( m_modelPoints, chunkHandler, OPTION_STORM_THERMOSTAT_MAIN_RUN_TIME_RECORD_MAJOR, OPTION_STORM_THERMOSTAT_MAIN_RUN_TIME_RECORD_MINOR )
    {
        m_modelPoints[0] ={ &mp_airFilterAlert, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };
        m_modelPoints[1] ={ &mp_airFilterOperationTime, CPL_DM_PERISTENCE_RECORD_NO_SUBSCRIBER };
        m_modelPoints[2] ={ 0,0 };
    }

public:
    /// See Cpl::Dm::Persistent::Record
    void resetData() noexcept
    {
        mp_airFilterAlert.write( { false, false, false } );
        mp_airFilterOperationTime.write( { 0,0 } );
    }

    /// See Cpl::Persistent::Record
    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept
    {
        Record::start( myMbox );
        setTimingSource( myMbox );
        Cpl::System::Timer::start( OPTION_STORM_THERMOSTAT_MAIN_RUN_TIME_RECORD_PERIODIC_UPDATE_MS );
    }

    /// See Cpl::Persistent::Record
    void stop() noexcept
    {
        Cpl::System::Timer::stop();
        Record::stop();
    }

    /// Timer callback: Write the Run time data to persistent storage
    void expired( void ) noexcept
    {
        m_chunkHandler.updateData( *this );
        Cpl::System::Timer::start( OPTION_STORM_THERMOSTAT_MAIN_RUN_TIME_RECORD_PERIODIC_UPDATE_MS );
    }

protected:
    /// List of Model Points for the Record
    Cpl::Dm::Persistent::Record::Item_T m_modelPoints[2 + 1];
};

};      // end namespace(s)
};
};

#endif  // end header latch

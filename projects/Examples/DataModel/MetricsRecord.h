#ifndef MetricsRecord_h_
#define MetricsRecord_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "colony_config.h"
#include "Cpl/Dm/Persistent/Record.h"
#include "ModelPoints.h"

/// Major Schema index for my record
#ifndef OPTION_MAIN_METRIC_RECORD_MAJOR
#define OPTION_MAIN_METRIC_RECORD_MAJOR    0
#endif

/// Minor Schema index for my record
#ifndef OPTION_MAIN_METRIC_RECORD_MINOR
#define OPTION_MAIN_METRIC_RECORD_MINOR    0
#endif


/** This concrete class implements the "Metrics" record for persistently
    storing the Algorithm's metrics and the Application's boot counter
 */
class MetricRecord : public Cpl::Dm::Persistent::Record
{
public:
    /// Constructor
    MetricRecord( Cpl::Persistent::Chunk& chunkHandler )
        : Cpl::Dm::Persistent::Record( m_modelPoints, chunkHandler, OPTION_MAIN_METRIC_RECORD_MAJOR, OPTION_MAIN_METRIC_RECORD_MINOR )
    {
        m_modelPoints[0] = { &mp::bootCounter, CPL_DM_PERISTENCE_RECORD_NO_SUBSCRIBER };    // Note: See notes in hookProcessPostRecordLoaded()
        m_modelPoints[1] = { &mp::metrics, CPL_DM_PERISTENCE_RECORD_NO_SUBSCRIBER};         // Note: The application is responsible for periodically update the persistent store with new metric data
        m_modelPoints[2] = { &mp::hiAlarmCounts, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };
        m_modelPoints[3] = { &mp::loAlarmCounts, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };

        m_modelPoints[4] = { 0,0 };
    }

protected:
    /// List of Model Points contained in the Record
    Cpl::Dm::Persistent::Record::Item_T m_modelPoints[4 + 1];

public:
    /** This method is called at start-up if/when the persistent storage is 
        detected as corrupt, e,g. the CRC is bad. See Cpl::Dm::Persistent::Record 
        additional details.
     */
    bool resetData() noexcept
    {
        // Reset the boot counter and metrics
        mp::bootCounter.write( 1 );    // Default Reset counter to '1', i.e. never can be zero once persistent storage has been loaded
        mp::metrics.clearAll();
        mp::hiAlarmCounts.write( 0 );
        mp::loAlarmCounts.write( 0 );

        // Return true so that the NVRAM will be updated with the new/valid values.
        return true;
    }

protected:
    /** This method is called when the persistent storage has been successfully
        loaded at start-up.  See Cpl::Dm::Persistent::Record for additional details
     */
    void hookProcessPostRecordLoaded() noexcept
    {
        // Increment the boot counter on successful load of the storage
        // NOTE: The hookProcessPostRecordLoaded() method is CALLED BEFORE the 
        //       data Record's subscriptions to the MPs occur.  This means 
        //       monitoring the boot counter MP for change in order to update 
        //       NVRAM does NOT work (because this is only time the boot counter
        //       MP is updated). The start-up sequence performs a flush() on the 
        //       Metrics record in order to ensure that the boot counter gets 
        //       updated to NVRAM as soon as possible during the start-up 
        //       sequence.
        mp::bootCounter.increment();
    }
};

#endif  // end header latch
#ifndef Zelda_Main_Metrics_Record_h_
#define Zelda_Main_Metrics_Record_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */

#include "colony_config.h"
#include "Cpl/Dm/Persistent/Record.h"
#include "Link/mp/ModelPoints.h"
#include "Cpl/System/Trace.h"
#include "Link/Main/PostCodes.h"
#include "Link/Post/CorruptStorage.h"
#include "Link/Logging/Api.h"

/// Major Schema index for my record
#ifndef OPTION_LINK_MAIN_METRIC_RECORD_MAJOR
#define OPTION_LINK_MAIN_METRIC_RECORD_MAJOR    0
#endif

/// Minor Schema index for my record
#ifndef OPTION_LINK_MAIN_METRIC_RECORD_MINOR
#define OPTION_LINK_MAIN_METRIC_RECORD_MINOR    0
#endif

/// Groot does NOT 'fail' the resetData() method
#if defined(I_AM_GROOT) || defined(DEBUG_BUILD)
#define LINK_MAIN_METRICS_RECORD_RESET_DATA_RESULT  true

/// Zelda 'fails' the resetData() method so the error is present on the next reboot
#else
#define LINK_MAIN_METRICS_RECORD_RESET_DATA_RESULT  false
#endif

///
namespace Link {
///
namespace Main {


/** This concrete class implements the "Metrics" record for persistently
    storing the counters, run time, etc.
 */
class MetricRecord : public Cpl::Dm::Persistent::Record
{
public:
    /// Constructor
    MetricRecord( Cpl::Persistent::Chunk& chunkHandler )
        : Cpl::Dm::Persistent::Record( m_modelPoints, chunkHandler, OPTION_LINK_MAIN_METRIC_RECORD_MAJOR, OPTION_LINK_MAIN_METRIC_RECORD_MINOR )
    {
        m_modelPoints[0] = { &mp::resetCounter, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };   
        m_modelPoints[1] = { &mp::infusionNumber, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };   
        m_modelPoints[2] = { 0,0 };
    }

public:
    /// See Cpl::Dm::Persistent::Record
    bool resetData() noexcept
    {
        // NOTE: This is bad if it ever occurs once the Pump has been 
        //       provisioned. So we update the criticalPOSTFailure model
        //       point so that the Application can take the appropriate 
        //       action.
        int err = ZELDA_MAIN_POST_BASE_CORRUPT_STRORAGE + LINK_POST_CORRUPT_STRORAGE_METRICS_BLOCK;
        mp::criticalPOSTFailure.write( err );
        Cpl::Text::FString<100>  postResult;
        Link::Main::convertPostResultToString( postResult, err );
        Link::Logging::logf( Link::Logging::CriticalId::POST_FAIL, "%s", postResult.getString()  );

        // Reset all counts
        mp::resetCounter.write( 1 );    // Default Reset counter to '1', i.e. never can be zero once persistent storage has been loaded
        mp::infusionNumber.write( 0 );

        return LINK_MAIN_METRICS_RECORD_RESET_DATA_RESULT;
    }

protected:
    /// See Cpl::Dm::Persistent::Record
    void hookProcessPostRecordLoaded() noexcept
    {
        // Increment the reset counter on successful load of the counter
        mp::resetCounter.increment();    
    }

protected:
    /// List of Model Points for the Record
    Cpl::Dm::Persistent::Record::Item_T m_modelPoints[2 + 1];
};

};  // end namespace(s)
};

#endif  // end header latch
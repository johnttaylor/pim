#ifndef Cpl_MApp_Temperature_Api_h_
#define Cpl_MApp_Temperature_Api_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "colony_config.h"
#include "Cpl/MApp/MApp_.h"
#include "Cpl/Dm/Mp/Float.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Timer.h"


/// Default interval time (in milliseconds) for sampling temperature
#ifndef OPTION_CPL_MAPP_TEMPEARTURE_SAMPLE_INTERVAL_MS
#define OPTION_CPL_MAPP_TEMPEARTURE_SAMPLE_INTERVAL_MS      100
#endif

/// Default interval time (in milliseconds) for displaying temperature
#ifndef OPTION_CPL_MAPP_TEMPEARTURE_DISPLAY_INTERVAL_MS
#define OPTION_CPL_MAPP_TEMPEARTURE_DISPLAY_INTERVAL_MS     (5*1000)
#endif

/// Default temperature Units (false:=Celsius, true:=Fahrenheit)
#ifndef OPTION_CPL_MAPP_TEMPEARTURE_FAHRENHEIT
#define OPTION_CPL_MAPP_TEMPEARTURE_FAHRENHEIT              true
#endif


///
namespace Cpl {
///
namespace MApp {
///
namespace Temperature {


/** This concrete class implements MApp the polls a model point for a 
    temperature value and periodically writes the value to the trace output.
    The MApp also collects some metrics and has the option to display temperature
    in degrees Fahrenheit or Celsius. 

    NOTE: Multiple of instance of the class can be created - as long as each
          instance has a unique 'name'.

    The class is intended to be example/template for an MApp - not that it does
    anything particular noteworthy.
 */
class Api : public Cpl::MApp::MApp_, public Cpl::System::Timer
{
public:
    /// The default MApp Name
    static constexpr const char* DEFAULT_NAME = "temperature";

    /** Usage string (recommended that lines do not exceed 80 chars)
                                                         1         2         3         4         5         6         7         8
                                                12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* USAGE       = "args: [<samplems> [<displayms> [F|C]]]\n"
                                               "  <samplems>   milliseconds between samples. Default is  100ms\n"
                                               "  <displayms>  milliseconds between outputs. Default is 5000ms\n"
                                               "  F|C          'F' use Fahrenheit, 'C' use Celsius. Default is 'F'";

    /** Description string (recommended that lines do not exceed 80 chars)
                                                         1         2         3         4         5         6         7         8
                                                12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* DESCRIPTION = "Periodically Samples temperature and displays sample/metric values.";


public:
    /// Constructor. Note: The myMbox argument is only needed because the class uses a SW timer
    Api( Cpl::Container::Map<MAppApi>&    mappList,
         Cpl::Dm::MailboxServer&          myMbox,
         Cpl::Dm::Mp::Float&              srcTemperatureMp,
         const char*                      name = DEFAULT_NAME );

protected:
    /// See Cpl::MApp::Api
    void intialize_() noexcept;

    /// See Cpl::MApp::Api
    bool start_( char* args ) noexcept;

    /// See Cpl::MApp::Api
    void stop_() noexcept;

    /// See Cpl::MApp::Api
    void shutdown_() noexcept;

protected:
    /// Helper method to parse the 'command line' options
    bool parse( char* args ) noexcept;

    /// Timer expired callback
    void expired( void ) noexcept;


protected:
    /// Temperature Input.  Assumed units is Celsius
    Cpl::Dm::Mp::Float& m_temperature;

    /// Maximum Temp. sampled
    float               m_maxTemp;

    /// Minimum Temp. sampled
    float               m_minTemp;

    /// Cumulative sum Temp
    float               m_sumTemp;

    /// Number of samples
    uint32_t            m_numSamples;

    /// Sample time in msecs
    uint32_t            m_sampleMs;

    /// Display time in msecs
    uint32_t            m_displayMs;

    /// Time marker of last display action
    uint32_t            m_timeMarkerMs;

    /// When true the values are displayed in Fahrenheit
    bool                m_fahrenheit;

    /// Flag that tracks when I have invalid data
    bool                m_invalidData;
};

};      // end namespaces
};
};
#endif  // end header latch

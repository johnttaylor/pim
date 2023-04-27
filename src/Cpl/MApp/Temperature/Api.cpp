/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/atob.h"

using namespace Cpl::MApp::Temperature;

/////////////////////////////////////////////////////
Api::Api( Cpl::Container::Map<MAppApi>&    mappList,
          Cpl::Dm::MailboxServer&          myMbox,
          Cpl::Dm::Mp::Float&              srcTemperatureMp,
          const char*                      name )
    : MApp_( mappList, name, DESCRIPTION, USAGE )
    , Cpl::System::Timer( myMbox )
    , m_temperature( srcTemperatureMp )
{
}


void Api::intialize_() noexcept
{
    // Nothing needed
}

void Api::shutdown_() noexcept
{
    // Ensure that everything is stopped
    Cpl::System::Timer::stop();
}

bool Api::start_( char* args ) noexcept
{

    // Parse my command line args
    if ( m_started || !parse( args ) )
    {
        return false;
    }

    CPL_SYSTEM_TRACE_MSG( OPTION_CPL_MAPP_TRACE_SECTION, ("%s: Configuration: sampleMs=%u ms, displayMs=%u ms, units=%s",
                                                           m_name.getKeyValue(),
                                                           m_sampleMs,
                                                           m_displayMs,
                                                           m_fahrenheit ? "'F" : "'C") );

    // Housekeeping
    m_started      = true;
    m_maxTemp      = FLT_MIN;
    m_minTemp      = FLT_MAX;
    m_sumTemp      = 0.0F;
    m_numSamples   = 0;
    m_invalidData  = false;
    m_timeMarkerMs = Cpl::System::ElapsedTime::milliseconds();

    // Poll the first sample
    expired();
    return true;
}

void Api::stop_() noexcept
{
    if ( m_started )
    {
        // Stop my timer
        Cpl::System::Timer::stop();

        // Housekeeping
        m_started = false;
    }
}

bool Api::parse( char* args ) noexcept
{
    // Default the arguments
    m_sampleMs   = OPTION_CPL_MAPP_TEMPEARTURE_SAMPLE_INTERVAL_MS;
    m_displayMs  = OPTION_CPL_MAPP_TEMPEARTURE_DISPLAY_INTERVAL_MS;
    m_fahrenheit = OPTION_CPL_MAPP_TEMPEARTURE_FAHRENHEIT;

    // No arguments
    const char* arg1 = Cpl::Text::stripSpace( args );
    if ( *arg1 == '\0' )
    {
        return true;
    }

    // Sample time
    unsigned long value;
    const char*   endPtr;
    if ( !Cpl::Text::a2ul( value, arg1, 10, " ", &endPtr ) )
    {
        return false;
    }
    m_sampleMs = (uint32_t) value;

    // Display time
    const char* arg2 = Cpl::Text::stripSpace( endPtr );
    if ( *arg2 == '\0' )
    {
        return true;
    }
    if ( !Cpl::Text::a2ul( value, arg2, 10, " ", &endPtr ) )
    {
        return false;
    }
    m_displayMs = (uint32_t) value;

    // Fahrenheit/Celsius 
    const char* arg3 = Cpl::Text::stripSpace( endPtr );
    if ( *arg3 == 'F' || *arg3 == 'f' )
    {
        m_fahrenheit = true;
        return true;
    }
    else if ( *arg3 == 'C' || *arg3 == 'c' )
    {
        m_fahrenheit = false;
        return true;
    }

    // If I get here the parsing failed
    return false;
}

void Api::expired( void ) noexcept
{
    // Sample the current temperature
    float t;
    if ( m_temperature.read( t ) )
    {
        m_invalidData = false;

        // Metrics
        if ( t < m_minTemp )
        {
            m_minTemp = t;
        }
        else if ( t > m_maxTemp )
        {
            m_maxTemp = t;
        }

        // Running totals
        m_numSamples++;
        m_sumTemp += t;

        uint32_t now = Cpl::System::ElapsedTime::milliseconds();
        if ( Cpl::System::ElapsedTime::expiredMilliseconds( m_timeMarkerMs, m_displayMs, now ) || m_numSamples == 1 )
        {
            m_timeMarkerMs = now;

            float avgTemp     = m_sumTemp / m_numSamples;
            float displayTemp = t;
            float maxTemp     = m_maxTemp;
            float minTemp     = m_minTemp;
            char  units       = 'C';
            if ( m_fahrenheit )
            {
                avgTemp     = (avgTemp * (9.0F / 5.0F)) + 32.0F;
                displayTemp = (displayTemp * (9.0F / 5.0F)) + 32.0F;
                maxTemp     = (maxTemp * (9.0F / 5.0F)) + 32.0F;
                minTemp     = (minTemp * (9.0F / 5.0F)) + 32.0F;
                units       = 'F';
            }

            CPL_SYSTEM_TRACE_MSG( OPTION_CPL_MAPP_TRACE_SECTION, ("%s: %g '%c, avg: %g '%c, min: %g '%c, max: %g '%c",
                                                                   m_name.getKeyValue(),
                                                                   displayTemp, units,
                                                                   avgTemp, units,
                                                                   minTemp, units,
                                                                   maxTemp, units) );
        }
    }
    // Only print an error message on the transition
    else if ( !m_invalidData )
    {
        m_invalidData = true;
        CPL_SYSTEM_TRACE_MSG( OPTION_CPL_MAPP_TRACE_SECTION, ("%s: <data invalid>", m_name.getKeyValue()) );
    }

    // Restart my timer
    Cpl::System::Timer::start( m_sampleMs );
}


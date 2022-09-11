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


#include "Trace.h"
#include "Private_.h"
#include "Cpl/Text/FString.h"
#include <string.h>


/// 
using namespace Cpl::System;

#define NUM_THREAD_FILTERS_     4

typedef Cpl::Text::FString<OPTION_CPL_SYSTEM_TRACE_MAX_SECTION_NAME_LEN> Section_T;

static bool               threadFilterEnabled_ = false;
static bool               enabled_             = OPTION_CPL_SYSTEM_TRACE_DEFAULT_ENABLE_STATE;
static Trace::InfoLevel_T infoLevel_           = OPTION_CPL_SYSTEM_TRACE_DEFAULT_INFO_LEVEL;
static Section_T          activeSections_[OPTION_CPL_SYSTEM_TRACE_MAX_SECTIONS];
static const char*        threadFilters_[NUM_THREAD_FILTERS_];

static Cpl::Text::FString<OPTION_CPL_SYSTEM_TRACE_MAX_BUFFER> buffer_;


////////////////////////////////////////////////////////////////////////
Trace::Trace( const char* filename, int linenum, const char* funcname, const char* section, const char* scope )
    :m_filename( filename ),
    m_linenum( linenum ),
    m_funcname( funcname ),
    m_section( section ),
    m_scope( scope )
{
    if ( isSectionEnabled_( section ) && passedThreadFilter_() )
    {
        traceLocation_( section, filename, linenum, funcname );
        traceUserMsg_( "->ENTER: %s", scope );
    }
}

Trace::~Trace()
{
    if ( isSectionEnabled_( m_section ) && passedThreadFilter_() )
    {
        traceLocation_( m_section, m_filename, m_linenum, m_funcname );
        traceUserMsg_( "->EXIT:  %s", m_scope );
    }
}


////////////////////////////////////////////////////////////////////////
// NOTE The following two methods MUST be called in order AND always
//      as pair!
//
void Trace::traceLocation_( const char* section, const char* filename, int linenum, const char* funcname )
{
    // Get the current tracing level parameter
    Locks_::tracing().lock();
    Trace::InfoLevel_T infoLevel = infoLevel_;
    Locks_::tracing().unlock();

    // Serialize the output
    Locks_::tracingOutput().lock();
    buffer_ = OPTION_CPL_SYSTEM_TRACE_PREFIX_STRING;
    TracePlatform_::appendInfo( buffer_, infoLevel, section, filename, linenum, funcname );
}


void Trace::traceUserMsg_( const char* format, ... )
{
    va_list ap;
    va_start( ap, format );
    buffer_.vformatAppend( format, ap );
    va_end( ap );

    buffer_ += OPTION_CPL_SYSTEM_TRACE_SUFFIX_STRING;

    // Ensure that the suffix is ALWAYS valid when appended (at the expense of truncating the user msg)
    if ( buffer_.truncated() )
    {
        buffer_.trimRight( strlen( OPTION_CPL_SYSTEM_TRACE_SUFFIX_STRING ) );
        buffer_ += OPTION_CPL_SYSTEM_TRACE_SUFFIX_STRING;
    }

    TracePlatform_::output( buffer_ );
    Locks_::tracingOutput().unlock();
}



////////////////////////////////////////////////////////////////////////
void Trace::enable_( void )
{
    Locks_::tracing().lock();
    enabled_ = true;
    Locks_::tracing().unlock();
}


void Trace::disable_( void )
{
    Locks_::tracing().lock();
    enabled_ = false;
    Locks_::tracing().unlock();
}


bool Trace::isEnabled_( void )
{
    bool result;
    Locks_::tracing().lock();
    result = enabled_;
    Locks_::tracing().unlock();
    return result;
}


Trace::InfoLevel_T Trace::setInfoLevel_( Trace::InfoLevel_T newLevel )
{
    Locks_::tracing().lock();
    InfoLevel_T previous = infoLevel_;
    infoLevel_           = newLevel;
    Locks_::tracing().unlock();
    return previous;
}

Trace::InfoLevel_T Trace::getInfoLevel_( void ) noexcept
{
    Locks_::tracing().lock();
    InfoLevel_T current = infoLevel_;
    Locks_::tracing().unlock();
    return current;
}


bool Trace::enableSection_( const char* sectionToEnable )
{
    bool result = false;
    if ( sectionToEnable )
    {
        Locks_::tracing().lock();

        int  i;
        for ( i=0; i < OPTION_CPL_SYSTEM_TRACE_MAX_SECTIONS; i++ )
        {
            if ( activeSections_[i].isEmpty() )
            {
                activeSections_[i] = sectionToEnable;
                result            = true;
                break;
            }
        }

        Locks_::tracing().unlock();
    }

    return result;
}


void Trace::disableSection_( const char* sectionToDisable )
{
    Locks_::tracing().lock();

    // Disable all sections
    if ( sectionToDisable == 0 )
    {
        int  i;
        for ( i=0; i < OPTION_CPL_SYSTEM_TRACE_MAX_SECTIONS; i++ )
        {
            activeSections_[i].clear();
        }
    }

    // Disable a single section
    else
    {
        int  i;
        for ( i=0; i < OPTION_CPL_SYSTEM_TRACE_MAX_SECTIONS; i++ )
        {
            if ( activeSections_[i] == sectionToDisable )
            {
                activeSections_[i].clear();
                break;
            }
        }
    }

    Locks_::tracing().unlock();
}


bool Trace::isSectionEnabled_( const char* section )
{
    bool result   = false;
    Locks_::tracing().lock();
    if ( enabled_ )
    {
        int i;
        for ( i=0; i < OPTION_CPL_SYSTEM_TRACE_MAX_SECTIONS; i++ )
        {
            if ( activeSections_[i][0] != '*' )
            {
                if ( activeSections_[i] == section )
                {
                    result = true;
                    break;
                }
            }
            else
            {
                if ( activeSections_[i].isEqualSubstring( 1, activeSections_[i].length() - 1, section ) )
                {
                    result = true;
                    break;
                }
            }
        }
    }

    Locks_::tracing().unlock();
    return result;
}

unsigned Trace::getSections_( Cpl::Text::String& dst )
{
    bool     first = true;
    unsigned count = 0;
    dst.clear();

    Locks_::tracing().lock();
    int i;
    for ( i=0; i < OPTION_CPL_SYSTEM_TRACE_MAX_SECTIONS; i++ )
    {
        if ( activeSections_[i].isEmpty() == false )
        {
            if ( !first )
            {
                dst += ' ';
            }

            dst  += activeSections_[i];
            first = false;
            count++;
        }
    }

    Locks_::tracing().unlock();
    return count;
}




void Trace::setThreadFilter_( const char* threadName1,
                              const char* threadName2,
                              const char* threadName3,
                              const char* threadName4
)
{
    Locks_::tracing().lock();

    // Set filter to: ACTIVE
    threadFilterEnabled_ = true;

    // Update filter table
    threadFilters_[0] = threadName1;
    threadFilters_[1] = threadName2;
    threadFilters_[2] = threadName3;
    threadFilters_[3] = threadName4;

    Locks_::tracing().unlock();
}

void Trace::clearThreadFilter_( void )
{
    // Set filter to: OFF
    Locks_::tracing().lock();
    threadFilterEnabled_ = false;

    // Clear  filter table
    threadFilters_[0] = 0;
    threadFilters_[1] = 0;
    threadFilters_[2] = 0;
    threadFilters_[3] = 0;
    Locks_::tracing().unlock();
}


bool Trace::passedThreadFilter_( const char* threadNameToTest )
{
    bool result = true;
    Locks_::tracing().lock();

    if ( threadFilterEnabled_ )
    {
        int i;
        for ( i=0, result=false; i < NUM_THREAD_FILTERS_; i++ )
        {
            if ( threadFilters_[i] && strcmp( threadNameToTest, threadFilters_[i] ) == 0 )
            {
                result = true;
                break;
            }
        }
    }

    Locks_::tracing().unlock();
    return result;
}


unsigned Trace::getThreadFilters_( Cpl::Text::String& dst )
{
    bool     first = true;
    unsigned count = 0;
    dst.clear();

    Locks_::tracing().lock();
    int i;
    for ( i=0; i < NUM_THREAD_FILTERS_; i++ )
    {
        if ( threadFilters_[i] != 0 )
        {
            if ( !first )
            {
                dst += ' ';
            }

            dst  += threadFilters_[i];
            first = false;
            count++;
        }
    }

    Locks_::tracing().unlock();
    return count;
}

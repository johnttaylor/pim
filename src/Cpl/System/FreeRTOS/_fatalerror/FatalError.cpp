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
/*
    Implementation of the System::FatalError interface using the default Output
    stream from the Trace engine.  Side effect of this decision is even if
    Trace is not being used, the application must implement the
    Cpl::System::Trace::getDefaultOutputStream_() method.

    Notes:
        o 'Extra Info' is limited to a '@@ Fatal Error:' prefix
        o The implementation is NOT thread safe.

*/

#include "Cpl/System/FatalError.h"
#include "Cpl/System/Shutdown.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/btoa.h"
#include "Cpl/Text/FString.h"
#include "FreeRTOS.h"
#include "task.h"

using namespace Cpl::System;

#define EXTRA_INFO      "\n@@ Fatal Error: "
#define SIZET_SIZE      ((sizeof(size_t) / 4 ) * 10 + 1)

#ifndef CPL_SYSTEM_FREERTOS_FATAL_ERROR_BUFSIZE
#define CPL_SYSTEM_FREERTOS_FATAL_ERROR_BUFSIZE     128
#endif

static Cpl::Text::FString<CPL_SYSTEM_FREERTOS_FATAL_ERROR_BUFSIZE> buffer_;


////////////////////////////////////////////////////////////////////////////////
void FatalError::log( const char* message )
{
    if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
    {
        Cpl::Io::Output* ptr = Cpl::System::Trace::getDefaultOutputStream_();

        ptr->write( EXTRA_INFO );
        ptr->write( message );
        ptr->write( "\n" );

        // Allow time for the error message to be outputted
        Cpl::System::Api::sleep( 250 );
    }

    Shutdown::failure( OPTION_CPL_SYSTEM_FATAL_ERROR_EXIT_CODE );
}

void FatalError::log( const char* message, size_t value )
{
    if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
    {
        int              dummy = 0;
        Cpl::Io::Output* ptr   = Cpl::System::Trace::getDefaultOutputStream_();

        ptr->write( EXTRA_INFO );
        ptr->write( message );
        ptr->write( ". v:= " );
        ptr->write( Cpl::Text::sizetToStr( value, buffer_.getBuffer( dummy ), SIZET_SIZE, 16 ) );
        ptr->write( "\n" );

        // Allow time for the error message to be outputted
        Cpl::System::Api::sleep( 150 );
    }

    Shutdown::failure( OPTION_CPL_SYSTEM_FATAL_ERROR_EXIT_CODE );
}


void FatalError::logf( const char* format, ... )
{
    va_list ap;
    va_start( ap, format );

    if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
    {
        buffer_ = EXTRA_INFO;
        buffer_.vformatAppend( format, ap );
        Cpl::System::Trace::getDefaultOutputStream_()->write( buffer_ );
    }

    Shutdown::failure( OPTION_CPL_SYSTEM_FATAL_ERROR_EXIT_CODE );
}


////////////////////////////////////////////////////////////////////////////////
void FatalError::logRaw( const char* message )
{
    log( message );
}

void FatalError::logRaw( const char* message, size_t value )
{
    log( message, value );
}




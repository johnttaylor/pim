/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2023  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Log.h"
#include "Cpl/Text/strip.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/Itc/SyncReturnHandler.h"
#include "Cpl/Logging/Api.h"
#include <string.h>

///
using namespace Cpl::Logging::TShell;

/// Hack to get around that NOT all compilers support the "%llx" notation for printf
#if INTPTR_MAX == INT32_MAX
/// print format max integer
#define PRINTF_UINT64_FMT       "%lx"
#define PRINTF_UINT64_FMT_PAD   "%10lu" 
/// type for max integer
#define PRINTF_UINT64_TYPE   unsigned long

#elif INTPTR_MAX == INT64_MAX
/// print format max integer
#define PRINTF_UINT64_FMT      "%llx"
#define PRINTF_UINT64_FMT_PAD  "%20llu" 
/// print format max integer
#define PRINTF_UINT64_TYPE   unsigned long long
#else
#error "Environment not 32 or 64-bit."
#endif

static void createLogEntry( const char* catString, uint32_t catId, const char* msgString, uint16_t msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    Cpl::Logging::createAndAddLogEntry_( catId, catString, msgId, msgString, msgTextFormat, ap );
    va_end( ap );
}


///////////////////////////
Log::Log( Cpl::Container::Map<Cpl::TShell::Command>&                         commandList,
          Cpl::Itc::PostApi&                                                 logEntryServerMailbox,
          Cpl::Persistent::IndexedEntryServer<Cpl::Logging::EntryData_T>&    logEntryServer,
          Cpl::TShell::Security::Permission_T                                minPermLevel ) noexcept
    : Cpl::TShell::Cmd::Command( commandList, verb, minPermLevel )
    , m_logServerMailbox( logEntryServerMailbox )
    , m_logServer( logEntryServer )
{
}

bool displayRecord( uint64_t                   entryKey,
                    Cpl::Logging::EntryData_T& logEntry,
                    Cpl::TShell::Context_ &    context,
                    Cpl::Text::String&         buffer ) noexcept
{
    Cpl::Text::FString<OPTION_CPL_LOGGING_MAX_LEN_CATEGORY_ID_TEXT> cat = "<unknowm>";
    Cpl::Text::FString<OPTION_CPL_LOGGING_MAX_LEN_MESSAGE_ID_TEXT>  msg = "<unknown>";
    Cpl::Logging::getIDStrings( logEntry.category, logEntry.msgId, cat, msg );

    buffer.format( "[" PRINTF_UINT64_FMT "] (" PRINTF_UINT64_FMT_PAD ") %s:%s. %s", (PRINTF_UINT64_TYPE) entryKey, (PRINTF_UINT64_TYPE) logEntry.timestamp, cat.getString(), msg.getString(), logEntry.msgText );
    return context.writeFrame( buffer );
}


///////////////////////////
Cpl::TShell::Command::Result_T Log::execute( Cpl::TShell::Context_ & context, char* cmdString, Cpl::Io::Output & outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    unsigned                        numParms = tokens.numParameters();
    bool                            io       = true;

    // Clear Log
    if ( numParms == 2 && strcmp( tokens.getParameter( 1 ), "clear" ) == 0 )
    {
        Cpl::Persistent::ClearAllEntriesRequest::Payload            payload;
        Cpl::Itc::SyncReturnHandler                                 srh;
        Cpl::Persistent::ClearAllEntriesRequest::ClearAllEntriesMsg msg( m_logServer, payload, srh );
        m_logServerMailbox.postSync( msg );
        return payload.m_success ? Command::eSUCCESS : Command::eERROR_FAILED;
    }

    // No command arguments -->display the newest log entry
    if ( numParms == 1 )
    {
        return getEntries( context, outtext, 0, 1 ) ? Command::eSUCCESS : Command::eERROR_FAILED;
    }

    // Get all
    if ( numParms == 2 && *(tokens.getParameter( 1 )) == '*' )
    {
        return getEntries( context, outtext ) ? Command::eSUCCESS : Command::eERROR_FAILED;
    }

    // Get limited
    if ( numParms == 2 && *(tokens.getParameter( 1 )) >= '0' && *(tokens.getParameter( 1 )) <= '9' )
    {
        unsigned int  max   = 0;
        if ( Cpl::Text::a2ui( max, tokens.getParameter( 1 ) ) )
        {
            return getEntries( context, outtext, 0, max ) ? Command::eSUCCESS : Command::eERROR_FAILED;
        }
        else
        {
            outtext.format( "Failed to convert <max> (%s, %s)", tokens.getParameter( 1 ) );
            context.writeFrame( outtext );
            return eERROR_FAILED;
        }
    }

    // Get some
    if ( numParms == 3 )
    {
        unsigned int  start   = 0;
        if ( Cpl::Text::a2ui( start, tokens.getParameter( 1 ) ) )
        {
            unsigned long max = 0xFFFF;
            if ( *tokens.getParameter( 2 ) == '*' || Cpl::Text::a2ul( max, tokens.getParameter( 2 ) ) )
            {
                return getEntries( context, outtext, start, max ) ? Command::eSUCCESS : Command::eERROR_FAILED;
            }
        }
        outtext.format( "Failed to convert <nth> and/or <max> (%s, %s)", tokens.getParameter( 1 ), tokens.getParameter( 2 ) );
        context.writeFrame( outtext );
        return eERROR_FAILED;
    }

    // Undocumented write sub command
    // log write <catNumVal> <msgNumVal> "<text>"
    if ( numParms == 5 && *(tokens.getParameter( 1 )) == 'w' )
    {
        unsigned long catId = 0;
        unsigned int  msgId = 0;
        if ( Cpl::Text::a2ul( catId, tokens.getParameter( 2 ) ) &&
             Cpl::Text::a2ui( msgId, tokens.getParameter( 3 ) ) )
        {
            createLogEntry( tokens.getParameter( 2 ), catId, tokens.getParameter( 3 ), msgId, tokens.getParameter( 4 ) );
            io &= context.writeFrame( "Created log entry" );
            return io ? Command::eSUCCESS : Command::eERROR_IO;
        }
        else
        {
            outtext.format( "Failed to convert numeric IDs (%s, %s)", tokens.getParameter( 2 ), tokens.getParameter( 3 ) );
            context.writeFrame( outtext );
            return eERROR_FAILED;
        }
    }
    // If I get here -->the argument(s) where bad
    return Command::eERROR_INVALID_ARGS;
}

bool Log::getEntries( Cpl::TShell::Context_ & context, Cpl::Text::String & outtext, unsigned startIndex, unsigned maxEntries ) noexcept
{
    // Get the first entry
    Cpl::Logging::EntryData_T                   logEntry;
    Cpl::Persistent::GetLatestRequest::Payload  payload( logEntry );
    if ( !getLatestLogEntry( payload ) )
    {
        context.writeFrame( "ERROR: Unable to retrieve the latest log record" );
        return false;
    }

    // Loop through all of the entries
    Cpl::Persistent::IndexedEntryReader::EntryMarker_T marker = payload.m_markerEntryRetreived;
    unsigned                                           count  = 0;
    unsigned                                           index  = 0;
    bool                                               io     = true;
    while ( count < maxEntries && io )
    {
        // Display the record
        if ( index >= startIndex )
        {
            io &= displayRecord( marker.indexValue, logEntry, context, outtext );
            count++;
        }
        Cpl::Persistent::GetPreviousRequest::Payload  payload( logEntry, marker, marker.indexValue );
        if ( !getPreviousLogEntry( payload ) )
        {
            break;
        }
        marker = payload.m_markerEntryRetreived;
        index++;
    }

    // Display total number of entries found/displayed
    if ( io && maxEntries > 1 )
    {
        outtext.format( "%u log records", count );
        io &= context.writeFrame( outtext );
    }
    return true;
}

bool Log::getLatestLogEntry( Cpl::Persistent::GetLatestRequest::Payload & payload ) noexcept
{
    Cpl::Itc::SyncReturnHandler                     srh;
    Cpl::Persistent::GetLatestRequest::GetLatestMsg msg( m_logServer, payload, srh );
    m_logServerMailbox.postSync( msg );
    return payload.m_success;
}


bool Log::getPreviousLogEntry( Cpl::Persistent::GetPreviousRequest::Payload & payload ) noexcept
{
    Cpl::Itc::SyncReturnHandler                     srh;
    Cpl::Persistent::GetPreviousRequest::GetPreviousMsg msg( m_logServer, payload, srh );
    m_logServerMailbox.postSync( msg );
    return payload.m_success;
}

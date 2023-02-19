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

#include "Cpl/Logging/Api.h"
#include "Cpl/Logging/Private_.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/FString.h"
#include <stdarg.h>

#define SECT_   "_0test"

using namespace Cpl::Logging;


////////////////////////////////////////////////////////////////////////////////
void Cpl::Logging::initialize( Cpl::Container::RingBufferMP<EntryData_T>& logEntryFIFO,
                               uint32_t                                   categoryIdForQueueOverflow,
                               const char*                                categoryQueueOverflowText,
                               uint16_t                                   messageIdForQueueOverflow,
                               const char*                                messageQueueOverflowText ) noexcept
{
}

void Cpl::Logging::shutdown() noexcept
{
}

uint32_t Cpl::Logging::enableCategory( uint32_t categoryMask ) noexcept
{
    return 0;
}

uint32_t Cpl::Logging::disableCategory( uint32_t categoryMask ) noexcept
{
    return 0;
}

uint32_t Cpl::Logging::getCategoryEnabledMask() noexcept
{
    return 0;
}

void Cpl::Logging::setCategoryMask( uint32_t newMask ) noexcept
{
}

uint32_t g_logEntryCount = 0;


////////////////////////////////////////////////////////////////////////////////
void Cpl::Logging::createAndAddLogEntry_( uint32_t    category,
                                          const char* catIdText,
                                          uint16_t    msgId,
                                          const char* msgIdText,
                                          const char* format,
                                          va_list     ap ) noexcept
{
    g_logEntryCount++;
    Cpl::Text::FString<512> buf;
    buf.vformat( format, ap );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("%s.%s [%08lX.%02u] %s", catIdText, msgIdText, category, msgId, buf.getString()) );
}


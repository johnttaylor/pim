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

#include "Api.h"
#include "Private_.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Trace.h"

using namespace Cpl::Logging;

static bool                                       queueFull_;
static unsigned                                   overflowCount_;
static Cpl::Container::RingBufferMP<EntryData_T>* logEntryFIFO_;
static Cpl::System::Mutex                         lock_;
static uint32_t                                   categoryMask_;
static uint32_t                                   overflowCatId_;
static uint16_t                                   overflowMsgId_;
static const char*                                overflowCatText_;
static const char*                                overflowMsgText_;


////////////////////////////////////////////////////////////////////////////////
void Cpl::Logging::initialize( Cpl::Container::RingBufferMP<EntryData_T>& logEntryFIFO,
                               uint32_t                                   categoryIdForQueueOverflow,
                               const char*                                categoryQueueOverflowText,
                               uint16_t                                   messageIdForQueueOverflow,
                               const char*                                messageQueueOverflowText ) noexcept
{
    CPL_SYSTEM_ASSERT( categoryQueueOverflowText );
    CPL_SYSTEM_ASSERT( messageQueueOverflowText );

    logEntryFIFO_    = &logEntryFIFO;
    categoryMask_    = 0xFFFFFFFF;
    overflowCatId_   = categoryIdForQueueOverflow;
    overflowCatText_ = categoryQueueOverflowText;
    overflowMsgId_   = messageIdForQueueOverflow;
    overflowMsgText_ = messageQueueOverflowText;
    queueFull_ = false;
    logEntryFIFO.clearTheBuffer();
}

void Cpl::Logging::shutdown() noexcept
{
}

uint32_t Cpl::Logging::enableCategory( uint32_t categoryMask ) noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( lock_ );
    uint32_t oldMask = categoryMask_;
    categoryMask_   |= categoryMask;
    return oldMask;
}

uint32_t Cpl::Logging::disableCategory( uint32_t categoryMask ) noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( lock_ );
    uint32_t oldMask = categoryMask_;
    categoryMask_   &= ~categoryMask;
    return oldMask;
}

uint32_t Cpl::Logging::getCategoryEnabledMask() noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( lock_ );
    return categoryMask_;
}

void Cpl::Logging::setCategoryMask( uint32_t newMask ) noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( lock_ );
    categoryMask_ = newMask;
}

////////////////////////////////////////////////////////////////////////////////
inline static void startText( Cpl::Text::String& dst,
                              const char*        catIdText,
                              const char*        msgIdText )
{
    dst.format( "%.*s:%.*s. ", OPTION_CPL_LOGGING_MAX_LEN_CATEGORY_ID_TEXT, catIdText, OPTION_CPL_LOGGING_MAX_LEN_MESSAGE_ID_TEXT, msgIdText );
}


static void createAndAddOverflowEntry() noexcept
{
    // Generate entry
    EntryData_T logEntry;
    logEntry.category  = overflowCatId_;
    logEntry.msgId     = overflowMsgId_;
    logEntry.timestamp = now();

    // Format text
    Cpl::Text::FString<OPTION_CPL_LOGGING_MAX_MSG_TEXT_LEN> stringBuf;
    startText( stringBuf, overflowCatText_, overflowMsgText_ );
    stringBuf.formatAppend( "overflow count=%d", overflowCount_ );

    // Add to the FIFO and echo to trace
    logEntryFIFO_->add( logEntry );
    CPL_SYSTEM_TRACE_MSG( overflowCatText_, ("%s", stringBuf.getString()) );
}

static bool isQueFull() noexcept
{
    CPL_SYSTEM_ASSERT( logEntryFIFO_ );

    // Return immediately if not in the overflowed state
    if ( !queueFull_ )
    {
        return false;
    }

    // Has space freed up?
    unsigned available = logEntryFIFO_->getMaxItems() - logEntryFIFO_->getNumItems();
    if ( available >= OPTION_CPL_LOGGING_MIN_QUEUE_SPACE )
    {
        createAndAddOverflowEntry();
        overflowCount_ = 0;
        queueFull_     = false;
        return false;
    }

    // No space - count the number of 'dropped' log entries
    overflowCount_++;
    return true;
}


////////////////////////////////////////////////////////////////////////////////
void Cpl::Logging::createAndAddLogEntry_( uint32_t    category,
                                          const char* catIdText,
                                          uint16_t    msgId,
                                          const char* msgIdText,
                                          const char* format,
                                          va_list     ap ) noexcept
{
    Cpl::System::Mutex::ScopeBlock criticalSection( lock_ );

    // Check if enabled
    if ( (category & categoryMask_) )
    {
        // Generate entry
        EntryData_T logEntry;
        logEntry.category  = category;
        logEntry.msgId     = msgId;
        logEntry.timestamp = now();

        // Format text
        Cpl::Text::FString<OPTION_CPL_LOGGING_MAX_MSG_TEXT_LEN> stringBuf;
        startText( stringBuf, catIdText, msgIdText );
        stringBuf.vformatAppend( format, ap );

        // Manage the queue overflow state
        if ( !isQueFull() )
        {
            // Space available in the queue -->add the new entry
            logEntryFIFO_->add( logEntry ); 
            if ( logEntryFIFO_->isFull() )
            {
                queueFull_ = true;
            }
        }

        // Echo to the Trace engine (always echoed even when not added to the FIFO)
        CPL_SYSTEM_TRACE_MSG( catIdText, ("%s", stringBuf.getString()) );
    }
}


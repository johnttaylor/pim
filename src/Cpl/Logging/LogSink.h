#ifndef Cpl_Logging_LogSink_h_
#define Cpl_Logging_LogSink_h_
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
/** @file */

#include "colony_config.h"
#include "Cpl/Container/RingBufferMP.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Logging/EntryData_T.h"


/** Maximum number of entries that can be written as the result of single
    change notification
 */
#ifndef OPTION_CPL_LOGGING_LOGSINK_MAX_BATCH_WRITE
#define OPTION_CPL_LOGGING_LOGSINK_MAX_BATCH_WRITE      4
#endif

 ///
namespace Cpl {
///
namespace Logging {

/** This class is responsible for consuming the application's Log buffer and
    'dispatching' log entries.  By default, the class simply drains the
    log buffer and discards the entries.  A child class is required to provide
    a meaningful implementation of the dispatchLogEntry() method.

    FYI: The Cpl::Persistent framework provides an alternate 'log sink' that
         writes log entries to Non-volatile storage along with an a API to 
         retrieve the log entries.
 */
class LogSink : public Cpl::Itc::CloseSync
{
public:
    /// Constructor
    LogSink( Cpl::Dm::MailboxServer&                                    myMbox,
             Cpl::Container::RingBufferMP<Cpl::Logging::EntryData_T>&   incomingEntriesBuffer ) noexcept;

public:
    /// This method starts the sink (See Cpl::Itc::OpenSync)
    void request( OpenMsg& msg );

    /// This method stops the sink (See Cpl::Itc::CloseSync)
    void request( CloseMsg& msg );

protected:
    /// Element Count Change notification 
    void elementCountChanged( Cpl::Dm::Mp::Uint32& mp, Cpl::Dm::SubscriberApi& clientObserver ) noexcept;

protected:
    /// Platform specific method to save the entry 
    virtual void dispatchLogEntry( Cpl::Logging::EntryData_T& src )
    {
        // Default is to do nothing
    }

protected:
    /// Observer for change notification (to the RingBuffer)
    Cpl::Dm::SubscriberComposer<LogSink, Cpl::Dm::Mp::Uint32>   m_observerElementCount;

    /// The Log Ring Buffer
    Cpl::Container::RingBufferMP<Cpl::Logging::EntryData_T>&    m_logBuffer;

    /// Track my open state
    bool                                                        m_opened;
};


}       // end namespaces
}
#endif  // end header latch
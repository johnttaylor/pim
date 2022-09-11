#ifndef Cpl_Persistent_Record_Server_h_
#define Cpl_Persistent_Record_Server_h_
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

#include "Cpl/Persistent/Record.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Itc/CloseSync.h"

///
namespace Cpl {
///
namespace Persistent {

/** This concrete class provides an Event driven Runnable object for executing
    the read/write operation to persistent storage media for N Records.
 */
class RecordServer: public Cpl::Dm::MailboxServer, public Cpl::Itc::CloseSync
{
public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Cpl::Timer::Local Timers.

        NOTE: 'recordList' is variable length array where the last entry in the
              array MUST BE a nullptr. 
     */
    RecordServer( Record*                             recordList[], 
                  unsigned long                       timingTickInMsec = OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
                  Cpl::System::SharedEventHandlerApi* eventHandler     = 0 ) noexcept;

public:
    /// This method starts the server (See Cpl::Itc::OpenSync)
    void request( OpenMsg& msg );

    /// This method stops the server (See Cpl::Itc::CloseSync)
    void request( CloseMsg& msg );

protected:
    /** Variable length list of Records to manage.  The last item list must be
        ZERO to indicate the end-of-the list
     */
    Record** m_records;

    /// Track my open state
    bool     m_opened;
};

};      // end namespaces
};
#endif  // end header latch
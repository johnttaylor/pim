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

#include "PeriodicScheduler.h"


#define SECT_ "Cpl::Itc"


///
using namespace Cpl::Itc;

/////////////////////
PeriodicScheduler::PeriodicScheduler( Interval_T                          intervals[],
                                      Hook_T                              beginThreadProcessing,
                                      Hook_T                              endThreadProcessing,
                                      ReportSlippageFunc_T                slippageFunc,
                                      NowFunc_T                           nowFunc,
                                      IdleFunc_T                          idleFunc,
                                      unsigned long                       timingTickInMsec,
                                      Cpl::System::SharedEventHandlerApi* eventHandler ) noexcept
    : MailboxServer( timingTickInMsec, eventHandler )
    , Cpl::System::PeriodicScheduler( intervals, beginThreadProcessing, endThreadProcessing, slippageFunc, nowFunc )
    , m_idleFunc( idleFunc )
{
}


/////////////////////
void PeriodicScheduler::appRun()
{
    startEventLoop();
    beginLoop();

    bool run = true;
    while ( run )
    {
        run = waitAndProcessEvents( isPendingMessage() );
        if ( run )
        {
            bool atLeastOne = executeScheduler();
            processMessages();
            processTimers();
            if ( m_idleFunc )
            {
                (m_idleFunc)((m_nowFunc)(), atLeastOne);
            }
        }
    }

    endLoop();
    stopEventLoop();
}
